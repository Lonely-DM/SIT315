#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>


using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        // Assign a random number between 0 and 99 to each element
        vector[i] = rand() % 100;
    }
}


int main(){

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v2, *v3;

    // Record the start time for performance measurement
    auto start = high_resolution_clock::now();

    // Allocate memory for the vectors
    v1 = (int *) malloc(size * sizeof(int *));
    v2 = (int *) malloc(size * sizeof(int *));
    v3 = (int *) malloc(size * sizeof(int *));


    randomVector(v1, size);

    randomVector(v2, size);


    // Sum the elements of v1 and v2, and store the result in v3
    for (int i = 0; i < size; i++)
    {
        v3[i] = v1[i] + v2[i];
    }

    auto stop = high_resolution_clock::now();

    // Calculate the duration of the operation
    auto duration = duration_cast<microseconds>(stop - start);


    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}