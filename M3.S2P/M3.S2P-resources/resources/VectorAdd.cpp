#include <mpi.h>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);  // Initialize the MPI environment

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of processes

    int n = 8;  // Length of the vectors
    std::vector<int> v1(n), v2(n), v3(n);  // Initialize vectors

    if (rank == 0) {
        // Initialize vectors in the master process
        for (int i = 0; i < n; ++i) {
            v1[i] = i + 1;
            v2[i] = i + 1;
        }
    }

    // Each process will handle a part of the vectors
    int chunk_size = n / size;
    std::vector<int> sub_v1(chunk_size), sub_v2(chunk_size), sub_v3(chunk_size);

    // Scatter the vectors to all processes using MPI_Scatter
    MPI_Scatter(v1.data(), chunk_size, MPI_INT, sub_v1.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2.data(), chunk_size, MPI_INT, sub_v2.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform vector addition locally in each process
    for (int i = 0; i < chunk_size; ++i) {
        sub_v3[i] = sub_v1[i] + sub_v2[i];
    }

    // Gather the results in the master process using MPI_Gather
    MPI_Gather(sub_v3.data(), chunk_size, MPI_INT, v3.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Resultant vector v3: ";
        for (int i = 0; i < n; ++i) {
            std::cout << v3[i] << " ";
        }
        std::cout << std::endl;
    }

    // Calculate the total sum using MPI_Reduce
    int local_sum = 0, total_sum = 0;
    for (int i = 0; i < chunk_size; ++i) {
        local_sum += sub_v3[i];
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Total sum of elements in v3: " << total_sum << std::endl;
    }

    MPI_Finalize();  // Finalize the MPI environment
    return 0;
}
