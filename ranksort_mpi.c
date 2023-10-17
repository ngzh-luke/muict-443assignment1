#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void fillArrayWithRandomIntegers(int *arr, int size, int upper)
{
    for (int i = 0; i < size; i++)
    {
        int num = (rand() % upper + 1);

        arr[i] = num;
        printf("\narr[%d]=%d", i, arr[i]);
    }
}

int main()
{
    srand(time(NULL));

    int sizes[] = {10000, 100000, 500000, 1000000, 2000000}; // sizes of the different arrays
    int num_arrays = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_arrays; i++)
    { // For each array
        int size = sizes[i];

        // Allocate space in heap for the array
        int *array = (int *)malloc(size * sizeof(int));

        if (array == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }

        fillArrayWithRandomIntegers(array, size, 99999);
        // printf("\n====%d", array);
    }

    return 0;
}
