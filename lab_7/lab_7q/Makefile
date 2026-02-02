CC = gcc
CFLAGS = -O3 -Wall -std=c11

# Пути к OpenBLAS (для Homebrew)
INCLUDES = -I/opt/homebrew/opt/openblas/include
LIBS = -L/opt/homebrew/lib -lopenblas -lm

all: lab7_plain lab7_simd lab7_blas

lab7_plain: lab7_plain.c
	$(CC) $(CFLAGS) lab7_plain.c -o lab7_plain -lm

lab7_simd: lab7_simd.c
	$(CC) $(CFLAGS) -march=armv8-a lab7_simd.c -o lab7_simd -lm

lab7_blas: lab7_blas.c
	$(CC) $(CFLAGS) lab7_blas.c -o lab7_blas $(INCLUDES) $(LIBS)

clean:
	rm -f lab7_plain lab7_simd lab7_blas
