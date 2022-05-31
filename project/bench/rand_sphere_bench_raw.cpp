#include "rtweekend.h"
#include "vec3.h"
#include "common_bench.h"

#include <cstdio>

thread_local unsigned int seed;

const size_t MAX_ITER = 1e9;

vec3 BM_RandSphereBaseline(size_t count) {
    vec3 rv;
    START_TIMER;
    for (size_t i = 0; i < count; i++) {
        rv = random_in_unit_sphere_v3();
        // rv = random_in_unit_sphere_v2();
        // rv = random_in_unit_sphere();
        clobber();
    } 
    END_TIMER;

    printf("%lf\n", elapsed_seconds.count()/count);
    return rv;
}

int main() {
    auto rv = BM_RandSphereBaseline(MAX_ITER);
    escape(&rv);

    return 0;
}
