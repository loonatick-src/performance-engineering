#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <chrono>
#include <memory>

#define ITERATIONS 999999
#define ITERATIONS_INNER 999

using std::chrono::steady_clock;
using seconds = std::chrono::duration<double, std::ratio< 1 > >;
using std::chrono::time_point;
thread_local unsigned int seed;

int do_something_with_ptr(std::shared_ptr<int> data) {
  *data = *data + 1;
}

int use_shared_ptr() {
  auto ptr = std::make_shared<int>(1);
  # pragma omp parallel
  {
    # pragma omp for
    for (int j = 0; j < ITERATIONS; j++) {
      for (int i = 0; i < ITERATIONS_INNER; i++) {
        do_something_with_ptr(ptr);
      }
    }
  }
  return *ptr;
}

int main(int argc, char *argv[]) {
  omp_set_num_threads(atoi(argv[1]));

  // timing
  {
    auto start_time = steady_clock::now();
    int sum = use_shared_ptr();
    auto end_time = steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<seconds>(end_time - start_time).count();
    std::cerr << "rand took " << elapsed_seconds << " seconds (" << sum << ")\n";
  }
  
  return(0);
}