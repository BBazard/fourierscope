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
 *  @brief Set all the matrix cells outside of a disk to 0
 *  @param[in] in The fftw_complex 2d matrix used as input
 *  @param[out] out The fftw_complex 2d matrix used as output
 *  @param[in] dim The dimension of the matrix (assumed square)
 *  @param[in] radius The radius of the disk
 *  @return 1 If the radius of the circle cannot fit in the matrix
 *  @return 0 Otherwise
 *
 *  This function computes a disk in the input matrix using taxicab geometry.
 *  The circle is centered on the middle of the matrix (not exact if the matrix
 *  dimension is an even number).
 *
 */
int cut_disk(fftw_complex* in, fftw_complex* out, int dim, int radius) {
  int mid = dim/2 + dim % 2;

  if (radius > mid) {
    return 1;
  } else {
    int *ref;
    ref = (int*) malloc(dim * dim * sizeof(int));

    for (int i = 0; i < dim * dim; i++)
      ref[i] = -1;

    ref[mid*dim+mid] = radius;
    von_neumann(mid, mid, radius-1, ref, dim, in, out);
    free(ref);
    return 0;
  }
}

/**
 *  @brief Compute the von_neumann
 *  @param[in] x The line on which the function is applied
 *  @param[in] y The column on which the function is applied
 *  @param[in] radius The radius of the disk, see below
 *  @param[in,out] mat The reference matrix used to remember the previous states
 *  @param[in] dim The dimension of the matrix
 *  @param[in] in The matrix in which to cut a disk
 *  @param[out] out The matrix in which is stored the result
 *
 *  This function is a recursive function spanning from the center of disk
 *  (initial call with right x and y values) to its border by decreasing
 *  radius by one each time it is called.
 *  In fact it copies the part which is in the disk to the output matrix.
 *
 */
void von_neumann(int x, int y, int radius, int *mat, int dim,
                 fftw_complex *in, fftw_complex *out) {
  if (radius >= 0) {
    (out[x*dim+y])[0] = (in[x*dim+y])[0];
    (out[x*dim+y])[1] = (in[x*dim+y])[1];

    if (mat[(x+1)*dim+y] < radius) {
      mat[(x+1)*dim+y] = radius;
      von_neumann(x+1, y, radius-1, mat, dim, in, out);
    }
    if (mat[(x-1)*dim+y] < radius) {
      mat[(x-1)*dim+y] = radius;
      von_neumann(x-1, y, radius-1, mat, dim, in, out);
    }
    if (mat[x*dim+y+1] < radius) {
      mat[x*dim+y+1] = radius;
      von_neumann(x, y+1, radius-1, mat, dim, in, out);
    }
    if (mat[x*dim+y-1] < radius) {
      mat[x*dim+y-1] = radius;
      von_neumann(x, y-1, radius-1, mat, dim, in, out);
    }
  }
}

/**
 *  @brief The identity function used in matrix_operation
 *
 *  Here, args should be equal to the NULL pointer
 *
 */
double identity(double d, void **args) {return d;}

/**
 *  @brief A function to divide by dim used in matrix_operation
 *
 *  args[0] MUST contains the pointer to the dimension of the matrix
 *
 */
double div_dim(double d, void **args) {
  int dim = *((int*) args[0]);
  return d/(double) (dim*dim);
}

/**
 *  @brief Gerchberg-Saxton algorithm
 *  @param[in] dim The dimension of the matrix
 *  @param[in] input The matrix on which to apply the algorithm
 *  @param[out] output The matrix in which to store the result
 *  @param[in] exec_limit The number of iteration to execute
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
  fftw_complex *tf;
  fftw_complex *disk;
  fftw_complex *itf;
  fftw_complex *modarg;

  void **args = (void**) malloc(2*sizeof(void*));
  args[0] = &dim;
  args[1] = NULL;

  in = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  tf = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  disk = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  itf = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
  modarg = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));

  fftw_plan forward = fftw_plan_dft_2d(dim, dim, in, tf,
                                       FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_plan backward = fftw_plan_dft_2d(dim, dim, disk, itf,
                                        FFTW_BACKWARD, FFTW_ESTIMATE);

  matrix_operation(input, in, dim, identity, NULL);
  matrix_init(dim, tf, 0);
  matrix_init(dim, disk, 0);
  matrix_init(dim, itf, 0);

  for (int it=0; it < exec_limit; it++) {
    fftw_execute(forward);

    cut_disk(tf, disk, dim, radius);

    fftw_execute(backward);
    matrix_operation(itf, in, dim, div_dim, args);

    for (int i=0; i < dim*dim; i++) {
      get_modarg(in[i], modarg[i]);
      (modarg[i])[0] = (input[i])[0];
      get_algebraic(modarg[i], in[i]);
    }
  }
  matrix_operation(in, output, dim, identity, NULL);

  fftw_destroy_plan(forward);
  fftw_destroy_plan(backward);

  fftw_free(in);
  fftw_free(tf);
  fftw_free(disk);
  fftw_free(itf);
  fftw_free(modarg);
  free(args);
}
