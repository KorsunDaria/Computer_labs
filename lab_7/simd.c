#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef float v4sf __attribute__((vector_size(16)));

void vec_add(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N * N; i += 4) {
    v4sf a = *((v4sf *)&A[i]);
    v4sf b = *((v4sf *)&B[i]);
    *((v4sf *)&C[i]) = a + b;
  }
}

void vec_sub(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N * N; i += 4) {
    v4sf a = *((v4sf *)&A[i]);
    v4sf b = *((v4sf *)&B[i]);
    *((v4sf *)&C[i]) = a - b;
  }
}

void vec_mul(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N * N; i += 4) {
    v4sf a = *((v4sf *)&A[i]);
    v4sf b = *((v4sf *)&B[i]);
    *((v4sf *)&C[i]) = a * b;
  }
}

void vec_div_scalar(float *C, float *A, float scalar, int N) {
  v4sf s = {scalar, scalar, scalar, scalar};
  for (int i = 0; i < N * N; i += 4) {
    v4sf a = *((v4sf *)&A[i]);
    *((v4sf *)&C[i]) = a / s;
  }
}

void matmul_simd(float *C, float *A, float *B, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      float sum = 0.0f;
     
      for (int k = 0; k < N; k += 4) {
        v4sf a_vec, b_vec, mul_vec;
       
        if (k + 3 < N) {
          a_vec = *((v4sf *)&A[i * N + k]);
          b_vec = *((v4sf *)&B[k * N + j]);
         
          mul_vec = a_vec * b_vec;
          sum += mul_vec[0] + mul_vec[1] + mul_vec[2] + mul_vec[3];
        } else {
          for (int kk = k; kk < N; kk++) {
            sum += A[i * N + kk] * B[kk * N + j];
          }
        }
      }
      C[i * N + j] = sum;
    }
  }
}

void transpose(float *At, float *A, int N) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      At[j * N + i] = A[i * N + j];
    }
  }
}

void eye(float *I, int N) {
  for (int i = 0; i < N * N; i++) I[i] = 0.0f;
  for (int i = 0; i < N; i++) I[i * N + i] = 1.0f;
}

float norm1(float *A, int N) {
  float max = 0.0f;
  for (int j = 0; j < N; j++) {
    float sum = 0.0f;
   
    int i;
    for (i = 0; i <= N - 4; i += 4) {
      v4sf a_vec = *((v4sf *)&A[i * N + j]);
      v4sf abs_vec;
     
      for (int k = 0; k < 4; k++) {
        abs_vec[k] = fabsf(a_vec[k]);
      }
      sum += abs_vec[0] + abs_vec[1] + abs_vec[2] + abs_vec[3];
    }
    for (; i < N; i++) {
      sum += fabsf(A[i * N + j]);
    }
    if (sum > max) max = sum;
  }
  return max;
}

float norm_inf(float *A, int N) {
  float max = 0.0f;
  for (int i = 0; i < N; i++) {
    float sum = 0.0f;
    int j;
    for (j = 0; j <= N - 4; j += 4) {
      v4sf a_vec = *((v4sf *)&A[i * N + j]);
      v4sf abs_vec;
      for (int k = 0; k < 4; k++) {
        abs_vec[k] = fabsf(a_vec[k]);
      }
      sum += abs_vec[0] + abs_vec[1] + abs_vec[2] + abs_vec[3];
    }

    for (; j < N; j++) {
      sum += fabsf(A[i * N + j]);
    }
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
  vec_div_scalar(B, At, n1 * ninf, N * N);

  matmul_simd(tmp, B, A, N);
  eye(I, N);
  vec_sub(R, I, tmp, N * N);

  eye(invA, N);
  for (int i = 0; i < N * N; i++) Rpow[i] = R[i];

  for (int m = 1; m < M; m++) {
    vec_add(invA, invA, Rpow, N * N);
    matmul_simd(tmp, Rpow, R, N);
    for (int i = 0; i < N * N; i++) Rpow[i] = tmp[i];
  }

  matmul_simd(tmp, B, invA, N);

  clock_t end_time = clock();
  double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

  printf("SIMD execution time for N=%d: %.4f seconds\n", N, execution_time);
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
