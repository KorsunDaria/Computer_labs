#include <sys/sysctl.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#ifdef __APPLE__
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#endif

constexpr size_t MAX_OFFSET = 256;
constexpr size_t ITERATIONS = 10000000;

struct Counter {
  std::atomic<int> value;
  char padding[256];  // Больше максимального ожидаемого размера
};

// Функция для установки аффинности потоков (кросс-платформенная)
void set_thread_affinity(std::thread& thread, int cpu_core) {
#ifdef __APPLE__
  thread_port_t mach_thread = pthread_mach_thread_np(thread.native_handle());
  thread_affinity_policy_data_t policy = {cpu_core};
  thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                    (thread_policy_t)&policy, 1);
#else
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu_core, &cpuset);
  pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
#endif
}

double measure_contention(size_t offset) {
  // Используем aligned_alloc для гарантированного выравнивания
  void* memory = aligned_alloc(64, 512);
  if (!memory) {
    std::cerr << "Failed to allocate aligned memory" << std::endl;
    return 0.0;
  }

  // Обнуляем память
  memset(memory, 0, 512);

  // Создаем счетчики
  std::atomic<int>* counter1 = new (memory) std::atomic<int>;
  std::atomic<int>* counter2 =
      new (static_cast<char*>(memory) + offset) std::atomic<int>;

  counter1->store(0);
  counter2->store(0);

  std::atomic<bool> start{false};
  std::atomic<bool> stop{false};

  auto t1 = std::thread([&]() {
    while (!start.load()) {
      std::this_thread::yield();
    }
    for (size_t i = 0; i < ITERATIONS && !stop.load(); ++i) {
      counter1->fetch_add(1, std::memory_order_relaxed);
    }
  });

  auto t2 = std::thread([&]() {
    while (!start.load()) {
      std::this_thread::yield();
    }
    for (size_t i = 0; i < ITERATIONS && !stop.load(); ++i) {
      counter2->fetch_add(1, std::memory_order_relaxed);
    }
  });

  // Пытаемся установить аффинность (на macOS может не сработать)
  try {
    set_thread_affinity(t1, 0);
    set_thread_affinity(t2, 1);
  } catch (...) {
    // Игнорируем ошибки аффинности
  }

  auto start_time = std::chrono::high_resolution_clock::now();
  start.store(true);

  // Ждем некоторое время
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  stop.store(true);
  t1.join();
  t2.join();

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration<double, std::milli>(end_time - start_time);

  // Уничтожаем объекты и освобождаем память
  counter1->~atomic();
  counter2->~atomic();
  free(memory);

  return duration.count();
}

int main() {
  std::cout << "=== Cache Line Size Detection for macOS ===\n\n";

  // Получаем реальный размер из системы
  int64_t cache_line_size = 0;
  size_t size = sizeof(cache_line_size);

  if (sysctlbyname("hw.cachelinesize", &cache_line_size, &size, NULL, 0) == 0) {
    std::cout << "System reported cache line size: " << cache_line_size
              << " bytes\n\n";
  } else {
    std::cout << "Could not determine cache line size from system\n\n";
    cache_line_size = 64;  // предположим 64 байта по умолчанию
  }

  // Измеряем contention
  std::vector<double> times;
  std::vector<size_t> offsets;

  // Тестируем ключевые смещения
  offsets.push_back(0);
  for (size_t offset = 4; offset <= MAX_OFFSET; offset += 4) {
    offsets.push_back(offset);
  }

  for (size_t offset : offsets) {
    double time = measure_contention(offset);
    times.push_back(time);
    std::cout << "Offset " << offset << " bytes: " << time << " ms"
              << std::endl;
  }

  // Анализируем результаты
  std::cout << "\n=== Analysis ===\n";

  if (times.size() > 0) {
    double baseline = times[0];  // offset 0
    std::cout << "Baseline (same cache line): " << baseline << " ms\n\n";

    // Ищем скачки производительности
    std::vector<size_t> contention_points;

    for (size_t i = 1; i < times.size(); ++i) {
      double slowdown = times[i] / baseline;
      size_t offset = offsets[i];

      std::cout << "Offset " << offset << " bytes: " << times[i]
                << " ms (slowdown: " << slowdown << "x)";

      if (slowdown > 1.3) {  // Порог для contention
        std::cout << "  <-- CONTENTION DETECTED";
        contention_points.push_back(offset);
      }

      // Проверяем совпадение с системным размером
      if (cache_line_size > 0 && offset % cache_line_size == 0) {
        std::cout << " [multiple of " << cache_line_size << "]";
      }

      std::cout << std::endl;
    }

    // Вывод результатов
    if (!contention_points.empty()) {
      std::cout << "\n=== Cache Line Size Analysis ===\n";
      std::cout << "First significant contention at: " << contention_points[0]
                << " bytes\n";

      // Проверяем кратные значения
      for (int possible_size : {32, 64, 128, 256}) {
        if (contention_points[0] >= possible_size) {
          std::cout << "Could be " << possible_size
                    << " byte cache line (detected at " << contention_points[0]
                    << ")\n";
        }
      }
    }
  }

  return 0;
}