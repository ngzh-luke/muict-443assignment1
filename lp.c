#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function to compare for qsort
int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// Parallel Rank Sort
void parallelRankSort(int* data, int n) {
    int world_size, world_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int local_size = n / world_size;
    int* local_data = (int*)malloc(local_size * sizeof(int));

    // Scatter the data to all processes
    MPI_Scatter(data, local_size, MPI_INT, local_data, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort local data using QuickSort
    qsort(local_data, local_size, sizeof(int), compare);

    // Gather sorted local data to the root process
    MPI_Gather(local_data, local_size, MPI_INT, data, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // If the number of elements on each node is below a threshold (e.g., 1000), sort locally
    if (local_size < 1000) {
        qsort(data, n, sizeof(int), compare);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == 0) {
        int sizes[] = {10000, 100000, 500000, 1000000, 2000000};
        int num_nodes[] = {1, 4, 8, 12, 16};

        printf("Data Size\tNodes\tExecution Time (s)\n");

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                int n = sizes[i];
                int num_procs = num_nodes[j];

                int* data = (int*)malloc(n * sizeof(int));

                // Generate random data
                for (int k = 0; k < n; k++) {
                    data[k] = rand() % 100000;
                }

                // Start timing
                double start_time = MPI_Wtime();

                // Perform parallel rank sort
                parallelRankSort(data, n);

                // End timing
                double end_time = MPI_Wtime();
                double execution_time = end_time - start_time;

                printf("%d\t%d\t%.6f\n", n, num_procs, execution_time);

                free(data);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
