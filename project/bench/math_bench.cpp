#include "rtweekend.h"
#include "vec3.h"
#include "omp.h"

#include <iostream>
#include <benchmark/benchmark.h>
#include <chrono>


// old school loop unrolling -- CPP macros
// better software engineering practice:
// create a template class for unrolled loops,
// that specializes using an unsigned in - the unroll degree
// use that template class to dispatch the division ratio
#define REPEAT_64(expr) REPEAT_32(expr); REPEAT_32(expr);
#define REPEAT_32(expr) REPEAT_16(expr); REPEAT_16(expr);
#define REPEAT_16(expr) REPEAT_8(expr); REPEAT_8(expr);
#define REPEAT_8(expr) REPEAT_4(expr); REPEAT_4(expr);
#define REPEAT_4(expr) REPEAT_2(expr); REPEAT_2(expr);
#define REPEAT_2(expr) (expr); (expr);

// using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::time_point;

thread_local unsigned int seed;

static void escape(void *p) { asm volatile("" : : "g"(p) : "memory"); }
static void clobber() { asm volatile("" : : : "memory"); }

// inline void repeat_sqrt(unsigned int count, double arg) {
//     for (unsized int i = 0 ; i < count; i++) {
//         clobber();
//         sqrt(arg);
//         clobber();
//     }
// }

static void BM_SqrtRepeated(benchmark::State& state) {
    auto seedp = &seed;
    auto arg = random_double_r(seedp);
    for (auto _ : state) {
        REPEAT_64(sqrt(arg));
    }
}

static void BM_SqrtManual(benchmark::State& state) {
    double sqrt_arg;
    auto seedp = &seed;
    for (auto _: state) {
        const auto arg = random_double_r(seedp);
        const auto start_time = high_resolution_clock::now();
        REPEAT_8(sqrt_arg = sqrt(arg));
        const auto end_time = high_resolution_clock::now();
        const auto elapsed_seconds =
            std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

// static void BM_SqrtManualClobber(benchmark::State& state) {
//     double sqrt_arg;
//     auto seedp = &seed;
//     for (auto _: state) {
//         const auto arg = random_double_r(seedp);
//         const auto start_time = high_resolution_clock::now();
//         clobber();
//         REPEAT_64(sqrt_arg = sqrt(arg));
//         const auto end_time = high_resolution_clock::now();
//         const auto elapsed_nanoseconds = std::chrono::duration_cast<nanoseconds>(end_time - start_time).count() / 64.0l;
//         state.SetIterationTime(elapsed_nanoseconds);
//     }
// }

// static void BM_SqrtManualEscape(benchmark::State& state) {
//     double sqrt_arg;
//     auto seedp = &seed;
//     for (auto _: state) {
//         const auto arg = random_double_r(seedp);
//         const auto start_time = high_resolution_clock::now();
//         REPEAT_64(sqrt_arg = sqrt(arg));
//         escape(&sqrt_arg);
//         const auto end_time = high_resolution_clock::now();
//         const auto elapsed_nanoseconds = std::chrono::duration_cast<nanoseconds>(end_time - start_time).count() / 64.0l;
//         state.SetIterationTime(elapsed_nanoseconds);
//     }
// }

static void BM_Sqrt(benchmark::State& state) {
    auto rnum = random_double_r(&seed);
    double rsqrt;
    for (auto _: state) {
        rsqrt = sqrt(rnum);
        escape(&rsqrt);
    }
}

static void BM_2Normsq(benchmark::State& state) {
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
    for (auto _: state) {
        state.PauseTiming();
        auto seedp = &seed;
        auto theta = random_double_r(0.0l, M_PI, seedp);
        state.ResumeTiming();
        auto x = cos(theta);
        escape(&x);
    }
}

BENCHMARK(BM_Sqrt);
BENCHMARK(BM_SqrtManual)->UseManualTime();
BENCHMARK(BM_SqrtRepeated);
// BENCHMARK(BM_SqrtManualClobber)->UseManualTime();
// BENCHMARK(BM_SqrtManualEscape)->UseManualTime();
// BENCHMARK(BM_2Normsq);
// BENCHMARK(BM_Trig);

BENCHMARK_MAIN();
