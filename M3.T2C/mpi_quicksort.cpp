#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>

// Quicksort function for local sorting
void quicksort(int* arr, int left, int right) {
    if (left < right) {
        int pivot = arr[left];
        int i = left;
        int j = right;
        while (i < j) {
            while (i < j && arr[j] >= pivot) --j;
            if (i < j) arr[i++] = arr[j];
            while (i < j && arr[i] <= pivot) ++i;
            if (i < j) arr[j--] = arr[i];
        }
        arr[i] = pivot;
        quicksort(arr, left, i - 1);
        quicksort(arr, i + 1, right);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100;  // Size of the array
    std::vector<int> data(n);

    // Initialize data only on root
    if (rank == 0) {
        std::generate(data.begin(), data.end(), rand);
    }

    int chunk_size = n / size;
    std::vector<int> local_data(chunk_size);

    // Scatter data among all processes
    MPI_Scatter(data.data(), chunk_size, MPI_INT, local_data.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Sort local chunk using quicksort
    quicksort(local_data.data(), 0, chunk_size - 1);

    // Gather sorted chunks back to root
    MPI_Gather(local_data.data(), chunk_size, MPI_INT, data.data(), chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Final merge on root
    if (rank == 0) {
        std::sort(data.begin(), data.end());
        std::cout << "Sorted data:\n";
        for (const int& num : data) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
    return 0;
}
