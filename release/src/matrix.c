/**
 *  @file
 *
 *  This file implements several basic functions to
 *  manipulate matrix and complexes
 *
 */

#include "include/matrix.h"

/**
 *  @brief Initializes a fftw_complex matrix
 *  @param[in] dim The matrix dimension
 *  @param[in,out] mat The matrix to init
 *  @param[in] value The value with which to initialize the matrix
 *
 *  This function init a matrix with 0 in complexity O(n^{2})
 *
 */
void matrix_init(int dim, fftw_complex *mat, double value) {
  for (int i=0; i < dim*dim; i++) {
    (mat[i])[0] = value;
    (mat[i])[1] = value;
  }
}

/**
 *  @brief Initializes a matrix with random complexes
 *  @param[in] dim The dimension of the matrix
 *  @param[out] mat The fftw_complex matrix to initialize
 *  @param[in] max_rand The maximum value for random numbers
 *
 *  This function initializes a matrix of fftw_complex with
 *  randomly generated numbers (integers)
 *
 *
 */
void matrix_random(int dim, fftw_complex *mat, int max_rand) {
  srand(time(NULL));

  for (int i=0; i < 10; i++)
    for (int j=0; j < 10; j++) {
      (mat[i*10+j])[0] = rand() % max_rand;
      (mat[i*10+j])[1] = rand() % max_rand;
      (mat[i*10+j])[1] /= 10000000000;
    }
}

/**
 *  @brief Prints a matrix
 *  @param[in] dim The dimension of the matrix
 *  @param[in] mat The matrix to print
 *
 *  This function prints a matrix of fftw_complex in a nice
 *  formatted output.
 *
 */
void matrix_print(int dim, fftw_complex *mat) {
  for (int i=0; i < dim; i++) {
    for (int j=0; j < dim; j++)
      printf("%07.1f;%07.1f  ", (mat[i*10+j])[0], (mat[i*10+j])[1]);
    printf("\n");
  }
  printf("\n");
}

/**
 *  @brief Apply a basix operation on a matrix
 *  @param[in] from The matrix on which to apply the transformation
 *  @param[out] to The matrix to store the result into
 *  @param[in] dim The dimension of both matrix
 *
 *  This function apply a basic function on double to a matrix,
 *  taking the content of the matrix "from" and storing it
 *  into the matrix "to", not altering the first one.
 *
 *  This function can be used to copy a matrix to another with a function
 *  returning directly its parameter.
 *
 */
void matrix_operation(fftw_complex *from, fftw_complex *to, int dim,
                      double (*fun)(double)) {
  for (int i=0; i < dim*dim; i++) {
    (to[i])[0] = (*fun)((from[i])[0]);
    (to[i])[1] = (*fun)((from[i])[1]);
  }
}

/**
 *  @brief Get module and argument from algebraic notation
 *  @param[in] in The algebraic complexe
 *  @param[out] out The module and argument
 *
 *  @TODO better doxydoc
 *  
 */
void get_modarg(fftw_complex in, fftw_complex out) {
  out[0] = sqrt(in[0]*in[0] + in[1]*in[1]);
  if (out[0] == 0) {
    out[1] = 0;
  } else {
    out[1] = acos(in[0]/out[0]);
    /* while (out[1] <= -M_PI) */
    /*   out[1] += M_PI; */
    /* while (out[1] > M_PI) */
    /*   out[1] -= M_PI; */
  }
}

/**
 *  @brief Get algebraic notation from argument and module
 *  @param[in] in The module and argument
 *  @param[out] out The algebraic notation
 *
 *  @TODO better doxydoc
 *
 */
void get_algebraic(fftw_complex in, fftw_complex out) {
  out[0] = in[0]*cos(in[1]);
  out[1] = in[0]*sin(in[1]);
}
