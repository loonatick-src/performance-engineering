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
    double y = 0.0l;
    for (auto _: state) {
        y = sqrt(x);
        clobber();
    }
    GLOBAL_DOUBLE = y;	
}

BENCHMARK(BM_Sqrt)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);

static void BM_Sin(benchmark::State& state) {
   auto x = random_double_r(1.0e8, 1.0e9, &seed);
   double y = 0.0l;
   for (auto _: state) {
        y = sin(x);
        clobber();
    }
    GLOBAL_DOUBLE = y;	
}

BENCHMARK(BM_Sin)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_Cos(benchmark::State& state) {
   auto x = random_double_r(1.0e8, 1.0e9, &seed);
   double y = 0.0l;
   for (auto _: state) {
        y = cos(x);
        clobber();
    }
    GLOBAL_DOUBLE = y;	
}

BENCHMARK(BM_Cos)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_Cbrt(benchmark::State& state) {
   auto x = random_double_r(1.0e8, 1.0e9, &seed);
   double y = 0.0l;
   for (auto _: state) {
        y = cbrt(x);
        clobber();
    }
    GLOBAL_DOUBLE = y;	
}

BENCHMARK(BM_Cbrt)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


static void BM_Mul(benchmark::State& state) {
   auto x = random_double_r(1.0, 1.0e9, &seed);
   auto y = random_double_r(1.0, 1.5, &seed);
   double z = 0.0l;
   for (auto _: state) {
        z = x * y;
        clobber();
   }
   GLOBAL_DOUBLE = z;	
}

BENCHMARK(BM_Mul)->ComputeStatistics("stddev", stddev)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);

BENCHMARK_MAIN();
