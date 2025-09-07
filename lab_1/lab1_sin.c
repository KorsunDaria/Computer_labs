#include <math.h>
#include <stdio.h>
#include <time.h>

double CalculateSin(double x, long N) {
  double term = x;
  double result = x;
  for (long n = 2; n <= N; ++n) {
    term *= -(x * x) / ((2 * n - 1) * (2 * n - 2));
    result += term;
  }
  return result;
}

int main() {
  struct timespec start, end;
  double x = M_PI / 3;
  long N = 8000000000;

  double result;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  result = CalculateSin(x, N);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);

  double time_taken =
      (end.tv_sec - start.tv_sec) + 0.000000001 * (end.tv_nsec - start.tv_nsec);

  printf("sin(%f) = %.12f\n", x, result);
  printf("time_taken = %.6f \n", time_taken);

  return 0;
}