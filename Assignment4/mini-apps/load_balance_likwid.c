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
const size_t THREAD_COUNT = 32;


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


int main() {
    LIKWID_MARKER_INIT;
    #pragma omp parallel 
    {
        LIKWID_MARKER_REGISTER("load_imbalance");
    }


    bool *arr = malloc(count * sizeof(bool));

    omp_set_num_threads(THREAD_COUNT);

    #pragma omp parallel
    {
        LIKWID_MARKER_START("load_imbalance");
        parallel_test(arr, count);
        LIKWID_MARKER_STOP("load_imbalance");
    }

    free(arr);

    LIKWID_MARKER_CLOSE;

    return 0;
}
