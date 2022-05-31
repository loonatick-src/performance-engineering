#include "rtweekend.h"
#include "vec3.h"

#include "common_bench.h"

#include <iostream>
#include <benchmark/benchmark.h>

thread_local unsigned int seed;

double GLOBAL_DOUBLE;
int GLOBAL_INT;

static void BM_RandomSphereBaseline(benchmark::State& state) {
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere();
        escape(&rand_vec);
    }
}

BENCHMARK(BM_RandomSphereBaseline)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


static void BM_RandomSpherePolar(benchmark::State& state) {
    for (auto _: state) {
        // TODO: prevent compiler from optimising this out
        auto rand_vec = random_in_unit_sphere_v2();
        escape(&rand_vec);
    }
}

BENCHMARK(BM_RandomSpherePolar)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


static void BM_RandomSphereCartesian(benchmark::State& state) {
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere_v3();
        escape(&rand_vec);
    }
}

BENCHMARK(BM_RandomSphereCartesian)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


static void BM_RandomSphereBaselineIter(benchmark::State& state) {
    int hit_count = 0;
    for (auto _ : state) {
        auto v = vec3::random(-1, 1);
        if (v.length_squared() < 1.0l) hit_count++;
        clobber();
    }
}

BENCHMARK(BM_RandomSphereBaselineIter)->ComputeStatistics("stddev", stddev)->Repetitions(10)->DisplayAggregatesOnly(true);


BENCHMARK_MAIN();
