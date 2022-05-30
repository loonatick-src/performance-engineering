#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

const size_t count = 1e8;
const size_t THREAD_COUNT = 32;

void escape(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}

void clobbber() {
    __asm__ volatile("" : : : "memory")
}

int test_imbalanced(int x) {
	// Transform to a number beween 0 and 2 * pi.
	double xd = ((double)x * (2 * M_PI) / N);

	// Do computation, the amount depends on the value of x.
	int n = 100000 * x;

    
	for (int i = 0; i < n; i++) {
		xd = sin(xd);
		xd = asin(xd);
	}
	xd = sin(xd);

	// Test the result.
	return xd < -0.5;
}

void parallel_test(bool* arr, size_t count) {
    #pragma omp parallel for
    for (size_t i = 0; i < count; i++) {
        arr[i] = test_imbalanced((int) (i+1));
    }

    return;
}

int main() {
    bool *arr = malloc(count * sizeof(bool));

    omp_set_num_threads(THREAD_COUNT);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    parallel_test(arr, count);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
        
    struct timespec elapsed_time {
        .tv_sec = end_time.tv_sec - start_time.tv_sec;
        .tv_nsec = end_time.tv_nsec - start_time.tv_nsec;
    } 

    printf("Elapsed time: %lu seconds %ld nanoseconds\n",
            elapsed_time.tv_sec, elapsed_time.tv_nsec);

    free(arr);

    return 0;
}
