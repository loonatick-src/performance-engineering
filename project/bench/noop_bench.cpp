#include "common_bench.h"

#include <benchmark/benchmark.h>

#define STATS_CONFIG Repetitions(100)->DisplayAggregatesOnly(true)

static void BM_Noop(benchmark::State &state) {
    for (auto _ : state) {clobber();}
}

BENCHMARK(BM_Noop)->STATS_CONFIG;

BENCHMARK_MAIN();
