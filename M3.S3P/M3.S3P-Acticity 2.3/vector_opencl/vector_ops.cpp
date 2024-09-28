#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

#define SZ 8  // 默认向量大小

// 初始化向量
void init(int *&A, int size) {
    A = new int[size];
    for (int i = 0; i < size; i++) {
        A[i] = rand() % 100;  // 随机初始化
    }
}

// 打印向量
void print(int *A, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << A[i] << " ";
    }
    std::cout << std::endl;
}

// OpenCL 配置与内核执行
void setup_openCL_and_execute(int *A, int *B, int *C, int size) {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem bufA, bufB, bufC;
    cl_int err;

    // 获取平台和设备
    err = clGetPlatformIDs(1, &platform, NULL);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);

    // 创建上下文和队列
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);

    // 创建程序
    FILE *program_handle = fopen("vector_ops.cl", "r");
    fseek(program_handle, 0, SEEK_END);
    size_t program_size = ftell(program_handle);
    rewind(program_handle);
    char *program_buffer = (char *)malloc(program_size + 1);
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);
    program_buffer[program_size] = '\0';

    program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, &program_size, &err);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "vector_add", &err);

    // 创建缓冲区并传输数据
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(int), NULL, NULL);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(int), NULL, NULL);
    bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(int), NULL, NULL);

    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, size * sizeof(int), A, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, size * sizeof(int), B, 0, NULL, NULL);

    // 设置内核参数
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

    // 执行内核
    size_t global_size = size;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    clFinish(queue);

    // 读取结果
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, size * sizeof(int), C, 0, NULL, NULL);

    // 清理
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    free(program_buffer);
}

// 多线程向量加法
void vector_add_thread(int *A, int *B, int *C, int start, int end) {
    for (int i = start; i < end; ++i) {
        C[i] = A[i] + B[i];
    }
}

void parallel_vector_add(int *A, int *B, int *C, int size, int num_threads) {
    std::vector<std::thread> threads;
    int chunk_size = size / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? size : start + chunk_size;
        threads.push_back(std::thread(vector_add_thread, A, B, C, start, end));
    }

    // 等待所有线程完成
    for (auto &t : threads) {
        t.join();
    }
}

int main() {
    // 初始化向量
    int *A, *B, *C;
    init(A, SZ);
    init(B, SZ);
    C = new int[SZ];

    std::cout << "Vector A: ";
    print(A, SZ);
    std::cout << "Vector B: ";
    print(B, SZ);

    // 使用 OpenCL 进行并行向量加法
    auto start_opencl = std::chrono::high_resolution_clock::now();
    setup_openCL_and_execute(A, B, C, SZ);
    auto end_opencl = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> opencl_elapsed = end_opencl - start_opencl;
    std::cout << "OpenCL result: ";
    print(C, SZ);
    std::cout << "OpenCL execution time: " << opencl_elapsed.count() << " s\n";

    // 使用多线程 CPU 进行向量加法
    auto start_thread = std::chrono::high_resolution_clock::now();
    parallel_vector_add(A, B, C, SZ, 4);  // 使用 4 个线程
    auto end_thread = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> thread_elapsed = end_thread - start_thread;
    std::cout << "Multithreaded CPU result: ";
    print(C, SZ);
    std::cout << "Multithreaded CPU execution time: " << thread_elapsed.count() << " s\n";

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
