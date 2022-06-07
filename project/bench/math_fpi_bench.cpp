#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <cmath>
#include <benchmark/benchmark.h>

#define STATS_CONFIG Repetitions(100)->DisplayAggregatesOnly(true)

thread_local unsigned int seed;

double GLOBAL_DOUBLE;

static void BM_Sqrt(benchmark::State &state) {
    double x = random_double_r(1.0e32, 1.0e34, &seed);
    for (auto _ : state) {
        x = sqrt(x);
    }
    escape(&x);
}
BENCHMARK(BM_Sqrt)->STATS_CONFIG;

static void BM_SinCos(benchmark::State &state) {
    double x = random_double_r(1.0e32, 1.0e34, &seed);
    for (auto _ : state) {
        x = sin(cos(x));
    }
    escape(&x);
}
BENCHMARK(BM_SinCos)->STATS_CONFIG;

static void BM_Sin(benchmark::State &state) {
    double x = random_double_r(1.0e32, 1.0e33, &seed);
    for (auto _ : state) {
        x = sin(x);
    }
    escape(&x);
}
BENCHMARK(BM_Sin)->STATS_CONFIG;

static void BM_Cos(benchmark::State &state) {
    double x = random_double_r(1.0e8, 1.0e9, &seed);
    for (auto _ : state) {
        x = cos(x);
    }
    escape(&x);
}
BENCHMARK(BM_Cos)->STATS_CONFIG;

static void BM_Cbrt(benchmark::State &state) {
    double x = random_double_r(1.0e8, 1.09, &seed);
    for (auto _ : state) {
        x = cbrt(x);
    }
    escape(&x);
}
BENCHMARK(BM_Cbrt)->STATS_CONFIG;

static void BM_L2normsq_rand_unit_sphere(benchmark::State &state) {
   double x;
   for (auto _ : state) {
	auto v = random_in_unit_sphere_v3();
        x = v.length_squared();	
   }
   escape(&x); 
}
BENCHMARK(BM_L2normsq_rand_unit_sphere)->STATS_CONFIG;

BENCHMARK_MAIN();
