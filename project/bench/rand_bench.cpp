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

static void BM_Rand_ab(benchmark::State& state) {
    double x = -1.0l;
    for (auto _ : state) {
        x = random_double_r(x, 1.0l, &seed); // once again cockblock that ILP
    }
    escape(&x);
}

BENCHMARK(BM_Rand_ab)->Repetitions(PE_REPS)->DisplayAggregatesOnly(true);


BENCHMARK_MAIN();
