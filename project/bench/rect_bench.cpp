#include "aarect.h"
#include "material.h"
#include "init.h"
/**********************************************************************
* Because of the way `rand_r` has been incorporated into the project, *
* we're forced to use OpenMP. Consider removing calls to              *
* `omp_get_num_threads` and figuring out a more ergonomic API         *
***********************************************************************/
#include "omp.h"

#include <iostream>
#include <benchmark/benchmark.h>

unsigned int *seeds = NULL;

static void DoSetup(const benchmark::State& state) {
    init_seeds(&seeds, omp_get_num_threads());
}

static void DoTearDown(const benchmark::State& state) {
    free(seeds);
}

static void BM_XYRectHit(benchmark::State& state) {
    hit_record rec;
    const point3 orig(1, 1, 1);
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    xy_rect plane(0, 5, 0, 2, 5, white);
    // can we also save the number of points that hit and the number
    // that miss?
    for (auto _: state) {
        state.PauseTiming();
        // generate random vector
        auto direction = random_in_hemisphere(vec3(0, 0, 1));
        ray r(orig, direction);
        state.ResumeTiming();
        plane.hit(r, 0.0, infinity, rec);
    }
}

BENCHMARK(BM_XYRectHit)->Setup(DoSetup)->Teardown(DoTearDown);

BENCHMARK_MAIN();
