#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t read_tsc() {
  uint64_t val;
  asm volatile("mrs %0, cntvct_el0" : "=r"(val));
  return val;
}

void fill_forward(int* array, int N) {
  for (int i = 0; i < N; i++) array[i] = (i + 1) % N;
}

void fill_backward(int* array, int N) {
  for (int i = 0; i < N; i++) array[i] = (i == 0) ? N - 1 : i - 1;
}

void fill_random(int* array, int N) {
  for (int i = 0; i < N; i++) array[i] = i;
  for (int i = N - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

void first_pass(int* array, int N) {
  int k = 0;
  for (int i = 0; i < N; i++) {
    k = array[k];
  }
  volatile int prevent_opt = k;
}

double full_pass(int* array, int N, int K) {
  int k = 0;
  uint64_t start = read_tsc();

  for (int i = 0; i < N * K; i++) {
    k = array[k];
  }

  uint64_t end = read_tsc();
  volatile int prevent_opt = k;

  return (double)(end - start) / (N * K);
}

int get_optimal_K(int N) {
  if (N <= 1024)
    return 100;
  else if (N <= 8192)
    return 50;
  else if (N <= 65536)
    return 20;
  else
    return 5;
}

void generate_csv() {
  srand(time(0));
  FILE* file =
      fopen("/Users/dariakorsun/labs/Computer_labs/lab_8/results.csv", "w");
  printf("N,Forward,Backward,Random\n");
  fprintf(file, "N,Forward,Backward,Random\n");
  int flag = 0;
  for (int N = 41287680; N <= 2 * 16 * 32 * 256 * 1024;) {
    int* array = (int*)malloc(N * sizeof(int));
    if (array == NULL) {
      printf("Memory allocation failed for N=%d\n", N);
      break;
    }

    int K = get_optimal_K(N);

    fill_forward(array, N);
    first_pass(array, N);
    double forward_time = full_pass(array, N, K);

    fill_backward(array, N);
    first_pass(array, N);
    double backward_time = full_pass(array, N, K);

    fill_random(array, N);
    first_pass(array, N);
    double random_time = full_pass(array, N, K);

    fprintf(file, "%d,%.2f,%.2f,%.2f\n", N, forward_time, backward_time,
            random_time);
    printf("%d,%.2f,%.2f,%.2f\n", N, forward_time, backward_time, random_time);

    free(array);

    if (N < 256) {
      if (flag == 0) {
        printf("256\n");
        flag = 1;
      }
      N += 4 * 2;
    } else if (N >= 256 && N < 512) {
      flag = 0;
      N += 4 * 2;
    } else if (N >= 512 && N < 1024) {
      if (flag == 0) {
        printf("1024\n");
        flag = 1;
      }
      N += 4 * 2;
    } else if (N >= 1024 && N < 2048) {
      flag = 0;
      N += 4 * 2;
    } else if (N >= 2048 && N < 4096) {
      if (flag == 0) {
        printf("4096\n");
        flag = 1;
      }
      N += 8 * 2;
    } else if (N >= 4096 && N < 8192) {
      flag = 0;
      N += 16 * 2;
    } else if (N >= 8192 && N < 16384) {
      if (flag == 0) {
        printf("16384\n");
        flag = 1;
      }
      N += 32 * 2;
    } else if (N >= 16384 && N < 32768) {
      flag = 0;
      N += 64 * 2;
    } else if (N >= 32768 && N < 65536) {
      if (flag == 0) {
        printf("65536\n");
        flag = 1;
      }
      N += 128 * 2;
    } else if (N >= 65536 && N < 131072) {
      flag = 0;
      N += 256 * 2;
    } else if (N >= 131072 && N < 262144) {
      if (flag == 0) {
        printf("262144\n");
        flag = 1;
      }
      N += 512 * 2;
    } else if (N >= 262144 && N < 524288) {
      flag = 0;
      N += 1024 * 2;
    } else if (N >= 524288 && N < 1048576) {
      if (flag == 0) {
        printf("1048576\n");
        flag = 1;
      }
      N += 2048 * 2;
    } else if (N >= 1048576 && N < 2097152) {
      flag = 0;
      N += 8192 * 2;
    } else if (N >= 2097152 && N < 4194304) {
      if (flag == 0) {
        printf("4194304\n");
        flag = 1;
      }
      N += 16384 * 2;
    } else if (N >= 4194304) {
      flag = 0;
      N += 32768 * 16;
    }
  }
}

int main() {
  generate_csv();
  return 0;
}