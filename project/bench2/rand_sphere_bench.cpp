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


void print_format_data(double mean, double stddev, const std::string &name) {
    std::cerr << name << "    " << mean * 1.0e9l << " +- " << stddev * 1.0e9l << " ns\n";
    return;
}

void bench_log(const std::string& name) {
    std::cerr << "Running benchmark for " << name << std::endl;
    return;
}

using vd = std::vector<double>;

vd BM_RandSphereBaseline(size_t count) {
    vd t_iter;
    vec3 v;
    PE_LOOP {
        START_TIMER;
        v = random_in_unit_sphere();
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

vd BM_RandSphereSpherical(size_t count) {
    vd t_iter;
    vec3 v;
    PE_LOOP {
        START_TIMER;
        v = random_in_unit_sphere_v2();
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

vd BM_RandSphereCartesian(size_t count) {
    vd t_iter;
    vec3 v;
    PE_LOOP {
        START_TIMER;
        v = random_in_unit_sphere_v3();
        END_TIMER;
        t_iter.push_back(elapsed_seconds.count());
    }
    return t_iter;
}

#define DO_BENCH(BM_FUNC)                                                     \
  bench_log(name);                                                             \
  PE_BENCH((BM_FUNC), name);

int main(void) {
    vd timing_data;
    std::string name("baseline");
    DO_BENCH(BM_RandSphereBaseline);

    name = std::string("spherical");
    DO_BENCH(BM_RandSphereSpherical);

    name = std::string("cartesian");
    DO_BENCH(BM_RandSphereCartesian);

    return 0;
}
