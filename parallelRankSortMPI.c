#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void fillArrayWithRandomIntegers(int *arr, int size, int upper)
{
    int num;
    int i;
    for (i = 0; i < size; i++)
    {
        num = (rand() % upper + 1);
        arr[i] = num;
    }
}

void rankSort(int *unsortedArray, int *localRanks, int n, int localStart, int localEnd)
{
    int rank, i, j, x;
    for (i = localStart; i < localEnd; i++)
    {
        x = 0;
        rank = 0;
        for (j = 0; j < n; j++)
        {
            if (unsortedArray[j] < unsortedArray[i])
            {
                rank++;
            }
        }

        // Array of local ranks of elements between start and end
        localRanks[x] = rank;
        x++;
    }

    int localInterval = localEnd - localStart; // Because the last process can have a different amount of elements
    if (rank != 0){
        MPI_Send(localRanks, localInterval, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    return;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    srand(time(NULL));

    MPI_Status status;

    int rank, numProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);


    // Timer variables
    clock_t start_time, end_time;
    double execution_time;


    // Size of array
    // Edit to desired number ----------------------------------------------

    int n;
    if (rank == 0){
        printf("Enter desired size of array: ");
        scanf("%d", &n);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);


    int interval = n/numProcesses; // Number of elements per process

    // Start and end index for the processes to work on
    int start = rank*interval; 
    int end = start + interval - 1;

    if (rank == (numProcesses - 1)){ // last process can end up with a different amount of elements
        end = n;
    }

    // Array for storing ranks calculated by each process for their respective range of the unsorted array
    int *localRanks = (int *)malloc((interval) * sizeof(int));
    if (localRanks == NULL)
    {
        fprintf(stderr, "Memory allocation failed for localRanks\n");
        return 1;
    }

    // Array for the unsorted array
    int *unsortedArray = (int *)malloc(n * sizeof(int));
    if (unsortedArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed for unsortedArray\n");
        return 2;
    }

    if (rank == 0){
        fillArrayWithRandomIntegers(unsortedArray, n, 99999);
    }


    // Initialize the sorted array
    int *sortedArray = (int *)malloc(n * sizeof(int));
    if (sortedArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sortedArray\n");
        return 3;
    }
    
    // Broadcast the whole unsorted array to all processes
    MPI_Bcast(unsortedArray, n, MPI_INT, 0, MPI_COMM_WORLD);


    // TIMER START ------------------------------------------------
    if (rank == 0){
        start_time = clock();
    }

    rankSort(unsortedArray, localRanks, n, start, end);


    // P0 gathers local ranks and sorts the unsorted array
    if (rank == 0){
        int i, j, x;

        // Sort p0s local ranks first
        for (j = 0; j < interval; j++)
            {
                sortedArray[localRanks[j]] = unsortedArray[j];
            }

        // Sort the rest of the processes
        for (i = 1; i < numProcesses; i++) // i = process number
        {
            // Neccessary variables for correcly placing numbers into their ranks
            int localStart = i*interval;
            int localEnd = localStart + interval - 1;
            if (i == (numProcesses - 1)){
                localEnd = n;
            }
            int localInterval = localEnd - localStart + 1;

            // Array for p0 to receive localRanks from
            int *ranksFromProcesses = (int *)malloc(localInterval * sizeof(int));
            if (ranksFromProcesses == NULL)
            {
                fprintf(stderr, "Memory allocation failed for ranksFromProcesses\n");
                return 1;
            }

            // Receive ranks of each processors intervals
            MPI_Recv(ranksFromProcesses, localInterval, MPI_INT, i, 1, MPI_COMM_WORLD, &status);

            // Finally sort the array
            x = 0;
            for (j = localStart; j < localEnd; j++)
            {
                sortedArray[ranksFromProcesses[x]] = unsortedArray[j];
                x++;
            }
            free(ranksFromProcesses);
        }
        free(localRanks);
        free(unsortedArray);
    }
    
    
    // Have all processes come to this point in the code before continuing, so that timer is correct
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0){
        // TIMER END --------------------------------------------------
        end_time = clock();
        execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Execution time for n = %d: %f seconds\n", n, execution_time);
        printf("\n");
    }
    

    free(sortedArray);
        
    MPI_Finalize();

    return 0;
}
