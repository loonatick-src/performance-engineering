#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdatomic.h>

_Thread_local unsigned int seed;

const size_t BINS = 256;
const size_t buffer_size = 1e9;

const size_t count = 1e8;
const size_t THREAD_COUNT = 32;

void escape(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}

void clobbber() {
    __asm__ volatile("" : : : "memory");
}

int generate_random_datum(unsigned int *seedp) {
    // weak typing, yes
    return (int) (BINS *
            (((double)rand_r(seedp)) / (RAND_MAX + 1.0l)));
}

void fill_buffer(int *arr, size_t buffer_size) {
    // might find false sharing here?
    #pragma omp parallel for
    for (size_t i = 0; i < buffer_size; i++) {
        arr[i] = generate_random_datum(&seed);
    }
    return;
}

void compute_hist(int *arr, size_t buffer_size, int *hist, size_t bins) {
    #pragma omp parallel for
    for (size_t i = 0; i < buffer_size; i++) {
        int hist_index = arr[i];
        #pragma omp atomic
        hist[hist_index]++; 
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

int main(void) {
    omp_set_num_threads(THREAD_COUNT);
    
    int *arr = malloc(buffer_size * sizeof(int));
    int *hist = malloc(BINS * sizeof(int));

    fill_buffer(arr, buffer_size);
    // print_buffer(arr, buffer_size);
    struct timespec start_time, end_time;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    compute_hist(arr, buffer_size, hist, BINS);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    /*
    struct timespec elapsed_time {
        .tv_sec = end_time.tv_sec - start_time.tv_sec;
        .tv_usec = end_time.tv_nsec - start_time.tv_nsec;
    };
    */

    printf("Elapsed time: %lu seconds %ld nanoseconds\n",
            end_time.tv_sec - start_time.tv_sec,
            end_time.tv_nsec - start_time.tv_nsec);

    free(arr);
    free(hist);
    return 0;
}
