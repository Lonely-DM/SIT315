#include <mpi.h>
#include <iostream>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);  // Initialize the MPI environment

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of processes

    // Point-to-point communication using MPI_Send and MPI_Recv
    if (rank == 0) {  // Master process
        std::string message = "Hello World!";
        for (int i = 1; i < size; ++i) {
            MPI_Send(message.c_str(), message.size() + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else {  // Worker processes
        char message[20];
        MPI_Recv(message, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << rank << " received message: " << message << std::endl;
    }

    // Broadcast communication using MPI_Bcast
    char broadcast_message[20] = "Hello from Master!";
    MPI_Bcast(broadcast_message, 20, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        std::cout << "Process " << rank << " received broadcast message: " << broadcast_message << std::endl;
    }

    MPI_Finalize();  // Finalize the MPI environment
    return 0;
}
