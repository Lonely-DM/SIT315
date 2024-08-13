#include <iostream>
#include <vector>
#include <cstdlib>  // 用于生成随机数
#include <chrono>   // 用于时间测量
#include <fstream>  // 用于文件操作
#include <pthread.h>  // 用于pthread多线程编程
#include <thread>

using namespace std;
using namespace std::chrono;

struct ThreadData {
    const vector<vector<int>>* A;
    const vector<vector<int>>* B;
    vector<vector<int>>* C;
    int startRow;
    int endRow;
    int N;
};

// 初始化矩阵的函数，使用随机值填充矩阵
void initializeMatrix(vector<vector<int>>& matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;  // 随机值在0到99之间
        }
    }
}

// 线程函数，用于计算矩阵的部分行
void* multiplySubset(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    const vector<vector<int>>& A = *(data->A);
    const vector<vector<int>>& B = *(data->B);
    vector<vector<int>>& C = *(data->C);
    int N = data->N;

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    pthread_exit(nullptr);
    return nullptr;
}

// 使用多线程计算矩阵乘法的函数
void multiplyMatricesPthread(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int N, int numThreads) {
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int rowsPerThread = N / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i] = { &A, &B, &C, i * rowsPerThread, (i + 1) * rowsPerThread, N };
        if (i == numThreads - 1) {
            threadData[i].endRow = N;  // 处理最后一个线程的剩余行
        }
        pthread_create(&threads[i], nullptr, multiplySubset, (void*)&threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }
}

// 将矩阵输出到文件的函数
void printMatrixToFile(const vector<vector<int>>& matrix, int N, const string& filename) {
    ofstream outFile(filename);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            outFile << matrix[i][j] << " ";
        }
        outFile << endl;
    }
    outFile.close();
}

int main() {
    int N = 1000;  // 矩阵大小
    int numThreads = 4; // 线程数
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C(N, vector<int>(N));

    srand(time(0));  // 随机数生成的种子

    // 使用随机值初始化矩阵A和B
    initializeMatrix(A, N);
    initializeMatrix(B, N);

    // 使用std::chrono记录开始时间
    auto start = high_resolution_clock::now();

    // 使用pthreads进行矩阵乘法，计算矩阵C
    multiplyMatricesPthread(A, B, C, N, numThreads);

    // 使用std::chrono记录结束时间
    auto stop = high_resolution_clock::now();

    // 计算所用时间
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken for matrix multiplication (pthreads): " << duration.count() << " seconds." << endl;

    // 将结果矩阵C输出到文件
    printMatrixToFile(C, N, "matrix_output_pthread.txt");

    return 0;
}
