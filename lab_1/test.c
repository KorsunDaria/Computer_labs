#include <math.h>
#include <stdio.h>
#include <time.h>

#define NUM_MEASUREMENTS 1000

void main() {
  struct timespec start, end;
  double times[NUM_MEASUREMENTS];
  double min_time = 1e9;

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double duration =
        (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);
    times[i] = duration;
  }

  double sum = 0;
  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    sum += times[i];
  }
  double average = sum / NUM_MEASUREMENTS;

  printf("Res: %.0f нс\n", average * 1e9);
}