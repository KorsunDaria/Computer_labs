#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matrix_multiply(float *A, float *B, float *C, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      float sum = 0.0f;
      for (int k = 0; k < N; k++) {
        sum += A[i * N + k] * B[k * N + j];
      }
      C[i * N + j] = sum;
    }
  }
}

void matrix_subtract(float *A, float *B, float *C, int N) {
  for (int i = 0; i < N * N; i++) {
    C[i] = A[i] - B[i];
  }
}

void matrix_add(float *A, float *B, float *C, int N) {
  for (int i = 0; i < N * N; i++) {
    C[i] = A[i] + B[i];
  }
}

void matrix_copy(float *src, float *dst, int N) {
  for (int i = 0; i < N * N; i++) {
    dst[i] = src[i];
  }
}

void identity_matrix(float *I, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      I[i * N + j] = (i == j) ? 1.0f : 0.0f;
    }
  }
}

float matrix_norm_inf(float *A, int N) {
  float max = 0.0f;
  for (int i = 0; i < N; i++) {
    float sum = 0.0f;
    for (int j = 0; j < N; j++) {
      sum += fabsf(A[i * N + j]);
    }
    if (sum > max) max = sum;
  }
  return max;
}

float matrix_norm_1(float *A, int N) {
  float max = 0.0f;
  for (int j = 0; j < N; j++) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
      sum += fabsf(A[i * N + j]);
    }
    if (sum > max) max = sum;
  }
  return max;
}

void matrix_inverse_series(float *A, float *result, int N, int M) {
  float *I = malloc(N * N * sizeof(float));
  float *B = malloc(N * N * sizeof(float));
  float *R = malloc(N * N * sizeof(float));
  float *temp = malloc(N * N * sizeof(float));
  float *R_power = malloc(N * N * sizeof(float));
  float *sum = malloc(N * N * sizeof(float));

  identity_matrix(I, N);

  // Calculate B = A^T / (||A||_1 * ||A||_∞)
  float norm_product = matrix_norm_1(A, N) * matrix_norm_inf(A, N);
  for (int i = 0; i < N * N; i++) {
    B[i] = A[i] / norm_product;
  }

  // Calculate R = I - A*B
  matrix_multiply(A, B, temp, N);
  matrix_subtract(I, temp, R, N);

  // Initialize sum with I
  matrix_copy(I, sum, N);

  // Initialize R_power with R
  matrix_copy(R, R_power, N);

  // Sum series: I + R + R^2 + ... + R^(M-1)
  for (int k = 1; k < M; k++) {
    // Add R_power to sum
    float *new_sum = malloc(N * N * sizeof(float));
    matrix_add(sum, R_power, new_sum, N);

    // Calculate next power: R_power = R_power * R
    float *next_power = malloc(N * N * sizeof(float));
    matrix_multiply(R_power, R, next_power, N);

    free(sum);
    free(R_power);
    sum = new_sum;
    R_power = next_power;
  }

  // Result = sum * B
  matrix_multiply(sum, B, result, N);

  free(I);
  free(B);
  free(R);
  free(temp);
  free(R_power);
  free(sum);
}

int main() {
  int N = 2048;  // Smaller size for testing
  int M = 10;

  float *A = malloc(N * N * sizeof(float));
  float *result = malloc(N * N * sizeof(float));

  // Initialize matrix A with test values
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      A[i * N + j] = (i == j) ? 2.0f : 0.1f;
    }
  }

  clock_t start = clock();
  matrix_inverse_series(A, result, N, M);
  clock_t end = clock();

  double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;

  printf("Basic version - Time: %f seconds\n", time_spent);

  free(A);
  free(result);

  return 0;
}