#include <iostream>
#include <vector>
#include <cstdlib>  // 用于生成随机数
#include <chrono>   // 用于时间测量
#include <fstream>  // 用于文件操作

using namespace std;
using namespace std::chrono;

// 初始化矩阵的函数，使用随机值填充矩阵
void initializeMatrix(vector<vector<int>>& matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;  // 随机值在0到99之间
        }
    }
}

// 矩阵相乘的函数
void multiplyMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
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
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C(N, vector<int>(N));

    srand(time(0));  // 随机数生成的种子

    // 使用随机值初始化矩阵A和B
    initializeMatrix(A, N);
    initializeMatrix(B, N);

    // 使用std::chrono记录开始时间
    auto start = high_resolution_clock::now();

    // 矩阵乘法，计算矩阵C
    multiplyMatrices(A, B, C, N);

    // 使用std::chrono记录结束时间
    auto stop = high_resolution_clock::now();

    // 计算所用时间
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken for matrix multiplication (sequential): " << duration.count() << " seconds." << endl;

    // 将结果矩阵C输出到文件
    printMatrixToFile(C, N, "matrix_output.txt");

    return 0;
}
