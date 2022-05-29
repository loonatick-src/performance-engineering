#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <chrono>

#define ITERATIONS 999999
#define ITERATIONS_INNER 999

using std::chrono::steady_clock;
using seconds = std::chrono::duration<double, std::ratio< 1 > >;
using std::chrono::time_point;
thread_local unsigned int seed;

int use_rand_r() {
  int sum = 0; // this prevents the optimizer from removing the code
  # pragma omp parallel
  {
    seed = omp_get_thread_num();
    
    # pragma omp for reduction (+:sum)
    for (int j = 0; j < ITERATIONS; j++) {
      for (int i = 0; i < ITERATIONS_INNER; i++) {
        sum += rand_r(&seed);
      }
    }
  }
  return sum;
}

int use_rand() {
  int sum = 0; // this prevents the optimizer from removing the code
  # pragma omp parallel
  {
    # pragma omp for reduction (+:sum)
    for (int j = 0; j < ITERATIONS; j++) {
      for (int i = 0; i < ITERATIONS_INNER; i++) {
        sum += rand();
      }
    }
  }
  return sum;
}

int main(int argc, char *argv[]) {
  omp_set_num_threads(atoi(argv[1]));

  // timing
  {
    auto start_time = steady_clock::now();
    int sum = use_rand();
    auto end_time = steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<seconds>(end_time - start_time).count();
    std::cerr << "rand took " << elapsed_seconds << " seconds (" << sum << ")\n";
  }
  
  {
    auto start_time = steady_clock::now();
    int sum = use_rand_r();
    auto end_time = steady_clock::now();
    double elapsed_seconds = std::chrono::duration_cast<seconds>(end_time - start_time).count();
    std::cerr << "rand_r took " << elapsed_seconds << " seconds (" << sum << ")\n";
  }
  return(0);
}