#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// TODO: add PAPI counters


// domain boundary should be crossed at > 16 threads
const int THREAD_COUNT = 32;
const size_t count = 1e9l;

void kernel(double *arr, size_t count) {
    #pragma omp parallel for
    for (size_t i = 0; i < count; i++) {
        arr[i] += sqrt((double) i);
    }
}

int main(int argc, char *argv[]) {
    // first-touch principle should allocate everything to the same domain
    double *array = calloc(count, sizeof(double));
    omp_set_num_threads(THREAD_COUNT);

    // start timers;
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    kernel(array, count);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
     
    time_t elapsed_seconds = end_time.tv_sec - start_time.tv_sec;
    long elapsed_nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
    printf("%lu seconds, %ld nanoseconds\n", elapsed_seconds, elapsed_nanoseconds);
    free(kernel);
    return 0;
}
