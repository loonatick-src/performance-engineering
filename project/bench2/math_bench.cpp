#include "rtweekend.h"
#include "vec3.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>
#include <string>



constexpr size_t PE_COUNT = 5e8;

#define PE_LOOP for (size_t i = 0; i < count; i++)

#define PE_BENCH(PE_BM_Func, BM_NAME)                                          \
  timing_data = PE_BM_Func(PE_COUNT);                                          \
  print_format_data(sample_mean(timing_data), sample_stddev(timing_data), name);


double sample_mean(const std::vector<double> &xs) {
    double mv = 0.0l;
    for (const auto x: xs) {
        mv += x;
    }
    return mv / xs.size();
}

double sample_svar(const std::vector<double>& xs) {
    const auto mean_value = sample_mean(xs);
    double s = 0.0l;
    for (const auto x: xs) {
        s += (x - mean_value)*(x - mean_value); // please do common sub-expr elim
    }
    const auto n = xs.size();
    return s / (n-1);
}

double sample_stddev(const std::vector<double>& xs) {
    return sqrt(sample_svar(xs));
}

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



// using std::chrono::steady_clock;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::time_point;

// not so hygeinic macros
#define START_TIMER auto start_time = high_resolution_clock::now()
#define END_TIMER                                                              \
  auto end_time = high_resolution_clock::now();                                \
  const auto elapsed_seconds =                                                 \
      std::chrono::duration_cast<std::chrono::duration<double>>(end_time -     \
                                                                start_time)

    
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

std::vector<double> BM_Sqrt(size_t count) {
    auto seedp = &seed;
    std::vector<double> t_iter;
    double x;
    for (size_t i = 0; i < count; i++) {
        x = random_double_r(1.0e3, 1.0e4, seedp);
        START_TIMER;
        // Bypass ILP
        REPEAT_4(x = sqrt(x));
        END_TIMER;
        clobber();
        t_iter.push_back(elapsed_seconds.count()/4.0l);
    }
    return t_iter;
}

std::vector<double> BM_Sqrt_dup(size_t count) {
    auto seedp = &seed;
    std::vector<double> t_iter;
    double x;
    for (size_t i = 0; i < count; i++) {
        x = random_double_r(1.0e3, 1.0e4, seedp);
        START_TIMER;
        // Bypass ILP
        REPEAT_4(x = sqrt(x));
        END_TIMER;
        clobber();
        t_iter.push_back(elapsed_seconds.count()/4.0l);
    }
    return t_iter;
}

std::vector<double> BM_Sqrt_rand(size_t count) {
    auto seedp = &seed;
    std::vector<double> t_iter;
    double x, arg;
    PE_LOOP {
        arg = random_double_r(10.0l, 100.0l, seedp);
        START_TIMER;
        x = sqrt(arg);
        END_TIMER;
        clobber();
        t_iter.push_back(elapsed_seconds.count());
    }

    return t_iter;
}

// Compiler is not optimising out the sqrt for some reason, so these are not really required
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

