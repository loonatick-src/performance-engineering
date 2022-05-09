#include "aarect.h"
#include "material.h"
#include "init.h"

#include <omp.h>
#include <iostream>
#include <chrono>

unsigned int *seeds = NULL;

using std::chrono::steady_clock;
using seconds = std::chrono::duration<double, std::ratio< 1 > >;
using std::chrono::time_point;

int BM_XYRectHit(int count) {
    hit_record rec;
    const point3 orig(1,1,1);
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    xy_rect plane(0,5,0,2,5,white);
    // double elapsed = 0.0l;
    int hit_count = 0;

    for (int i = 0; i < count; i++) {
        // generate random ray
        auto direction = random_in_hemisphere(vec3(0, 0, 1));
        ray r(orig, direction);

        // auto start_time = steady_clock::now();
        auto hit_p = plane.hit(r, 0.0, infinity, rec);
        // auto end_time = steady_clock::now(); 
        // if (hit_p) {
            hit_count++;
        // }
        // elapsed += std::chrono::duration_cast<seconds>(end_time - start_time).count();
    }

    //std::cerr << elapsed / count << " seconds" << std::endl;

    return hit_count;
}

int main() {
    init_seeds(&seeds, omp_get_num_threads());

    const int count = 2e8;
    auto hit_count = BM_XYRectHit(count);
    std::cerr << static_cast<double>(hit_count) / static_cast<double>(count) * 100.0l
        << " % rays hit the plane."
        << std::endl;

    return 0;
}
