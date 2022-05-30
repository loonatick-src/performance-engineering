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
const int THREAD_COUNT = 32;
const size_t count = 1e9l;

void kernel(double *arr, size_t count) {
    #pragma omp for
    for (size_t i = 0; i < count; i++) {
        arr[i] += sqrt((double) i);
    }
}

int main(int argc, char *argv[]) {
    // TODO: parse threads from command line
    omp_set_num_threads(THREAD_COUNT);
    LIKWID_MARKER_INIT;
#pragma omp parallel 
    {
        LIKWID_MARKER_REGISTER;
    }

    // first-touch principle should allocate everything to the same domain
    double *array = calloc(count, sizeof(double));

    // start timers;
    // struct timespec start_time, end_time;
    // clock_gettime(CLOCK_MONOTONIC, &start_time);

#pragma omp parallel
    {
        LIKWID_MARKER_START("numa");
        kernel(array, count);
        LIKWID_MARKER_END("numa");
    }

    // clock_gettime(CLOCK_MONOTONIC, &end_time);
     
    // time_t elapsed_seconds = end_time.tv_sec - start_time.tv_sec;
    // long elapsed_nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
    // printf("%lu seconds, %ld nanoseconds\n", elapsed_seconds, elapsed_nanoseconds);
    free(array);
    LIKWID_MARKER_CLOSE;
    return 0;
}
