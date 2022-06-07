#include "rtweekend.h"
#include "vec3.h"

#include "common_bench.h"

#include <iostream>
#include <benchmark/benchmark.h>

thread_local unsigned int seed;

double GLOBAL_DOUBLE;
int GLOBAL_INT;

static void BM_RandomSphereBaseline(benchmark::State& state) {
    auto rand_vec = random_in_unit_sphere();
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere();
        escape(&rand_vec);
    }
    GLOBAL_DOUBLE = rand_vec.y();
}

BENCHMARK(BM_RandomSphereBaseline)->Repetitions(100)->DisplayAggregatesOnly(true);


static void BM_RandomSpherePolar(benchmark::State& state) {
    auto rand_vec = random_in_unit_sphere_v2();
    for (auto _: state) {
        rand_vec = random_in_unit_sphere_v2();
        escape(&rand_vec);
    }
    GLOBAL_DOUBLE = rand_vec.x();
}

BENCHMARK(BM_RandomSpherePolar)->Repetitions(100)->DisplayAggregatesOnly(true);


static void BM_RandomSphereCartesian(benchmark::State& state) {
    auto rand_vec = random_in_unit_sphere_v3();
    for (auto _: state) {
        rand_vec = random_in_unit_sphere_v3();
        escape(&rand_vec);
    }
    GLOBAL_DOUBLE = rand_vec.z();
}

BENCHMARK(BM_RandomSphereCartesian)->Repetitions(100)->DisplayAggregatesOnly(true);


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
