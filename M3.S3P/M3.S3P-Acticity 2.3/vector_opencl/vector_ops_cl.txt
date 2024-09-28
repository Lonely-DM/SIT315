// OpenCL 内核：并行向量加法
__kernel void vector_add(__global const int* A, __global const int* B, __global int* C) {
    int i = get_global_id(0);  // 获取工作项的全局ID
    C[i] = A[i] + B[i];        // 执行向量加法
}
