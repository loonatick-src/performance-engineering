#ifndef PE_COMMON_H
#define PE_COMMON_H

#include <chrono>
#include <numeric>
#include <vector>
#include <cmath>


double mean(const std::vector<double> &v) {
    return std::accumulate(v.begin(), v.end(), 0.0l) / v.size();
}

double stddev(const std::vector<double> &v) {
    auto mv = mean(v);
    double s = 0.0l;
    for (const auto &x: v) {
        s += (x - mv) * (x - mv);
    }
    s /= v.size() - 1ul;

    return sqrt(s);
}


// old school loop unrolling -- CPP macros
// better software engineering practice:
// create a template class for unrolled loops,
// that specializes using an unsigned in - the unroll degree
// use that template class to dispatch the division ratio
#define REPEAT_64(expr)                                                        \
  REPEAT_32(expr)                                                             \
  REPEAT_32(expr)
#define REPEAT_32(expr)                                                        \
  REPEAT_16(expr)                                                             \
  REPEAT_16(expr)

#define REPEAT_16(expr)                                                        \
  REPEAT_8(expr)                                                              \
  REPEAT_8(expr)

#define REPEAT_8(expr)                                                         \
  REPEAT_4(expr)                                                              \
  REPEAT_4(expr)

#define REPEAT_4(expr)                                                         \
  REPEAT_2(expr)                                                              \
  REPEAT_2(expr)

#define REPEAT_2(expr)                                                         \
  expr;                                                                      \
  expr;


using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::time_point;

#define START_TIMER auto start_time = high_resolution_clock::now()
#define END_TIMER                                                              \
  auto end_time = high_resolution_clock::now();                                \
  const auto elapsed_seconds =                                                 \
      std::chrono::duration_cast<std::chrono::duration<double>>(end_time -     \
                                                                start_time)
static void escape(void *p) { asm volatile("" : : "g"(p) : "memory"); }
static void clobber() { asm volatile("" : : : "memory"); }

#endif
