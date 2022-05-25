#include "rtweekend.h"
#include "vec3.h"
#include "omp.h"

#include <iostream>
#include <benchmark/benchmark.h>

thread_local unsigned int seed;

// thank you Noogle Benchmark and Chandler Caruth, very cool
static void escape(void *p) {
  asm volatile("" : : "g"(p) : "memory");
}

static void clobber() {
  asm volatile("" : : : "memory");
}

static void BM_RandomSphereBaseline(benchmark::State& state) {
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere();
        escape(&rand_vec);
    }
}

static void BM_RandomSpherePolar(benchmark::State& state) {
    for (auto _: state) {
        // TODO: prevent compiler from optimising this out
        auto rand_vec = random_in_unit_sphere_v2();
        escape(&rand_vec);
    }
}

static void BM_RandomSphereCartesian(benchmark::State& state) {
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere_v3();
        escape(&rand_vec);
    }
}

static void BM_RandomSphereModdedbaseline(benchmark::State& state) {
    for (auto _: state) {
        auto rand_vec = random_in_unit_sphere_v4();
        escape(&rand_vec);
    }
}

BENCHMARK(BM_RandomSphereBaseline);
BENCHMARK(BM_RandomSpherePolar);
BENCHMARK(BM_RandomSphereCartesian);
BENCHMARK(BM_RandomSphereModdedbaseline);

BENCHMARK_MAIN();
