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
    MPI_send(localRanks, localInterval, MPI_INT, 0, 1, MPI_COMM_WORLD);
    free(localRanks);
    return;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    MPI_Init(&argc, &argv);

    int rank, numProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);


    // Timer variables
    clock_t start_time, end_time;
    double execution_time;

    int n = 100; // Size of array

    int interval = n/numProcesses; // Number of elements per process

    // Start and end index for the processes to work on
    int start = rank*interval; 
    int end = start + interval - 1;

    if (rank == size){ // last process can end up with a different amount of elements
        end = n;
    }

    // Array for storing ranks calculated by each process for their respective range of the unsorted array
    int *localRanks = (int *)malloc((end - start) * sizeof(int));
    if (localRanks == NULL)
    {
        fprintf(stderr, "Memory allocation failed for localArray\n");
        return 1;
    }

    // Array for the unsorted array
    int *unsortedArray = (int *)malloc(n * sizeof(int));
    if (unsortedArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed for localArray\n");
        return 1;
    }

    if (rank == 0){
        fillArrayWithRandomIntegers(unsortedArray, n, 99999);
    }


    // Initialize the sorted array
    int *sortedArray = (int *)malloc(n * sizeof(int));
    if (sortedArray == NULL)
    {
        fprintf(stderr, "Memory allocation failed for sortedArray\n");
        return 1;
    }
    
    // Broadcast the whole unsorted array to all processes
    MPI_Bcast(unsortedArray, n, MPI_INT, 0, MPI_COMM_WORLD);


    // TIMER START ------------------------------------------------
    if (rank == 0){
        start_time = clock();
    }

    rankSort(unsortedArray, localRanks, n, start, end);


    /*
    Collecting the ranks:
    for i < size:
        mpi_recv(localarray, i)
        put it into sortedarray
    */

    // P0 gathers local ranks and sorts the unsorted array
    if (rank == 0){
        int i, j, x;
        for (i = 0; i < numProcesses; i++) // i = process number
        {
            // Neccessary variables for correcly placing numbers into their ranks
            int localStart = i*interval;
            int localEnd = localStart + interval - 1;
            if (i == size){
                end = n;
            }
            int localInterval = localEnd - localStart;

            // Array for p0 to receive localRanks from
            int *ranksFromProcesses = (int *)malloc(localInterval * sizeof(int));
            if (ranksFromProcesses == NULL)
            {
                fprintf(stderr, "Memory allocation failed for ranksFromProcesses\n");
                return 1;
            }

            // Receive ranks of each processors intervals
            MPI_recv(ranksFromProcesses, localInterval, MPI_INT, i, 1, MPI_COMM_WORLD);

            // Finally sort the array
            x = 0;
            for (j = localStart; j < localEnd; j++)
            {
                sortedArray[localRanks[x]] = unsortedArray[j]
                x++;
            }
        }
        free(ranksFromProcesses);
        free(unsortedArray);
    }
    
    
    // Have all processes come to this point in the code before continuing, so that timer is correct
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0){
        // TIMER END --------------------------------------------------
        end_time = clock();
        execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Execution time for n = %d: %f seconds\n", n, execution_time);

        // Print the sorted array
        printf("Sorted Array for size %d:\n", n);

        int j;
        for (j = 0; j < numProcesses; j++)
        {
            printf("%d \n", sortedArray[j]);
        }
        printf("\n");
    }
    

    free(sortedArray);
        
    MPI_Finalize();

    return 0;
}
