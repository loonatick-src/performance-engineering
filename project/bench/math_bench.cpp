#include "rtweekend.h"
#include "vec3.h"

#include "common_bench.h"

#include <iostream>
#include <benchmark/benchmark.h>
#include <omp.h>


thread_local unsigned int seed;

static void BM_SqrtManual(benchmark::State& state) {
    double sqrt_arg;
    auto seedp = &seed;
    double x;
    for (auto _: state) {
        x = random_double_r(1.0e7l, 1.0e8l, seedp);
        START_TIMER;
        REPEAT_8(x = sqrt(x));
        END_TIMER;
        escape(&x);
        state.SetIterationTime(elapsed_seconds.count()/8.0l);
    }
}

static void BM_SqrtRepeat_8(benchmark::State& state) {
    auto x = random_double_r(1.0e6, 1.0e7, &seed);
    for (auto _ : state) {
        REPEAT_8(x = sqrt(x));
    }
}

static void BM_SqrtRepeat_4_rand(benchmark::State& state) {
    double x;
    for (auto _ : state) {
        x = random_double_r(1.0e10, 1.0e20, &seed);
        REPEAT_4(x = sqrt(x));
        escape(&x);
    }
}


static void BM_rand_ab(benchmark::State& state) {
   double x;
   for (auto _ : state) {
       x = random_double_r(1.0e10, 1.0e20, &seed);
       clobber();
   }
}

static void BM_Sqrt(benchmark::State& state) {
    auto rnum = random_double_r(&seed);
    double rsqrt;
    for (auto _: state) {
        rsqrt = sqrt(rnum);
        escape(&rsqrt);
    }
}

static void BM_2Normsq(benchmark::State& state) {
    // TODO: use ranges to pass arguments instead of RNG
    for (auto _: state) {
        state.PauseTiming();
        auto seedp = &seed;
        auto x = random_double_r(seedp);
        auto y = random_double_r(seedp);
        auto z = random_double_r(seedp);
        state.ResumeTiming();
        auto v = vec3(x, y, z);
        escape(&v);
    }
}

static void BM_Trig(benchmark::State& state) {
    double x = random_double_r(1.0e6, 1.0e7, &seed);
    for (auto _: state) {
        // the fixed point iteration converges, which will affect the runtime
        x = sin(x);
        escape(&x);
    }
}


BENCHMARK(BM_Sqrt);
BENCHMARK(BM_SqrtManual)->UseManualTime();
BENCHMARK(BM_SqrtRepeat_8);
// BENCHMARK(BM_SqrtRepeated);
// BENCHMARK(BM_SqrtManualClobber)->UseManualTime();
// BENCHMARK(BM_SqrtManualEscape)->UseManualTime();
// BENCHMARK(BM_2Normsq);
BENCHMARK(BM_Trig);

BENCHMARK_MAIN();