std::vector<double> BM_2Normsq(size_t count) {
    std::vector<double> t_iter;
    for (size_t i = 0; i < count; i++) {
        auto seedp = &seed;
        // TODO: is RNG required? How do I justify taking the same argument per iteration?
        auto x = random_double_r(seedp);
        auto y = random_double_r(seedp);
        auto z = random_double_r(seedp);
        auto v = vec3(x, y, z);
        START_TIMER;
        auto lsq = v.length_squared();
        END_TIMER;
        escape(&v);
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

std::vector<double> BM_Sqrt_rand_dup(size_t count) {
    auto seedp = &seed;
    std::vector<double> t_iter;
    PE_LOOP {
        const auto arg = random_double_r(10.0l, 100.0l, seedp);
        START_TIMER;
        auto x = sqrt(arg);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }

    return t_iter;
}

std::vector<double> BM_2Normsq_dup(size_t count) {
    std::vector<double> t_iter;
    for (size_t i = 0; i < count; i++) {
        auto seedp = &seed;
        // TODO: is RNG required? How do I justify taking the same argument per iteration?
        auto x = random_double_r(seedp);
        auto y = random_double_r(seedp);
        auto z = random_double_r(seedp);
        auto v = vec3(x, y, z);
        START_TIMER;
        auto lsq = v.length_squared();
        END_TIMER;
        escape(&v);
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}


std::vector<double> BM_Trig(size_t count) {
    std::vector<double> t_iter;
    for (size_t i = 0; i < count; i++) {
        auto seedp = &seed;
        auto theta = random_double_r(0.0l, M_PI, seedp);
        START_TIMER;
        auto x = cos(theta);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

std::vector<double> BM_Trig_dup(size_t count) {
    std::vector<double> t_iter;
    for (size_t i = 0; i < count; i++) {
        auto seedp = &seed;
        auto theta = random_double_r(0.0l, M_PI, seedp);
        START_TIMER;
        auto x = cos(theta);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

std::vector<double> BM_add(size_t count) {
    std::vector<double> t_iter;
    auto seedp = &seed;
    double x = random_double_r(seedp);
    double y = random_double_r(seedp);
    PE_LOOP {
        START_TIMER;
        REPEAT_8(x = x + y);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count() / 8.0l);
    }
    return t_iter;
}

std::vector<double> BM_mul(size_t count) {
    std::vector<double> t_iter;
    auto seedp = &seed;
    double x = random_double_r(1.0l, 10.0l, seedp);
    double y = random_double_r(1.0l, 10.0l, seedp);
    PE_LOOP {
        START_TIMER;
        REPEAT_8(x = x * y);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count() / 8.0l);
    }
    return t_iter;
}

std::vector<double> BM_mul_insitu_rng(size_t count) {
    std::vector<double> t_iter;
    auto seedp = &seed;
    PE_LOOP {
        double x = random_double_r(1.0l, 10.0l, seedp);
        double y = random_double_r(1.0l, 10.0l, seedp);
        START_TIMER;
        REPEAT_8(x = x + y);
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count() / 8.0l);
    }
    return t_iter;
}

double BM_Loop(size_t count) {
    START_TIMER;
    for (size_t i = 0; i < count; i++) {
        clobber();
    }
    END_TIMER;
    return elapsed_seconds.count();
}

void print_format_data(double mean, double stddev, const std::string &name) {
    std::cerr << name << "    " << mean * 1.0e9l << " +- " << stddev * 1.0e9l << " ns\n";
    return;
}

void bench_log(const std::string& name) {
    std::cerr << "Running benchmark for " << name << std::endl;
    return;
}
    
int main(void) {
    std::vector<double> timing_data;
    std::string name("sqrt");
    bench_log(name);
    PE_BENCH(BM_Sqrt, name);
    bench_log(name);
    PE_BENCH(BM_Sqrt_dup, name);

    name = std::string("2-norm-sq");
    bench_log(name);
    PE_BENCH(BM_2Normsq, name);
    bench_log(name);
    PE_BENCH(BM_2Normsq_dup, name);

    name = std::string("trig");
    bench_log(name);
    PE_BENCH(BM_Trig, name);
    bench_log(name);
    PE_BENCH(BM_Trig_dup, name);

    name = std::string("sqrt-rand");
    bench_log(name);
    PE_BENCH(BM_Sqrt_rand, name);
    bench_log(name);
    PE_BENCH(BM_Sqrt_rand_dup, name);

    name = std::string("add");
    bench_log(name);
    PE_BENCH(BM_add, name);
    bench_log(name);
    PE_BENCH(BM_add, name);

    name = std::string("mul");
    bench_log(name);
    PE_BENCH(BM_mul, name);
    bench_log(name);
    PE_BENCH(BM_mul, name);

    name = std::string("mul_inloop_rng");
    bench_log(name);
    PE_BENCH(BM_mul_insitu_rng, name);
    bench_log(name);
    PE_BENCH(BM_mul_insitu_rng, name);
    
    
    std::cerr << "Raw loop timer" << BM_Loop(PE_COUNT) * 1.0e9 / PE_COUNT << std::endl;
    return 0;
}
