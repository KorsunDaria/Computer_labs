#include <cblas.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  int N = 2048, M = 10;

  // Выделяем память для больших матриц
  float *A = (float *)malloc(N * N * sizeof(float));
  float *At = (float *)malloc(N * N * sizeof(float));
  float *B = (float *)malloc(N * N * sizeof(float));
  float *I_mat = (float *)malloc(N * N * sizeof(float));
  float *R = (float *)malloc(N * N * sizeof(float));
  float *tmp = (float *)malloc(N * N * sizeof(float));
  float *invA = (float *)malloc(N * N * sizeof(float));

  // Инициализация матрицы A случайными значениями
  for (int i = 0; i < N * N; i++) {
    A[i] = (float)rand() / RAND_MAX;
  }

  clock_t start_time = clock();

  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++) At[j * N + i] = A[i * N + j];

  float n1 = 0, ninf = 0;
  for (int j = 0; j < N; j++) {
    float sum = 0;
    for (int i = 0; i < N; i++) sum += fabsf(A[i * N + j]);
    if (sum > n1) n1 = sum;
  }
  for (int i = 0; i < N; i++) {
    float sum = 0;
    for (int j = 0; j < N; j++) sum += fabsf(A[i * N + j]);
    if (sum > ninf) ninf = sum;
  }

  for (int i = 0; i < N * N; i++) B[i] = At[i] / (n1 * ninf);

  for (int i = 0; i < N * N; i++) I_mat[i] = 0.0f;
  for (int i = 0; i < N; i++) I_mat[i * N + i] = 1.0f;

  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, -1.0f, B, N,
              A, N, 1.0f, I_mat, N);
  for (int i = 0; i < N * N; i++) R[i] = I_mat[i];

  for (int i = 0; i < N * N; i++) invA[i] = I_mat[i];
  cblas_scopy(N * N, R, 1, tmp, 1);
  for (int m = 1; m < M; m++) {
    cblas_saxpy(N * N, 1.0f, tmp, 1, invA, 1);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0f, tmp,
                N, R, N, 0.0f, I_mat, N);
    cblas_scopy(N * N, I_mat, 1, tmp, 1);
  }

  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0f, B, N,
              invA, N, 0.0f, I_mat, N);

  clock_t end_time = clock();
  double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  printf("OpenBLAS execution time for N=%d: %.4f seconds\n", N, execution_time);

  // Освобождаем память
  free(A);
  free(At);
  free(B);
  free(I_mat);
  free(R);
  free(tmp);
  free(invA);

  return 0;
}