#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
    int N = 4, M = 10;
    float A[16] = {
        4, 1, 2, 0,
        1, 3, 0, 1,
        0, 2, 2, 1,
        1, 0, 1, 2
    };
    float At[16], B[16], I[16], R[16], tmp[16], invA[16];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            At[j*N + i] = A[i*N + j];

    float n1 = 0, ninf = 0;
    for (int j = 0; j < N; j++) {
        float sum = 0;
        for (int i = 0; i < N; i++) sum += fabsf(A[i*N+j]);
        if (sum > n1) n1 = sum;
    }
    for (int i = 0; i < N; i++) {
        float sum = 0;
        for (int j = 0; j < N; j++) sum += fabsf(A[i*N+j]);
        if (sum > ninf) ninf = sum;
    }

    for (int i = 0; i < N*N; i++) B[i] = At[i] / (n1 * ninf);

    for (int i = 0; i < N*N; i++) I[i] = 0.0f;
    for (int i = 0; i < N; i++) I[i*N + i] = 1.0f;

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N, -1.0f, B, N, A, N, 1.0f, I, N);
    for (int i = 0; i < N*N; i++) R[i] = I[i];

    for (int i = 0; i < N*N; i++) invA[i] = I[i];
    cblas_scopy(N*N, R, 1, tmp, 1);
    for (int m = 1; m < M; m++) {
        cblas_saxpy(N*N, 1.0f, tmp, 1, invA, 1);
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                    N, N, N, 1.0f, tmp, N, R, N, 0.0f, I, N);
        cblas_scopy(N*N, I, 1, tmp, 1);
    }

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                N, N, N, 1.0f, B, N, invA, N, 0.0f, I, N);

    printf("Inverse matrix (OpenBLAS):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%8.4f ", I[i*N + j]);
        printf("\n");
    }
    return 0;
}
