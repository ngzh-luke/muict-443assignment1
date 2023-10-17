#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fillArrayWithRandomIntegers(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand();
    }
}

int main() {
    srand(time(NULL));

    int sizes[] = {10000, 100000, 500000, 1000000, 2000000}; // sizes of the different arrays
    int num_arrays = sizeof(sizes) / sizeof(sizes[0]); 

    for (int i = 0; i < num_arrays; i++) { // For each array
        int size = sizes[i];

        // Allocate space in heap for the array
        int *array = (int *)malloc(size * sizeof(int));

        if (array == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }

        fillArrayWithRandomIntegers(array, size);

    }

    return 0;
}
