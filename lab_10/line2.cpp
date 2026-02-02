#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <pthread.h>
#include <sys/sysctl.h>

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

constexpr size_t CACHE_LINE_SIZE = 128;
constexpr size_t ARRAY_SIZE = 512;
constexpr size_t ITERATIONS = 100000000;
constexpr size_t WRITER_POS = 256;

struct ThreadData {
  char* array;
  size_t read_pos;
  std::atomic<bool> start{false};
  std::atomic<bool> stop{false};
  double time_ms = 0.0;
};

// Привязка потока к CPU на macOS
void set_cpu_affinity(int cpu) {
  thread_affinity_policy_data_t policy = {cpu};
  thread_port_t thread = pthread_mach_thread_np(pthread_self());
  thread_policy_set(thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy,
                    THREAD_AFFINITY_POLICY_COUNT);
}

// Получение количества CPU
int get_cpu_count() {
  int cpu_count;
  size_t len = sizeof(cpu_count);
  sysctlbyname("hw.logicalcpu", &cpu_count, &len, NULL, 0);
  return cpu_count;
}

void* writer_thread(void* arg) {
  ThreadData* data = static_cast<ThreadData*>(arg);

  // Привязка к CPU 1, если он существует
  int cpu_count = get_cpu_count();
  if (cpu_count > 1) {
    set_cpu_affinity(1);
  }

  while (!data->start.load(std::memory_order_acquire)) {
  }
  volatile char* ptr = &data->array[WRITER_POS];
  while (!data->stop.load(std::memory_order_acquire)) {
    *ptr = 1;
  }
  return nullptr;
}

void* reader_thread(void* arg) {
  ThreadData* data = static_cast<ThreadData*>(arg);

  // Привязка к CPU 0
  set_cpu_affinity(0);

  data->start.store(true, std::memory_order_release);

  auto start = std::chrono::high_resolution_clock::now();
  volatile char dummy = 0;
  for (size_t i = 0; i < ITERATIONS; i++) {
    dummy += data->array[data->read_pos];
  }
  auto end = std::chrono::high_resolution_clock::now();
  data->time_ms =
      std::chrono::duration<double, std::milli>(end - start).count();

  data->stop.store(true, std::memory_order_release);
  return nullptr;
}

int main() {
  std::ofstream csv_file("aligned_results.csv");
  csv_file << "offset,time_ms" << std::endl;

  // macOS не имеет aligned_alloc до версии 10.15, используем posix_memalign
  char* array;
  if (posix_memalign((void**)&array, CACHE_LINE_SIZE, ARRAY_SIZE) != 0) {
    std::cerr << "Failed to allocate aligned memory" << std::endl;
    return 1;
  }
  std::memset(array, 0, ARRAY_SIZE);

  const int min_offset = -256;
  const int max_offset = 256;

  std::cout << "CPU count: " << get_cpu_count() << std::endl;

  for (int offset = min_offset; offset <= max_offset; offset++) {
    ThreadData data;
    data.array = array;
    data.read_pos = WRITER_POS + offset;

    if (data.read_pos >= ARRAY_SIZE) continue;
    if (offset < -WRITER_POS) continue;  // Исправление для отрицательных offset

    pthread_t writer, reader;
    pthread_create(&writer, nullptr, writer_thread, &data);
    pthread_create(&reader, nullptr, reader_thread, &data);

    pthread_join(reader, nullptr);
    pthread_join(writer, nullptr);

    std::cout << "offset = " << offset << ", read_pos = " << data.read_pos
              << ", time = " << data.time_ms << " ms\n";
    csv_file << offset << "," << data.time_ms << std::endl;
  }

  csv_file.close();
  free(array);  // posix_memalign требует free
  return 0;
}