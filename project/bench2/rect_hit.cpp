#include "aarect.h"
#include "material.h"
#include "init.h"

#include <omp.h>
#include <iostream>
#include <chrono>

thread_local unsigned int seed;

using std::chrono::steady_clock;
using seconds = std::chrono::duration<double, std::ratio< 1 > >;
using std::chrono::time_point;

int BM_XYRectHit(int count) {
    hit_record rec;
    const point3 orig(0.5l,0.5l,0.5l);
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    xy_rect plane(0,1,0,1,5,white);
    // double elapsed = 0.0l;
    int hit_count = 0;

    double x = 1, y = 0, z = 1;
    double dx = -2.0l / (static_cast<double>(count));
    ray r(orig, vec3(x, y, z)); 
    auto dr = vec3(dx, 0, 0);

    for (int i = 0; i < count; i++) {
        // auto start_time = steady_clock::now();
        auto hit_p = plane.hit(r, 0.0, infinity, rec);
        if (hit_p) {
            hit_count++;
        }
        r.dir += dr;
    }

    // std::cerr << elapsed / count << " seconds" << std::endl;

    return hit_count;
}

int main() {
    const int count = 2e9;
    auto hit_count = BM_XYRectHit(count);
    std::cerr << static_cast<double>(hit_count) / static_cast<double>(count) * 100.0l
        << " % rays hit the plane."
        << std::endl;

    return 0;
}
