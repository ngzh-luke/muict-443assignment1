#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <mpi.h>

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

void rankSort(int *localArr, int size, int *sortedArr)
{
    
    int rank, i, j;
    for (i = 0; i < size; i++)
    {
        rank = 1;
        for (j = 0; j < size; j++)
        {
            if (localArr[j] < localArr[i])
            {
                rank++;
            }
        }
        sortedArr[rank - 1] = localArr[i];
    }

    return;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    // Timer variables
    clock_t start_time, end_time;
    double execution_time;

    int sizes[] = {100}; // Sizes of the different arrays
    int num_arrays = sizeof(sizes) / sizeof(sizes[0]);

    int size, i, j;
    for (i = 0; i < num_arrays; i++)
    { // For each array
        size = sizes[i];

        // Allocate space in the heap for the local array
        int *localArray = (int *)malloc(size * sizeof(int));
        if (localArray == NULL)
        {
            fprintf(stderr, "Memory allocation failed for localArray\n");
            return 1;
        }

        fillArrayWithRandomIntegers(localArray, size, 99999);

        // Initialize the sorted array
        int *sortedArray = (int *)malloc(size * sizeof(int));
        if (sortedArray == NULL)
        {
            fprintf(stderr, "Memory allocation failed for sortedArray\n");
            return 1;
        }

        // TIMER START ------------------------------------------------
        start_time = clock();

        // Apply RankSort to the local array
        rankSort(localArray, size, sortedArray);

        end_time = clock();
        // TIMER END --------------------------------------------------
        
        execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

        printf("Execution time for n = %d: %f seconds\n", size, execution_time);

        // Print the sorted array
        printf("Sorted Array for size %d:\n", size);
        for (j = 0; j < size; j++)
        {
            printf("%d \n", sortedArray[j]);
        }
        printf("\n");
        
        free(localArray);
        free(sortedArray);
        
    }

    return 0;
}
