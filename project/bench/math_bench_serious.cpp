#include "common_bench.h"
#include "rtweekend.h"
#include "vec3.h"

#include <benchmark/benchmark.h>

#define STATS_CONFIG 

double GLOBAL_DOUBLE;

static void BM_Sqrt(benchmark::State &state);

static void BM_SinCos(benchmark::State &state);

static void BM_Sin(benchmark::State &state);

static void BM_Cos(benchmark::State &state);

static void BM_Cbrt(benchmark::State &state);

static void BM_L2normsq(benchmar::State &state) {

}
