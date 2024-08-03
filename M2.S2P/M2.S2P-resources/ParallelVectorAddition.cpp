#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>

using namespace std::chrono;
using namespace std;

// Function to populate a vector with random values
void randomVector(int* vector, int start, int end)
{
    for (int i = start; i < end; ++i)
    {
        vector[i] = rand() % 100;
    }
}

// Function to sum elements of two vectors into a third vector
void vectorSum(int* v1, int* v2, int* v3, int start, int end)
{
    for (int i = start; i < end; ++i)
    {
        v3[i] = v1[i] + v2[i];
    }
}

int main()
{
    unsigned long size = 100000000;
    srand(time(0));

    int* v1 = new int[size];
    int* v2 = new int[size];
    int* v3 = new int[size];

    // List of different chunk sizes to test
    vector<unsigned int> chunk_sizes = {1000, 10000, 100000, 1000000};

    for (unsigned int chunk_size : chunk_sizes)
    {
        auto start = high_resolution_clock::now();

        // Parallel generation of random vectors
        vector<thread> threads;
        int num_chunks = size / chunk_size;

        for (int i = 0; i < num_chunks; ++i)
        {
            int start_idx = i * chunk_size;
            int end_idx = (i == num_chunks - 1) ? size : start_idx + chunk_size;
            threads.push_back(thread(randomVector, v1, start_idx, end_idx));
            threads.push_back(thread(randomVector, v2, start_idx, end_idx));
        }

        for (auto& t : threads)
        {
            t.join();
        }
        threads.clear();

        // Parallel summation of vectors
        for (int i = 0; i < num_chunks; ++i)
        {
            int start_idx = i * chunk_size;
            int end_idx = (i == num_chunks - 1) ? size : start_idx + chunk_size;
            threads.push_back(thread(vectorSum, v1, v2, v3, start_idx, end_idx));
        }

        for (auto& t : threads)
        {
            t.join();
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Chunk size: " << chunk_size << ", Time taken by parallel function: " << duration.count() << " microseconds" << endl;
    }

    delete[] v1;
    delete[] v2;
    delete[] v3;

    return 0;
}
