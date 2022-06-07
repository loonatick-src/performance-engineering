#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <benchmark/benchmark.h>

thread_local unsigned int seed;

#define STATS_CONFIG Repetitions(100)->DisplayAggregatesOnly()
vec3 GLOBAL_VEC3;
double GLOBAL_DOUBLE;

// Only ctor in loop -- affected by ILP most likely
/*
static void BM_Vec3(benchmark::State& state) {
    double x = random_double_r(&seed),
           y = random_double_r(&seed),
           z = random_double_r(&seed);
    for (auto _ : state) {
	GLOBAL_VEC3 = vec3(x, y, z);
	escape(&GLOBAL_VEC3);
    }
}
BENCHMARK(BM_Vec3)->STATS_CONFIG;

// call normsq on randomly generated vector
static void BM_L2Normsq_rand(benchmark::State& state) {
    double l2;
    for (auto _ : state) {
	auto v = vec3::random();
        GLOBAL_DOUBLE = v.length_squared();
    }
}
BENCHMARK(BM_L2Normsq_rand)->STATS_CONFIG;

// only random in loop
static void BM_Vec3Rand(benchmark::State& state) {
    for (auto _ : state) {
        GLOBAL_VEC3 = vec3::random();
    }
}
BENCHMARK(BM_Vec3Rand)->STATS_CONFIG;
*/

static void BM_Mul(benchmark::State& state) {
   double x = random_double_r(-2.0l, 2.0l, &seed);
   for (auto _ : state) {
	x *= x;
	escape(&x);
   }
}
BENCHMARK(BM_Mul)->STATS_CONFIG;

static void BM_Add(benchmark::State& state) {
   double x = random_double_r(-2.0l, 2.0l, &seed);
   for (auto _ : state) {
	x += x;
	escape(&x);
   }
}
BENCHMARK(BM_Add)->STATS_CONFIG;



/*
static void BM_Add_rand(benchmark::State& state) {
   double x, y = random_double_r(&seed);
   for (auto _ : state) {
       x = random_double_r(&seed);
       x += y;  // FP multiplication is not associative
       escape(&x);
   }
}
BENCHMARK(BM_Add_rand)->STATS_CONFIG;
*/





BENCHMARK_MAIN();
