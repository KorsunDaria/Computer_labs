#include <math.h>
#include <stdio.h>

double CalculateSin(double x, long N) {
  double term = x;
  double result = x;
  for (long n = 2; n <= N; ++n) {
    term *= -(x * x) / ((2 * n - 1) * (2 * n - 2));
    result += term;
  }
  return result;
}

// int main() {
//   double x = M_PI / 3;
//   long long N = 15000000000;
//   double result;

//   result = CalculateSin(x, N);

//   printf("sin(%f) = %.12f\n", x, result);

//   return 0;
// }

// gcc -O3 -target x86_64-apple-darwin -c lab3.c -o lab3_o3_x86-64.o
// objdump -d lab3_o3_x86-64.o > x86-64_o3.asm

// gcc -O0 -target x86_64-apple-darwin -c lab3.c -o lab3_o0_x86-64.o
// objdump -d lab3_o0_x86-64.o > x86-64_o0.asm

// gcc -O3 -target i386-apple-darwin -c lab3.c -o lab3_o3_x86.o
// objdump -d lab3_o3_x86.o > x86_o3.asm

// gcc -O0 -target i386-apple-darwin -c lab3.c -o lab3_o0_x86.o
// objdump -d lab3_o0_x86.o > x86_o0.asm
