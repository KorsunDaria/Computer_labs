#include <pthread.h>
#include <sched.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>

#ifdef __APPLE__
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#endif

constexpr size_t ARRAY_SIZE = 256;
constexpr size_t ITERATIONS = 100000000;
constexpr size_t WRITER_POS = 128;

struct alignas(64) ThreadData {
  char* array;
  size_t read_pos;
  std::atomic<bool> start{false};
  std::atomic<bool> stop{false};
  double time_ms;
};

void set_cpu_affinity(int cpu) {
#ifdef __APPLE__
  // Реализация для macOS
  thread_port_t thread = pthread_mach_thread_np(pthread_self());
  thread_affinity_policy_data_t policy = {cpu};
  thread_policy_set(thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy,
                    1);
#else
  // Реализация для Linux
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
#endif
}

void* writer_thread(void* arg) {
  ThreadData* data = static_cast<ThreadData*>(arg);
  set_cpu_affinity(1);
  while (!data->start.load()) {
  }
  volatile char* ptr = &data->array[WRITER_POS];
  while (!data->stop.load()) {
    *ptr = 1;
  }
  return nullptr;
}

void* reader_thread(void* arg) {
  ThreadData* data = static_cast<ThreadData*>(arg);
  set_cpu_affinity(0);

  data->start.store(true);

  auto start = std::chrono::high_resolution_clock::now();
  volatile char dummy = 0;
  for (size_t i = 0; i < ITERATIONS; i++) {
    dummy += data->array[data->read_pos];
  }
  auto end = std::chrono::high_resolution_clock::now();
  data->time_ms =
      std::chrono::duration<double, std::milli>(end - start).count();

  data->stop.store(true);
  return nullptr;
}

int main() {
  std::ofstream csv_file("results.csv");
  csv_file << "position,time_ms" << std::endl;

  char* array = new char[ARRAY_SIZE];
  std::memset(array, 0, ARRAY_SIZE);

  for (size_t pos = 0; pos < ARRAY_SIZE; pos++) {
    ThreadData data;
    data.array = array;
    data.read_pos = pos;

    pthread_t writer, reader;
    pthread_create(&writer, nullptr, writer_thread, &data);
    pthread_create(&reader, nullptr, reader_thread, &data);

    pthread_join(reader, nullptr);
    pthread_join(writer, nullptr);

    std::cout << '(' << pos << "," << data.time_ms << "),\n";
    csv_file << pos << "," << data.time_ms << std::endl;
  }

  csv_file.close();
  delete[] array;
  return 0;
}