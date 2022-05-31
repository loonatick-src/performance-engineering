#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <benchmark/benchmark.h>

thread_local unsigned int seed;

vec3 GLOBAL_VEC3;

static void BM_RandSphereBaseline(benchmark::State& state) {
    for (auto _ : state) {
        GLOBAL_VEC3 = random_in_unit_sphere();
    }
}

BENCHMARK(BM_RandSphereBaseline)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);

BENCHMARK_MAIN();
