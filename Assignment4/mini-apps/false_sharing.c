#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdatomic.h>

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

_Thread_local unsigned int seed;

#define BINS 256
const int BUFFER_SIZE = 1e9;

const size_t count = 1e8;

void escape(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}

void clobber() {
    __asm__ volatile("" : : : "memory");
}

int generate_random_datum(unsigned int *seedp) {
    // weak typing, yes
    return (int) (BINS *
            (((double)rand_r(seedp)) / (RAND_MAX + 1.0l)));
}

void fill_buffer(int *arr, size_t buffer_size) {
    // might find false sharing here?
    #pragma omp for
    for (size_t i = 0; i < buffer_size; i++) {
        arr[i] = generate_random_datum(&seed);
    }
    return;
}

void print_buffer(int *arr, size_t buffer_size) {
    for (size_t i = 0; i < buffer_size-1; i++) {
        printf("%d ", arr[i]);
    }
    printf("%d\n", arr[buffer_size-1]);
    return;
}

int main(int argc, char *argv[]) {
    int THREAD_COUNT = atoi(argv[1]);    
    omp_set_num_threads(THREAD_COUNT);
    printf("Number of threads: %d\n", THREAD_COUNT);

    LIKWID_MARKER_INIT;
    #pragma omp parallel 
    {
        LIKWID_MARKER_REGISTER("fill");
        LIKWID_MARKER_REGISTER("hist");
    }
    
    int *arr = malloc(BUFFER_SIZE * sizeof(int));
    int hist[BINS] = {0};

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    #pragma omp parallel
    {
        LIKWID_MARKER_START("fill");
        fill_buffer(arr, BUFFER_SIZE);
        LIKWID_MARKER_STOP("fill");

        LIKWID_MARKER_START("hist");
        
        #pragma omp for
        for (size_t i = 0; i < BUFFER_SIZE; i++) {
            int hist_index = arr[i];
            #pragma omp atomic
            hist[hist_index]++; 
        }
        
        LIKWID_MARKER_STOP("hist");
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double seconds = end.tv_sec - start.tv_sec;
    double nanoseconds = end.tv_nsec - start.tv_nsec;
    printf("runtime: %lf\n", seconds + (nanoseconds*1e-9));

    free(arr);
    LIKWID_MARKER_CLOSE;
    return 0;
}
