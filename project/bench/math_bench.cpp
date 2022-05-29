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
#define REPEAT_64(expr)                                                        \
  REPEAT_32(expr);                                                             \
  REPEAT_32(expr);
#define REPEAT_32(expr)                                                        \
  REPEAT_16(expr);                                                             \
  REPEAT_16(expr);

#define REPEAT_16(expr)                                                        \
  REPEAT_8(expr);                                                              \
  REPEAT_8(expr);

#define REPEAT_8(expr)                                                         \
  REPEAT_4(expr);                                                              \
  REPEAT_4(expr);

#define REPEAT_4(expr)                                                         \
  REPEAT_2(expr);                                                              \
  REPEAT_2(expr);

#define REPEAT_2(expr)                                                         \
  (expr);                                                                      \
  (expr);


#define START_TIMER auto start_time = high_resolution_clock::now()
#define END_TIMER                                                              \
  auto end_time = high_resolution_clock::now();                                \
  const auto elapsed_seconds =                                                 \
      std::chrono::duration_cast<std::chrono::duration<double>>(end_time -     \
                                                                start_time)


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

// most likely misleading due to ILP -- will report reciprocal throughput essentially
// static void BM_SqrtRepeated(benchmark::State& state) {
//     auto seedp = &seed;
//     auto arg = random_double_r(seedp);
//     for (auto _ : state) {
//         REPEAT_8(x = sqrt(arg));
//         esacpe(&x);
//     }
// }

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

// most likely misleading because LSD and ILP
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
    x = random_double_r(1.0e6, 1.0e7, &seed);
    for (auto _: state) {
        // I hope the fixed point iteration does not convege
        x = sin(x);
        escape(&x);
    }
}

BENCHMARK(BM_Sqrt);
BENCHMARK(BM_SqrtManual)->UseManualTime();
BENCHMARK(BM_SqrtRepeated);
// BENCHMARK(BM_SqrtManualClobber)->UseManualTime();
// BENCHMARK(BM_SqrtManualEscape)->UseManualTime();
// BENCHMARK(BM_2Normsq);
BENCHMARK(BM_Trig);

BENCHMARK_MAIN();
