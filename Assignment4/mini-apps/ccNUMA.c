#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
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

// domain boundary should be crossed at > 16 threads
const size_t count = 1e9l;

void kernel(double *arr, size_t count) {
    #pragma omp for
    for (size_t i = 0; i < count; i++) {
        arr[i] += sqrt((double) i);
    }
}

int main(int argc, char *argv[]) {
    int THREAD_COUNT = atoi(argv[1]);    
    omp_set_num_threads(THREAD_COUNT);
    printf("Number of threads: %d\n", THREAD_COUNT);

    LIKWID_MARKER_INIT;
    #pragma omp parallel 
    {
        LIKWID_MARKER_REGISTER("numa");
    }

    // first-touch principle should allocate everything to the same domain
    double *array = calloc(count, sizeof(double));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    #pragma omp parallel
    {
        LIKWID_MARKER_START("numa");
        kernel(array, count);
        LIKWID_MARKER_STOP("numa");
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double seconds = end.tv_sec - start.tv_sec;
    double nanoseconds = end.tv_nsec - start.tv_nsec;
    printf("runtime: %lf\n", seconds + (nanoseconds*1e-9));


    free(array);
    LIKWID_MARKER_CLOSE;
    return 0;
}
