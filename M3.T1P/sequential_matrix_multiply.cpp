#include <iostream>

#define N 100  // Matrix size

void matrixMultiply(int A[N][N], int B[N][N], int C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int A[N][N], B[N][N], C[N][N] = {0};

    // Initialize matrices A and B
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    // Perform matrix multiplication
    matrixMultiply(A, B, C);

    // Output a sample of the result matrix
    std::cout << "Result matrix (sample):" << std::endl;
    for (int i = 0; i < 5; i++) {  // Print only a small part for testing
        for (int j = 0; j < 5; j++) {
            std::cout << C[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
