#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <benchmark/benchmark.h>

thread_local unsigned int seed;

vec3 GLOBAL_VEC3;

static void BM_RandSphereSpherical(benchmark::State& state) {
    for (auto _ : state) {
	GLOBAL_VEC3 = random_in_unit_sphere_v2();
    }
}

BENCHMARK(BM_RandSphereSpherical)->Repetitions(100)->DisplayAggregatesOnly(true);

BENCHMARK_MAIN();
