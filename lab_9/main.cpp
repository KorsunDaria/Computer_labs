#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;
using namespace std::chrono;

uint64_t getCpuTicks() {
  auto now = high_resolution_clock::now();
  auto duration = now.time_since_epoch();
  return duration_cast<nanoseconds>(duration).count();
}

int* getArray(int n, int size, int offset) {
  int arraySize = offset * n;
  int* arr = new int[arraySize];

  for (int i = 0; i < arraySize; ++i) {
    arr[i] = 0;
  }

  for (int i = 0; i < size / n; ++i) {
    for (int j = 0; j < n - 1; ++j) {
      int current_pos = i + j * offset;
      int next_pos = i + (j + 1) * offset;
      arr[current_pos] = next_pos;
    }

    int last_pos = i + (n - 1) * offset;
    arr[last_pos] = (i + 1) % (size / n);
  }

  return arr;
}

uint64_t measure(int* array, int size) {
  volatile int k = 0;
  uint64_t start = getCpuTicks();

  for (int i = 0; i < size; i++) {
    k = array[k];
  }

  uint64_t end = getCpuTicks();
  return (end - start) / size;
}

int main() {
  const int size = (32 * 1024) / sizeof(int);
  const int offset = (16 * 1024 * 1024) / sizeof(int);
  const int N = 128;
  const int iterations = 1000;

  for (int n = 1; n < N + 1; ++n) {
    int* arr = getArray(n, size, offset);
    uint64_t minTicks = UINT64_MAX;

    for (int i = 0; i < iterations; ++i) {
      uint64_t ticks =  (arr, size);
      if (ticks < minTicks) {
        minTicks = ticks;
      }
    }

    std::cout << minTicks << std::endl;
    delete[] arr;
  }

  return 0;
}