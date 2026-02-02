#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void matmul(float *C, float *A, float *B, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < N; k++)
                sum += A[i*N + k] * B[k*N + j];
            C[i*N + j] = sum;
        }
}

void matadd(float *C, float *A, float *B, int N) {
    for (int i = 0; i < N*N; i++)
        C[i] = A[i] + B[i];
}

void eye(float *I, int N) {
    for (int i = 0; i < N*N; i++) I[i] = 0.0f;
    for (int i = 0; i < N; i++) I[i*N + i] = 1.0f;
}

void transpose(float *At, float *A, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            At[j*N + i] = A[i*N + j];
}

float norm1(float *A, int N) {
    float max = 0.0f;
    for (int j = 0; j < N; j++) {
        float sum = 0.0f;
        for (int i = 0; i < N; i++)
            sum += fabsf(A[i*N + j]);
        if (sum > max) max = sum;
    }
    return max;
}

float norm_inf(float *A, int N) {
    float max = 0.0f;
    for (int i = 0; i < N; i++) {
        float sum = 0.0f;
        for (int j = 0; j < N; j++)
            sum += fabsf(A[i*N + j]);
        if (sum > max) max = sum;
    }
    return max;
}

int main() {
    int N = 4, M = 10;
    float A[16] = {
        4, 1, 2, 0,
        1, 3, 0, 1,
        0, 2, 2, 1,
        1, 0, 1, 2
    };

    float At[16], B[16], R[16], I[16], tmp[16], invA[16];
    transpose(At, A, N);

    float n1 = norm1(A, N);
    float ninf = norm_inf(A, N);
    for (int i = 0; i < N*N; i++) B[i] = At[i] / (n1 * ninf);

    matmul(tmp, B, A, N);
    eye(I, N);
    for (int i = 0; i < N*N; i++) R[i] = I[i] - tmp[i];

    eye(invA, N);
    float Rpow[16];
    for (int i = 0; i < N*N; i++) Rpow[i] = R[i];
    for (int m = 1; m < M; m++) {
        matadd(invA, invA, Rpow, N);
        matmul(tmp, Rpow, R, N);
        for (int i = 0; i < N*N; i++) Rpow[i] = tmp[i];
    }

    matmul(tmp, B, invA, N);

    printf("Approx inverse A^-1:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%8.4f ", tmp[i*N + j]);
        printf("\n");
    }
    return 0;
}
