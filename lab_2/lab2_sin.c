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
  // long N = 30000000000;  // 63,4 / 22,6 / 22,4 / 22,4 / 22,4 / 95,4 / 22,5
  // long N = 25000000000;  // 52,3 / 18,7 / 18,7 / 18,6 / 18,7 / 79,5 / 18,7
  // long N = 20000000000;  // 41,3 / 15,0 / 15,0 / 14.9 / 14,9 / 63,3 / 15,0
  long N = 15000000000;  // 31,7 / 11,2 / 11,2 / 11,2 / 11,2 / 47,7 / 11,2

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