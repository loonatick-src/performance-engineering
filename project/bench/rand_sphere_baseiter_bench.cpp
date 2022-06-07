#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <benchmark/benchmark.h>

thread_local unsigned int seed;

vec3 GLOBAL_VEC3;

static void BM_RandomSphereBaselineIter(benchmark::State& state) {
    vec3 v;
    for (auto _ : state) {
        auto v_temp = vec3::random(-1, 1);
        if (v_temp.length_squared() >= 1.0l) continue;
	v = v_temp;  // emulate return statement
    }
    escape(&v);
}

BENCHMARK(BM_RandomSphereBaselineIter)->Repetitions(100)->DisplayAggregatesOnly(true);

BENCHMARK_MAIN();
