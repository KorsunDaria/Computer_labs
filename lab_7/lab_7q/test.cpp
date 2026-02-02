#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>

const size_t ARRAY_SIZE = 256 * 1024 * 1024;  // 256 MB
const int RUNS = 5;

volatile long sink = 0;

int main() {
  long* arr = static_cast<long*>(aligned_alloc(4096, ARRAY_SIZE));

  // Заполняем массив
  for (size_t i = 0; i < ARRAY_SIZE / sizeof(long); ++i) {
    arr[i] = i;
  }

  std::cout << "stride_bytes,gb_per_sec\n";

  for (size_t stride_bytes = 8; stride_bytes <= 1024; stride_bytes += 4) {
    size_t stride = stride_bytes / sizeof(long);
    size_t iterations = (ARRAY_SIZE / sizeof(long)) / stride;

    double best_time = 1e300;

    for (int r = 0; r < RUNS; ++r) {
      long sum = 0;

      auto t0 = std::chrono::high_resolution_clock::now();

      // Читаем с заданным stride
      for (size_t rep = 0; rep < 10; ++rep) {
        for (size_t i = 0; i < iterations; ++i) {
          sum += arr[i * stride];
        }
      }

      auto t1 = std::chrono::high_resolution_clock::now();
      sink = sum;

      std::chrono::duration<double> dur = t1 - t0;
      if (dur.count() < best_time) best_time = dur.count();
    }

    // Вычисляем пропускную способность
    size_t bytes_read = iterations * sizeof(long) * 10;
    double gb_per_sec = (bytes_read / best_time) / (1024.0 * 1024.0 * 1024.0);

    std::cout << stride_bytes << "," << gb_per_sec << "\n";
  }

  free(arr);
  return 0;
}