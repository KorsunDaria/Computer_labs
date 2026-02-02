#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define CACHE_LINE 128
#define STRIDE (PAGE_SIZE)
#define MAX_ENTRIES 8192  // Уменьшили до 8192
#define ITERATIONS 1000000
#define REPEAT_COUNT 100  // Количество повторений для усреднения

static inline uint64_t read_tsc(void) {
  uint64_t val;
  asm volatile("mrs %0, cntvct_el0" : "=r"(val));
  return val;
}

void flush_cache(void *addr, size_t size) {
  asm volatile("dsb sy");
  volatile char *flush_buffer = malloc(1024 * 1024);
  if (flush_buffer) {
    for (size_t i = 0; i < 1024 * 1024; i += CACHE_LINE) {
      asm volatile("ldr xzr, [%0]" ::"r"(&flush_buffer[i]));
    }
    free((void *)flush_buffer);
  }
  asm volatile("dsb sy");
}

double measure_single_run(void **pages, int entries, int iterations,
                          int stride_multiplier) {
  int effective_stride = stride_multiplier * STRIDE;

  for (int i = 0; i < entries - 1; i++) {
    int next_idx = (i + 1) % entries;
    *(void **)pages[i * stride_multiplier] =
        pages[next_idx * stride_multiplier];
  }
  *(void **)pages[(entries - 1) * stride_multiplier] = pages[0];

  void *ptr = pages[0];

  // Прогрев
  for (int i = 0; i < entries; i++) {
    ptr = *(void **)ptr;
  }

  flush_cache(pages[0], entries * effective_stride);

  uint64_t start = read_tsc();

  for (int i = 0; i < iterations; i++) {
    ptr = *(void **)ptr;
  }

  uint64_t end = read_tsc();

  asm volatile("" : : "r"(ptr));

  uint64_t total_cycles = end - start;
  return (double)total_cycles / iterations;
}

void measure_tlb_large_stride(void **pages, int entries, int iterations,
                              int stride_multiplier, FILE *csv_file) {
  double total_cycles = 0.0;

  // Многократное измерение для усреднения
  for (int run = 0; run < REPEAT_COUNT; run++) {
    double cycles =
        measure_single_run(pages, entries, iterations, stride_multiplier);
    total_cycles += cycles;
  }

  double avg_cycles = total_cycles / REPEAT_COUNT;

  // Запись в CSV (только среднее значение)
  if (csv_file) {
    fprintf(csv_file, "%d,%.3f\n", entries, avg_cycles);
    fflush(csv_file);
  }
}

int main() {
  printf("TLB Size Detection with Enhanced Precision\n");
  printf("Page size: %d, Stride: %d bytes\n", PAGE_SIZE, STRIDE);
  printf("Repeat count: %d, Iterations: %d\n", REPEAT_COUNT, ITERATIONS);
  printf("Range: 8 to 8192 entries with step 8\n\n");

  // Открываем CSV файл для записи
  FILE *csv_file = fopen("tlb_results.csv", "w");
  if (!csv_file) {
    printf("Error: Cannot create CSV file\n");
    return 1;
  }

  // Записываем заголовок CSV
  fprintf(csv_file, "entries,cycles_per_access\n");
  fflush(csv_file);

  size_t total_size = MAX_ENTRIES * 8 * STRIDE;
  printf("Allocating %lu MB of memory...\n", total_size / (1024 * 1024));

  void *buffer;
  if (posix_memalign(&buffer, PAGE_SIZE, total_size) != 0) {
    printf("Memory allocation failed\n");
    fclose(csv_file);
    return 1;
  }

  memset(buffer, 0, total_size);

  void **pages = (void **)malloc(MAX_ENTRIES * 8 * sizeof(void *));
  if (!pages) {
    printf("Pages array allocation failed\n");
    free(buffer);
    fclose(csv_file);
    return 1;
  }

  for (int i = 0; i < MAX_ENTRIES * 8; i++) {
    pages[i] = (char *)buffer + i * STRIDE;
  }

  // Меньший шаг: от 8 до 256 с шагом 8, затем до 8192 с шагом 16
  const int START = 8;
  const int END = MAX_ENTRIES;

  int total_points = 0;

  // Подсчет общего количества точек
  for (int entries = START; entries <= 256; entries += 8) {
    total_points++;
  }
  for (int entries = 256 + 16; entries <= END; entries += 16) {
    total_points++;
  }

  printf("Testing %d points from %d to %d entries...\n", total_points, START,
         END);
  printf("Progress: ");
  fflush(stdout);

  int current_point = 0;

  // Первый диапазон: 8-256 с шагом 8 (высокая детализация)
  for (int entries = START; entries <= 256; entries += 8) {
    measure_tlb_large_stride(pages, entries, ITERATIONS, 4, csv_file);
    current_point++;

    if (current_point % (total_points / 20) == 0) {
      printf("#");
      fflush(stdout);
    }
  }

  // Второй диапазон: 272-8192 с шагом 16 (средняя детализация)
  for (int entries = 272; entries <= END; entries += 16) {
    measure_tlb_large_stride(pages, entries, ITERATIONS, 4, csv_file);
    current_point++;

    if (current_point % (total_points / 20) == 0) {
      printf("#");
      fflush(stdout);
    }
  }

  printf(" Done!\n");

  free(pages);
  free(buffer);

  // Закрываем CSV файл
  fclose(csv_file);

  printf("\nTest completed successfully!\n");
  printf("Maximum tested: %d entries\n", MAX_ENTRIES);
  printf("Total measurements: %d points × %d runs = %d measurements\n",
         total_points, REPEAT_COUNT, total_points * REPEAT_COUNT);
  printf("Results saved to: tlb_results.csv\n");

  return 0;
}