#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matmul(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++) {
      float sum = 0.0f;
      for (int k = 0; k < N; k++) sum += A[i * N + k] * B[k * N + j];
      C[i * N + j] = sum;
    }
}

void matadd(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N * N; i++) C[i] = A[i] + B[i];
}

void eye(float *I, int N) {
  for (int i = 0; i < N * N; i++) I[i] = 0.0f;
  for (int i = 0; i < N; i++) I[i * N + i] = 1.0f;
}

void transpose(float *At, float *A, int N) {
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++) At[j * N + i] = A[i * N + j];
}

float norm1(float *A, int N) {
  float max = 0.0f;
  for (int j = 0; j < N; j++) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) sum += fabsf(A[i * N + j]);
    if (sum > max) max = sum;
  }
  return max;
}

float norm_inf(float *A, int N) {
  float max = 0.0f;
  for (int i = 0; i < N; i++) {
    float sum = 0.0f;
    for (int j = 0; j < N; j++) sum += fabsf(A[i * N + j]);
    if (sum > max) max = sum;
  }
  return max;
}

int main() {
  int N = 2048, M = 10;

  float *A = (float *)malloc(N * N * sizeof(float));
  float *At = (float *)malloc(N * N * sizeof(float));
  float *B = (float *)malloc(N * N * sizeof(float));
  float *R = (float *)malloc(N * N * sizeof(float));
  float *I = (float *)malloc(N * N * sizeof(float));
  float *tmp = (float *)malloc(N * N * sizeof(float));
  float *invA = (float *)malloc(N * N * sizeof(float));
  float *Rpow = (float *)malloc(N * N * sizeof(float));

  for (int i = 0; i < N * N; i++) {
    A[i] = (float)rand() / RAND_MAX;
  }

  clock_t start_time = clock();

  transpose(At, A, N);

  float n1 = norm1(A, N);
  float ninf = norm_inf(A, N);
  for (int i = 0; i < N * N; i++) B[i] = At[i] / (n1 * ninf);

  matmul(tmp, B, A, N);
  eye(I, N);
  for (int i = 0; i < N * N; i++) R[i] = I[i] - tmp[i];

  eye(invA, N);
  for (int i = 0; i < N * N; i++) Rpow[i] = R[i];
  for (int m = 1; m < M; m++) {
    matadd(invA, invA, Rpow, N);
    matmul(tmp, Rpow, R, N);
    for (int i = 0; i < N * N; i++) Rpow[i] = tmp[i];
  }

  matmul(tmp, B, invA, N);

  clock_t end_time = clock();
  double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  printf("Plain C execution time for N=%d: %.4f seconds\n", N, execution_time);

  // Освобождаем память
  free(A);
  free(At);
  free(B);
  free(R);
  free(I);
  free(tmp);
  free(invA);
  free(Rpow);

  return 0;
}