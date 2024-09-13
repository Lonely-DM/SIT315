#include <iostream>
#include <vector>
#include <cstdlib>  // 用于生成随机数
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

int main() {
    int n = 10000000; // 数组大小设置为 10,000,000
    vector<int> arr(n);

    // 生成随机数填充数组
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 10000000; // 生成 0 到 9,999,999 的随机数
    }

    // 记录开始时间
    auto start = high_resolution_clock::now();

    // 调用顺序 QuickSort
    quickSort(arr, 0, n - 1);

    // 记录结束时间
    auto end = high_resolution_clock::now();

    // 计算运行时间
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "Sequential QuickSort runtime: " << duration.count() << " ms" << endl;

    return 0;
}
