#define CL_TARGET_OPENCL_VERSION 300

#include <mpi.h>
#include <iostream>
#include <CL/cl.h>

#define N 100  // Matrix size

const char* kernelSource = R"CLC(
__kernel void matrixMultiply(__global int* A, __global int* B, __global int* C, int N) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    int sum = 0;
    for (int k = 0; k < N; k++) {
        sum += A[row * N + k] * B[k * N + col];
    }
    C[row * N + col] = sum;
}
)CLC";

void matrixMultiplyOpenCL(int A[N][N], int B[N][N], int C[N][N], int start, int end) {
    // Initialize OpenCL
    cl_platform_id platform_id = nullptr;
    cl_device_id device_id = nullptr;
    cl_context context = nullptr;
    cl_command_queue command_queue = nullptr;
    cl_program program = nullptr;
    cl_kernel kernel = nullptr;
    cl_mem a_mem_obj = nullptr, b_mem_obj = nullptr, c_mem_obj = nullptr;

    cl_int ret;
    ret = clGetPlatformIDs(1, &platform_id, nullptr);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, nullptr);
    context = clCreateContext(nullptr, 1, &device_id, nullptr, nullptr, &ret);

    // Use clCreateCommandQueueWithProperties instead of the deprecated clCreateCommandQueue
    cl_command_queue_properties props[] = {CL_QUEUE_PROPERTIES, 0, 0};
    command_queue = clCreateCommandQueueWithProperties(context, device_id, props, &ret);

    // Create memory buffers
    a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), nullptr, &ret);
    b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), nullptr, &ret);
    c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * N * sizeof(int), nullptr, &ret);

    // Copy matrices to buffer
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, N * N * sizeof(int), A, 0, nullptr, nullptr);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, N * N * sizeof(int), B, 0, nullptr, nullptr);

    // Create the kernel program from source
    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, nullptr, &ret);
    ret = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);

    // Create OpenCL kernel
    kernel = clCreateKernel(program, "matrixMultiply", &ret);

    // Set the kernel arguments
    int matrix_size = N;  // Store N in a variable
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &a_mem_obj);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &b_mem_obj);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &c_mem_obj);
    clSetKernelArg(kernel, 3, sizeof(int), &matrix_size);  // Pass the variable instead of the macro

    // Execute the kernel
    size_t global_item_size[] = {size_t(end - start), size_t(N)};
    size_t local_item_size[] = {1, 1};
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, nullptr, global_item_size, local_item_size, 0, nullptr, nullptr);

    // Copy the result back to host
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, N * N * sizeof(int), C, 0, nullptr, nullptr);

    // Clean up
    clReleaseMemObject(a_mem_obj);
    clReleaseMemObject(b_mem_obj);
    clReleaseMemObject(c_mem_obj);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
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

    // Perform matrix multiplication using OpenCL on the assigned rows
    matrixMultiplyOpenCL(A, B, C, start_row, end_row);

    // Gather the result back to the root process
    MPI_Gather(&C[start_row][0], rows_per_proc * N, MPI_INT, &C, rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Print a sample of the result matrix
        std::cout << "Result matrix (sample):\n";
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                std::cout << C[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
