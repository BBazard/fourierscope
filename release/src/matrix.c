/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file implements several basic functions for
 *  matrices and complex numbers manipulation.
 *
 */

#include "include/matrix.h"

/**
 *  @brief Initialize a fftw_complex matrix
 *  @param[in] dim The matrix dimension
 *  @param[in,out] mat The matrix to initialize
 *  @param[in] value The value with which to initialize the matrix
 *
 *  This function initializes a matrix with 0 in complexity O(n^{2})
 *
 */
void matrix_init(int dim, fftw_complex *mat, double value) {
  for (int i=0; i < dim*dim; i++) {
    (mat[i])[0] = value;
    (mat[i])[1] = value;
  }
}

/**
 *  @brief Initialize a matrix with random complexes
 *  @param[in] dim The dimension of the matrix
 *  @param[out] mat The fftw_complex matrix to initialize
 *  @param[in] max_rand The maximum value for random numbers
 *
 *  This function initializes a matrix of fftw_complex with
 *  randomly generated integers
 *
 *
 */
void matrix_random(int dim, fftw_complex *mat, int max_rand) {
  srand(time(NULL));

  for (int i=0; i < dim; i++)
    for (int j=0; j < dim; j++) {
      (mat[i*dim+j])[0] = rand() % max_rand; /* NOLINT(runtime/threadsafe_fn) */
      (mat[i*dim+j])[1] = 0;
    }
}

/**
 *  @brief Print a matrix
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
      printf("%7.1f;%7.1f  ", (mat[i*dim+j])[0], (mat[i*dim+j])[1]);
    printf("\n");
  }
  printf("\n");
}

/**
 *  @brief Apply a basic operation on a matrix
 *  @param[in] from The matrix on which to apply the transformation
 *  @param[out] to The matrix to store the result into
 *  @param[in] dim The dimension of both matrix
 *  @param[in] fun The function to apply to each real and imaginary part
 *  @param[in] args The array of pointers to several parameters
 *
 *  This function apply a basic function on doubles to a matrix,
 *  taking the content of the matrix "from" and storing it
 *  into the matrix "to", not altering the first one.
 *
 *  This function can be used to copy a matrix to another with a function
 *  returning directly its parameter.
 *
 */
void matrix_operation(fftw_complex *from, fftw_complex *to, int dim,
                      double (*fun)(double, void**), void **args) {
  for (int i=0; i < dim*dim; i++) {
    (to[i])[0] = (*fun)((from[i])[0], args);
    (to[i])[1] = (*fun)((from[i])[1], args);
  }
}

/**
 *  @brief Get module and argument from algebraic form
 *  @param[in] in The algebraic complex
 *  @param[out] out The module and argument
 *
 *  This function takes an algebraic complex number a+ib,
 *  stored in the in parameter,
 *  and computes the module and argument, returned in the out parameter.
 *
 */
void get_modarg(fftw_complex in, fftw_complex out) {
  out[0] = sqrt(in[0]*in[0] + in[1]*in[1]);
  if (out[0] == 0) {
    out[1] = 0;
  } else {
    out[1] = acos(in[0]/out[0]);
    while (out[1] <= 0)
      out[1] += 2*PI;
    while (out[1] > 2*PI)
      out[1] -= 2*PI;
  }
}

/**
 *  @brief Get algebraic form from argument and module
 *  @param[in] in The module and argument
 *  @param[out] out The algebraic form
 *
 *  This function computes the algebraic form of a complex
 *  given its module and argument.
 *
 */
void get_algebraic(fftw_complex in, fftw_complex out) {
  out[0] = in[0]*cos(in[1]);
  out[1] = in[0]*sin(in[1]);
}

/**
 *  @brief Get the real part of a fftw_complex matrix
 *  @param[in] dim The dimension of both matrix
 *  @param[in] complex The fftw_complex matrix
 *  @param[out] real The real double matrix that results
 *
 *  Get the real part of a fftw_complex into a double matrix.
 *  This can also be used with get_modarg to get a matrix of module.
 *
 */
void matrix_realpart(int dim, fftw_complex *complex, double *real) {
  for (int i=0; i < dim*dim; i++) {
    real[i] = (complex[i])[0];
  }
}
