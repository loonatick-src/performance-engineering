#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <cmath>
#include <benchmark/benchmark.h>

#define STATS_CONFIG Repetitions(100)->DisplayAggregatesOnly()

thread_local unsigned int seed;

static void BM_SqrtNaive(benchmark::State& state) {
    double x = random_double_r(&seed), y = 0.0l;
    for (auto _ : state) {
        y = sqrt(x);
        escape(&y);
    } 
}

BENCHMARK(BM_SqrtNaive)->STATS_CONFIG;

BENCHMARK_MAIN();
       
