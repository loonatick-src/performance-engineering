#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#ifdef ITS_PAPI_TIME
#include "papi.h"
#include "common.h"
#endif

const size_t count = 1e8;
const size_t THREAD_COUNT = 32;

size_t NUM_EVENT = TODO;

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

void print_values(int values[NUM_EVENT]);

int main() {
#ifdef ITS_PAPI_TIME
    int retval;
    // handle to event set
    int EventSet = PAPI_NULL;
    // list of event codes for initialising the event set
    int event_codes[NUM_EVENT] = { pls dont compile thx/* TODO */ }
    // char errstring[PAPI_MAX_STR_LEN];
    // final counter values
    // assuming overflow handling was enabled during compilation of libpapi.a
    long long values[NUM_EVENT];

    if (retval = PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
        handle_error(retval);
    }

    if ((retval=PAPI_create_eventset(&EventSet)) != PAPI_OK) {
        handle_error(retval);
    }

    if ((retval=PAPI_add_events(EventSet, event_codes, NUM_EVENT)) != PAPI_OK) {
        handle_error(retval);
    }
#endif

    bool *arr = malloc(count * sizeof(bool));

    omp_set_num_threads(THREAD_COUNT);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

#ifdef ITS_PAPI_TIME
    // START PAPI COUNTERS
    if (( retval = PAPI_start(EventSet) ) != PAPI_OK) {
        handle_error(retval);
    }
#endif
    parallel_test(arr, count);
#ifdef ITS_PAPI_TIME
    // STOP PAPI COUNTERS
    if ((retval = PAPI_stop(EventSet, values)) != PAPI_OK) {
        handle_error(retval);
    }

    print_values(values, NUM_EVENT);
#endif

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
