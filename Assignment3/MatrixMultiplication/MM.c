#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define IA(y, x) A[y*n+x]
#define IB(y, x) B[y*p+x]
#define IC(y, x) C[y*p+x]

#define min(x, y) (x < y ? x : y)

#define BYTES_PER_CACHE_LINE 64

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

void matrix_mult(int m, int n, int p, float *A, float *B, float *C) {
    const int block_size = BYTES_PER_CACHE_LINE / sizeof(float);
    
    memset(C, 0, sizeof(float) * m * p);

    #pragma omp parallel for collapse(3) num_threads(16)
    for (int y = 0; y < m; y += block_size) {
        for (int x = 0; x < p; x += block_size) {
            for (int k = 0; k < n; k += block_size) {

                int block_y_start = y,
                    block_y_end = min(m, y + block_size);
                int block_x_start = x,
                    block_x_end = min(p, x + block_size);
                int block_k_start = k,
                    block_k_end = min(n, k + block_size);
                
                for (int by = block_y_start; by < block_y_end; by++) {
                    for (int bx = block_x_start; bx < block_x_end; bx++) {
                        float dot = 0;
                        for (int bk = block_k_start; bk < block_k_end; bk++) {
                            dot += IA(by, bk) * IB(bk, bx);
                        }
                        IC(by, bx) += dot;
                    }
                }
            }
        }
    }
}

int main (int argc, char** argv) {
    float *A, *B, *C;
    int m, n, p;
    
    srand(time(NULL));

    m = rand() % 1024;
    n = rand() % 1024;
    p = rand() % 1024;

    A = (float *)malloc(sizeof(float) * m * n);
    B = (float *)malloc(sizeof(float) * n * p);

    for(int i = 0; i < (m * n);i++)
	{
		A[i] = rand() % 256;
	}

    for(int j = 0; j < (n * p);j++)
	{
		B[j] = rand() % 256;
	}

    C = (float *)malloc(sizeof(float) * m * p);

    struct timespec vartime = timer_start();

    matrix_mult(m, n, p, A, B, C);

    long time_elapsed_nanos = timer_end(vartime); 

    printf("Time taken by kernel for matrixes of %d by %d by %d: %ld\n", m, n, p, time_elapsed_nanos); 

    free(A);
    free(B);
    free(C);

    return 0;
}