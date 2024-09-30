#include <mpi.h>
#include <omp.h>
#include <iostream>

#define N 100  // Matrix size

void matrixMultiply(int A[N][N], int B[N][N], int C[N][N], int start, int end) {
    #pragma omp parallel for
    for (int i = start; i < end; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int A[N][N], B[N][N], C[N][N] = {0};

    if (rank == 0) {
        // Initialize matrices A and B on rank 0
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
                B[i][j] = i - j;
            }
        }
    }

    // Broadcast matrices A and B to all processes
    MPI_Bcast(&B, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&A, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    int rows_per_proc = N / size;
    int start_row = rank * rows_per_proc;
    int end_row = (rank + 1) * rows_per_proc;

    // Perform matrix multiplication on the assigned rows
    matrixMultiply(A, B, C, start_row, end_row);

    // Gather the result back to the root process
    MPI_Gather(&C[start_row][0], rows_per_proc * N, MPI_INT, &C, rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Print a sample of the result matrix
        std::cout << "Result matrix (sample):" << std::endl;
        for (int i = 0; i < 5; i++) {  // Print only a small part for testing
            for (int j = 0; j < 5; j++) {
                std::cout << C[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
