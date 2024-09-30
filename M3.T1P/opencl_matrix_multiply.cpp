#define CL_TARGET_OPENCL_VERSION 120 //OpenCL 1.2

#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

const int N = 100;

const char *kernelSource = 
"__kernel void matrix_multiply(__global float* A, __global float* B, __global float* C, int N) {\n"
"    int row = get_global_id(0);\n"
"    int col = get_global_id(1);\n"
"    float sum = 0.0f;\n"
"    for (int i = 0; i < N; i++) {\n"
"        sum += A[row * N + i] * B[i * N + col];\n"
"    }\n"
"    C[row * N + col] = sum;\n"
"}\n";

int main() {
    // Initialize matrices
    float *A = (float *)malloc(sizeof(float) * N * N);
    float *B = (float *)malloc(sizeof(float) * N * N);
    float *C = (float *)malloc(sizeof(float) * N * N);

    for (int i = 0; i < N * N; i++) {
        A[i] = i * 1.0f;
        B[i] = i * 1.0f;
        C[i] = 0.0f;
    }

    // Get platform
    cl_int err;
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Get device
    cl_device_id device_id;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    if (err != CL_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Create context
    cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (!context) {
        return EXIT_FAILURE;
    }

    // Create command queue
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &err);
    if (!queue) {
        clReleaseContext(context);
        return EXIT_FAILURE;
    }

    // Create memory buffers
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N * N, NULL, &err);
    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * N * N, NULL, &err);
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N * N, NULL, &err);

    if (!bufA || !bufB || !bufC) {
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return EXIT_FAILURE;
    }

    // Write data to device memory
    err = clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, sizeof(float) * N * N, A, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, sizeof(float) * N * N, B, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        clReleaseMemObject(bufA);
        clReleaseMemObject(bufB);
        clReleaseMemObject(bufC);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
        return EXIT_FAILURE;
    }

    // Create program object and compile
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &err);
    if (!program) {
        return EXIT_FAILURE;
    }

    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Create kernel object
    cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &err);
    if (!kernel || err != CL_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Set kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufC);
    clSetKernelArg(kernel, 3, sizeof(int), &N);

    // Define global work size and enqueue kernel
    size_t globalWorkSize[] = { N, N };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
    if (err) {
        return EXIT_FAILURE;
    }

    // Read results from device memory
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, sizeof(float) * N * N, C, 0, NULL, NULL);

    // Print a portion of the result matrix
    printf("Result matrix (sample):\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%f ", C[i * N + j]);
        }
        printf("\n");
    }

    // Cleanup resources
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(A);
    free(B);
    free(C);

    return 0;
}
