// This kernel function squares each element of the input vector.
// It will be executed in parallel on the OpenCL device (e.g., GPU or CPU).
__kernel void square_magnitude(const int size, __global int* v) {
    
    // Each thread gets a unique index representing its position in the global space
    const int globalIndex = get_global_id(0);   

    // Uncomment to see the index each processing element works on
    // printf("Kernel process index :(%d)\n ", globalIndex);

    // Perform the square operation for the element at this thread's global index
    v[globalIndex] = v[globalIndex] * v[globalIndex];
}
