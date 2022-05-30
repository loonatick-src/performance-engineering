#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#ifdef LIKWID_PERFMON
#include "likwid.h"
#else
#define LIKWID_MARKER_INIT
#define LIKWID_MARKER_THREADINIT
#define LIKWID_MARKER_SWITCH
#define LIKWID_MARKER_REGISTER(regionTag)
#define LIKWID_MARKER_START(regionTag)
#define LIKWID_MARKER_STOP(regionTag)
#define LIKWID_MARKER_CLOSE
#define LIKWID_MARKER_GET(regionTag, nevents, events, time, count)
#endif

const size_t count = 1e3;


void escape(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}

void clobber() {
    __asm__ volatile("" : : : "memory");
}

int test_imbalanced(int x) {
	// Transform to a number beween 0 and 2 * pi.
	double xd = ((double)x * (2 * M_PI) / count);

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
    #pragma omp for
    for (size_t i = 0; i < count; i++) {
        arr[i] = test_imbalanced((int) (i+1));
    }

    return;
}


int main(int argc, char *argv[]) {
    int THREAD_COUNT = atoi(argv[1]);    
    omp_set_num_threads(THREAD_COUNT);
    printf("Number of threads: %d\n", THREAD_COUNT);
    
    LIKWID_MARKER_INIT;
    #pragma omp parallel 
    {
        LIKWID_MARKER_REGISTER("load_imbalance");
    }


    bool *arr = malloc(count * sizeof(bool));
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    #pragma omp parallel
    {
        LIKWID_MARKER_START("load_imbalance");
        parallel_test(arr, count);
        LIKWID_MARKER_STOP("load_imbalance");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double seconds = end.tv_sec - start.tv_sec;
    double nanoseconds = end.tv_nsec - start.tv_nsec;
    printf("runtime: %lf\n", seconds + (nanoseconds*1e-9));


    free(arr);

    LIKWID_MARKER_CLOSE;

    return 0;
}
