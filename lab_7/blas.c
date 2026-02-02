#include </opt/homebrew/Cellar/openblas/0.3.30/include/cblas.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void sumMatrix(float* A, float* B, int N) {
  cblas_saxpy(N * N, 1.0, B, 1, A, 1);
}

void mulMatrix(float* res, float* A, float* B, int N) {
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, A, N, B,
              N, 0.0, res, N);
}

float getNorm1Matrix(float* A, int N) {
  float max = __FLT_MIN__;
  for (size_t i = 0; i < N; ++i) {
    float norm = 0;
    for (size_t j = 0; j < N; ++j) {
      norm += fabsf(A[j * N + i]);
    }
    max = norm > max ? norm : max;
  }
  return max;
}

float getNormInfMatrix(float* A, int N) {
  float max = __FLT_MIN__;
  for (size_t i = 0; i < N; ++i) {
    float norm = 0;
    for (size_t j = 0; j < N; ++j) {
      norm += fabsf(A[i * N + j]);
    }
    max = norm > max ? norm : max;
  }
  return max;
}

void calculateB(float* A, float* B, float* Im, int N, float scalar) {
  cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, N, N, N, scalar, A, N,
              Im, N, 0.0, B, N);
}

void calculateR(float* R, float* B, float* A, float* Im, int N) {
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, -1.0, B, N, A,
              N, 1.0, R, N);
}

void equalMatrix(float* A, float* B, int N) {
  for (size_t i = 0; i < N * N; ++i) {
    A[i] = B[i];
  }
}

void calculateReverseMatrix(float* res, float* A, int N, int M) {
  float* temp1 = malloc(N * N * sizeof(float));
  float* temp2 = malloc(N * N * sizeof(float));
  float* R = malloc(N * N * sizeof(float));
  float* B = malloc(N * N * sizeof(float));
  float* Im = malloc(N * N * sizeof(float));

  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      Im[i * N + j] = i == j ? 1.f : 0.f;
      R[i * N + j] = i == j ? 1.f : 0.f;
    }
  }

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  float scalar = 1 / (getNorm1Matrix(A, N) * getNormInfMatrix(A, N));
  calculateB(A, B, Im, N, scalar);
  calculateR(R, B, A, Im, N);

  if (M == 1) {
    for (size_t i = 0; i < N * N; ++i) {
      res[i] = Im[i];
    }
  } else {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        res[i * N + j] = i == j ? 1 + R[i * N + j] : R[i * N + j];
        temp1[i * N + j] = R[i * N + j];
        temp2[i * N + j] = R[i * N + j];
      }
    }
  }

  for (size_t i = 2; i < M; i++) {
    mulMatrix(temp2, temp1, R, N);
    sumMatrix(res, temp2, N);
    equalMatrix(temp1, temp2, N);
  }

  equalMatrix(temp2, res, N);
  mulMatrix(res, temp2, B, N);

  clock_gettime(CLOCK_MONOTONIC_RAW, &end);
  double time =
      end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
  printf("Time taken: %lf\n", time);

  free(temp1);
  free(temp2);
  free(R);
  free(B);
  free(Im);
}

int main() {
  int N = 2048;
  float* A = malloc(N * N * sizeof(float));
  float* res = malloc(N * N * sizeof(float));

  for (int i = 0; i < N * N; i++) {
    A[i] = (float)rand() / RAND_MAX;
  }

  calculateReverseMatrix(res, A, N, 10);

  free(A);
  free(res);
  return 0;
}

// gcc blas.c -o blas -O3 -I/usr/local/opt/openblas/include -lblas