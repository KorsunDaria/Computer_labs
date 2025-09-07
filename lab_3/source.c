
#include <stdio.h>

double leibniz_pi(unsigned long long N) {
  double pi = 0.0;
  for (unsigned long long k = 0; k < N; ++k) {
    pi += (k % 2 == 0 ? 1.0 : -1.0) / (2 * k + 1);
  }
  return pi * 4;
}
