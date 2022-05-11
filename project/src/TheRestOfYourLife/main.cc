//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"
// commented out because the version of boost on DAS 5 has a `check` function/macro of its own
// #include "dbg.h"  // C99 header, should be fine
#include "aarect.h"
#include "box.h"
#include "input.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <iostream>
#include <chrono>
#include <omp.h>
#include <boost/multi_array.hpp>

using std::chrono::steady_clock;
using seconds = std::chrono::duration<double, std::ratio< 1 > >;
using std::chrono::time_point;

// haha yes shared mutable global
thread_local unsigned int seed;

color ray_color(
    const ray& r,
    const color& background,
    const hittable& world,
    shared_ptr<hittable> lights,
    int depth
) {
    hit_record rec;
    // debug("Reached depth %d in thread %d", depth, omp_get_thread_num());
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    // auto thread_id = omp_get_thread_num();
    if (!world.hit(r, 0.001, infinity, rec)) {
        return background;
    }
    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    if (srec.is_specular) {
        return srec.attenuation
             * ray_color(srec.specular_ray, background, world, lights, depth-1);
    }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
    mixture_pdf p(light_ptr, srec.pdf_ptr);
    ray scattered = ray(rec.p, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    return emitted
         + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                            * ray_color(scattered, background, world, lights, depth-1)
                            / pdf_val;
}


hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190,90,190), 90 , glass));

    return objects;
}


int main(int argc, char *argv[]) {
    // Image
    typedef boost::multi_array<double, 3> image_t;
    auto params = read_input(argc, argv);
    const int image_width = params.image_width;
    const int image_height = params.image_height;
    const int samples_per_pixel = params.samples_per_pixel;
    const int thread_count = params.thread_count;
    const int max_depth = params.max_depth;
    const double aspect_ratio = ((double)image_width) / ((double)image_height);

    image_t output_image(boost::extents[image_height][image_width][3]);

    // World

    auto lights = make_shared<hittable_list>();
    lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
    lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));

    auto world = cornell_box();

    color background(0,0,0);

    // Camera

    point3 lookfrom(278, 278, -800);
    point3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto time0 = 0.0;
    auto time1 = 1.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, time0, time1);

    // Render

    // I like streams. I hate streams.
    #ifndef NDEBUG
    std::cerr << "[DEBUG] " << __FILE__ << ':' << __LINE__ << " in function " << __func__;
    std::cerr << ": thread_count = " << thread_count << std::endl;
    std::cerr << ": image_width  = " << image_width << std::endl;
    std::cerr << ": samples_per_pixel = " << samples_per_pixel << std::endl;
    #endif
    omp_set_num_threads(thread_count);

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // timing
    auto start_time = steady_clock::now();

    # pragma omp parallel
    {
      seed = omp_get_thread_num();
      
      #ifndef NDEBUG
      std::cerr << ": seed for thread" << omp_get_thread_num() << " = " << seed << std::endl;
      #endif
      
      # pragma omp for collapse(2)
      for (int j = image_height-1; j >= 0; --j) {
          for (int i = 0; i < image_width; ++i) {
              // if (omp_get_thread_num() == 0)
              //     std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
              auto thread_id = omp_get_thread_num();
              color pixel_color(0,0,0);
              for (int s = 0; s < samples_per_pixel; ++s) {
                  auto u = (i + random_double_r(&seed)) / (image_width-1);
                  auto v = (j + random_double_r(&seed)) / (image_height-1);
                  ray r = cam.get_ray_r(u, v);
                  pixel_color += ray_color(r, background, world, lights, max_depth);
              }
              // write_color(std::cout, pixel_color, samples_per_pixel);

              output_image[j][i][0] = pixel_color.x();
              output_image[j][i][1] = pixel_color.y();
              output_image[j][i][2] = pixel_color.z();
              // # pragma omp critical
              // debug("Pixel [%d, %d] written by thread %u", i, j, omp_get_thread_num());
              // debug("Thread %d finished writing pixels", omp_get_thread_num());
          }
      }
    }

    auto end_time = steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<seconds>(end_time - start_time).count();
    std::cerr << "Writing image to buffer took " << elapsed_seconds << "seconds\n";

    for (int j = image_height-1; j >= 0; j--) {
        for (int i = 0; i < image_width; i++) {
            // check_debug(j < image_height && j >= 0 && i < image_width && i >= 0, "How tf is this out of bounds");
            write_color(std::cout, color(output_image[j][i][0], output_image[j][i][1], output_image[j][i][2]), samples_per_pixel);
            // debug("Wrote pixel [%d, %d] to file", i,j);
        }
    }
    std::cerr << "\nDone.\n";
    return 0;
    error:
    std::cerr << "Get fucked\n";
}
