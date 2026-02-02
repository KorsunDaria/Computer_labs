#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define CACHE_LINE 128
#define STRIDE (PAGE_SIZE + CACHE_LINE)
#define MAX_ENTRIES 8192
#define ITERATIONS 100000
#define REPEAT_COUNT 80

static inline uint64_t read_tsc(void) {
  uint64_t val;
  asm volatile("mrs %0, cntvct_el0" : "=r"(val));
  return val;
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

  for (int i = 0; i < entries; i++) {
    ptr = *(void **)ptr;
  }

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
  double min_cycles = 1e9;  // Инициализируем очень большим значением

  for (int run = 0; run < REPEAT_COUNT; run++) {
    double cycles =
        measure_single_run(pages, entries, iterations, stride_multiplier);

    // Сохраняем минимальное значение
    if (cycles < min_cycles) {
      min_cycles = cycles;
    }
  }

  if (csv_file) {
    fprintf(csv_file, "%d,%.3f\n", entries, min_cycles);
    fflush(csv_file);
  }
}

int main() {
  FILE *csv_file = fopen("tlb_results.csv", "w");
  if (!csv_file) {
    return 1;
  }

  fprintf(csv_file, "entries,cycles_per_access\n");
  fflush(csv_file);

  size_t total_size = MAX_ENTRIES * 8 * STRIDE;

  void *buffer;
  if (posix_memalign(&buffer, PAGE_SIZE, total_size) != 0) {
    fclose(csv_file);
    return 1;
  }

  memset(buffer, 0, total_size);

  void **pages = (void **)malloc(MAX_ENTRIES * 8 * sizeof(void *));
  if (!pages) {
    free(buffer);
    fclose(csv_file);
    return 1;
  }

  for (int i = 0; i < MAX_ENTRIES * 8; i++) {
    pages[i] = (char *)buffer + i * STRIDE;
  }

  for (int entries = 8; entries <= 256; entries += 8) {
    measure_tlb_large_stride(pages, entries, ITERATIONS, 4, csv_file);
  }

  for (int entries = 272; entries <= MAX_ENTRIES; entries += 16) {
    measure_tlb_large_stride(pages, entries, ITERATIONS, 4, csv_file);
  }

  free(pages);
  free(buffer);
  fclose(csv_file);

  return 0;
}