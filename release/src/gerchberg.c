/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file implements the Gerchberg-Saxton algorithm.
 *  It allows to retrieve the phase of the light when a picture
 *  was taken from the intensity of it.
 *
 */

#include "include/gerchberg.h"

/**
 *  @brief Gerchberg-Saxton algorithm
 *  @param[in] dim The dimension of the matrix
 *  @param[in] input The matrix on which to apply the algorithm
 *  @param[out] output The matrix in which to store the result
 *  @param[in] exec_limit The number of iterations to execute
 *  @param[in] radius The radius of the circle used to limit the spectrum
 *
 *  This funtion implements the Gerchberg-Saxton algorithm, and will store the
 *  result of this algorithm in the output parameter given a number
 *  of iteration and a radius.
 *
 *  Note that after a given number of iteration the results will stop
 *  getting better.
 *
 */
void gerchberg(int dim, fftw_complex *input, fftw_complex *output,
               int exec_limit, int radius) {
  fftw_complex *in;
  fftw_complex *freq;
  fftw_complex *disk;
  fftw_complex *time;
  fftw_complex *modarg;

  void **args = (void**) malloc(2*sizeof(void*));
  args[0] = &dim;
  args[1] = NULL;

  in = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  freq = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  disk = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  time = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  modarg = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));

  fftw_plan forward = fftw_plan_dft_2d(dim, dim, in, freq,
                                       FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_plan backward = fftw_plan_dft_2d(dim, dim, disk, time,
                                        FFTW_BACKWARD, FFTW_ESTIMATE);

  matrix_operation(input, in, dim, identity, NULL);
  matrix_init(dim, freq, 0);
  matrix_init(dim, disk, 0);
  matrix_init(dim, time, 0);

  for (int it=0; it < exec_limit; it++) {
    fftw_execute(forward);

    copy_disk(freq, disk, dim, radius);

    fftw_execute(backward);
    matrix_operation(time, in, dim, div_dim, args);

    for (int i=0; i < dim*dim; i++) {
      alg2exp(in[i], modarg[i]);
      (modarg[i])[0] = (input[i])[0];
      exp2alg(modarg[i], in[i]);
    }
  }
  matrix_operation(in, output, dim, identity, NULL);

  fftw_destroy_plan(forward);
  fftw_destroy_plan(backward);

  fftw_free(in);
  fftw_free(freq);
  fftw_free(disk);
  fftw_free(time);
  fftw_free(modarg);
  free(args);
}
