#include <iostream>
#include <vector>
#include <cstdlib>  // 用于生成随机数
#include <omp.h>    // OpenMP 头文件
#include <chrono>   // 用于计算运行时间

using namespace std;
using namespace std::chrono; // chrono 库

// 交换函数
void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

// Partition 函数
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high]; // 选择最后一个元素作为 pivot
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

// 顺序 QuickSort 函数
void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high); // 获取分割点
        quickSort(arr, low, pi - 1); // 左侧排序
        quickSort(arr, pi + 1, high); // 右侧排序
    }
}

// 并行 QuickSort 函数
void quickSortParallel(vector<int>& arr, int low, int high, int depth) {
    if (low < high) {
        int pi = partition(arr, low, high); // 获取分割点

        // 根据子数组大小决定是否并行
        if (high - low > 1000) {  // 子数组大于1000时并行
            if (depth < 6) {  // 深度控制以避免线程过多
                #pragma omp parallel sections
                {
                    #pragma omp section
                    quickSortParallel(arr, low, pi - 1, depth + 1);

                    #pragma omp section
                    quickSortParallel(arr, pi + 1, high, depth + 1);
                }
            } else {
                // 顺序执行
                quickSort(arr, low, pi - 1);
                quickSort(arr, pi + 1, high);
            }
        } else {
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }
}

int main() {
    int n = 10000000; // 数组大小设置为 10,000,000
    vector<int> arr(n);

    // 生成随机数填充数组
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000000; // 生成 0 到 9,999,999 的随机数
    }

    // 记录开始时间
    auto start = high_resolution_clock::now();

    // 调用并行版 QuickSort
    #pragma omp parallel
    {
        #pragma omp single
        quickSortParallel(arr, 0, n - 1, 0);
    }

    // 记录结束时间
    auto end = high_resolution_clock::now();

    // 计算运行时间
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Parallel QuickSort runtime: " << duration.count() << " ms" << endl;

    return 0;
}
