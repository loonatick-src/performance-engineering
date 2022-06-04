#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <cmath>
#include <cstdlib>
#include <benchmark/benchmark.h>
#include <numeric>

double GLOBAL_DOUBLE;

thread_local unsigned int seed;

#define PE_REPS 100


static void BM_Sqrt(benchmark::State& state) {
    auto x = random_double_r(1.0e8, 1.0e9, &seed);
    auto y = GLOBAL_DOUBLE;
    for (auto _: state) {
        y = sqrt(x);
        clobber();
    }
}

BENCHMARK(BM_Sqrt)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);

/*
// divide final result by 8
static void BM_SqrtRepeat_8(benchmark::State& state) {
    auto x = random_double_r(1.0e6, 1.0e7, &seed);
    for (auto _ : state) {
        // REPEAT_8(x = sqrt(x));
        // escape(&x);
        for (int i = 0; i < 8; i++) {
            GLOBAL_DOUBLE = sqrt(x);
            clobber();
        }
    }
}

BENCHMARK(BM_SqrtRepeat_8);
*/

/*
static void BM_SqrtRepeat_4(benchmark::State& state) {
    auto x = random_double_r(1.0e6, 1.0e7, &seed);
    for (auto _ : state) {
        // REPEAT_8(x = sqrt(x));
        // escape(&x);
        for (int i = 0; i < 4; i++) {
            GLOBAL_DOUBLE = sqrt(x);
            clobber();
        }
    }
}

BENCHMARK(BM_SqrtRepeat_4);
*/


/*
static void BM_SqrtRepeat_4_rand(benchmark::State& state) {
    for (auto _ : state) {
        GLOBAL_DOUBLE = random_double_r(1.0e10, 1.0e20, &seed);
        // REPEAT_4(GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE));
        GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE);
        GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE);
        GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE);
        GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE);
        // escape(&GLOBAL_DOUBLE);
    }
}

BENCHMARK(BM_SqrtRepeat_4_rand);
*/


static void BM_SqrtFixedPointIter(benchmark::State& state) {
    // auto x = random_double_r(1.0e6, 1.0e7, &seed);
    auto x = random_double_r(1.0e100, 1.0e200, &seed);
    for (auto _ : state) {
        x = sqrt(x);
        // escape(&x);
    }
    GLOBAL_DOUBLE = x;	
}

BENCHMARK(BM_SqrtFixedPointIter)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_Rand_ab(benchmark::State& state) {
   for (auto _ : state) {
       GLOBAL_DOUBLE = random_double_r(1.0e10, 1.0e20, &seed);
       // clobber();
   }
}

BENCHMARK(BM_Rand_ab)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_Nop(benchmark::State& state) {
    for (auto _: state) {
        clobber();
    }
}

BENCHMARK(BM_Nop)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_SqrtFPI_small(benchmark::State& state) {
    for (auto _ : state) {
        GLOBAL_DOUBLE = random_double_r(0.0l, 1.0l, &seed);
        REPEAT_32(GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE));
    }
}

BENCHMARK(BM_SqrtFPI_small)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


static void BM_SqrtFPI_large(benchmark::State& state) {
    for (auto _ : state) {
        GLOBAL_DOUBLE = random_double_r(1.0e250, 1.0e300, &seed);
        REPEAT_32(GLOBAL_DOUBLE = sqrt(GLOBAL_DOUBLE));
    }
}

BENCHMARK(BM_SqrtFPI_large)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


BENCHMARK_MAIN();
