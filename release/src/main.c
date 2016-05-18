/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This is the main file
 *
 */

#include "include/main.h"

/**
 *  @brief Main function
 *
 *  @todo WRITE IT
 *
 */
int main(int argc, char **argv) {
  fftw_init_threads();
  fftw_plan_with_nthreads(omp_get_max_threads());
  /* TODO */
  fftw_cleanup_threads();
  return 0;
}
