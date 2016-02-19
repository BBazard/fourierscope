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
 *  in and out parameters can be the same memory address.
 *
 */
void get_modarg(fftw_complex in, fftw_complex out) {
  fftw_complex tmp;
  tmp[0] = sqrt(in[0]*in[0] + in[1]*in[1]);
  if (tmp[0] == 0) {
    tmp[1] = 0;
  } else {
    tmp[1] = acos(in[0]/out[0]);
    while (tmp[1] <= 0)
      tmp[1] += 2*PI;
    while (tmp[1] > 2*PI)
      tmp[1] -= 2*PI;
  }
  out[0] = tmp[0];
  out[1] = tmp[1];
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
 *  @param[in] complex_matrix The fftw_complex matrix
 *  @param[out] real_matrix The real double matrix that results
 *
 *  Get the real part of a fftw_complex into a double matrix.
 *  This can also be used with get_modarg to get a matrix of module.
 *
 */
void matrix_realpart(int dim, fftw_complex *complex_matrix,
                     double *real_matrix) {
  for (int i=0; i < dim*dim; i++) {
    real_matrix[i] = (complex_matrix[i])[0];
  }
}

/**
 *  @todo doxydoc
 *
 */
double matrix_max(int diml, int dimw, double *matrix) {
  double max = matrix[0];
  for (int i=0; i < diml; i++)
    for (int j=1; j < dimw; j++)
      if (matrix[i*dimw+j] > max)
        max = matrix[i*dimw+j];
  return max;
}

/**
 *  @todo doxydoc
 *
 */
double matrix_min(int diml, int dimw, double *matrix) {
  double min = matrix[0];
  for (int i=0; i < diml; i++)
    for (int j=1; j < dimw; j++)
      if (matrix[i*dimw+j] < min)
        min = matrix[i*dimw+j];
  return min;
}

/**
 *  @brief Extract a little part of a matrix from a bigger one
 *
 *  @return 1 If the little matrix can't fit in the big one
 *  @return 0 Otherwise
 *
 */
int matrix_extract(int smallDim, int bigDim, fftw_complex* small,
                   fftw_complex* big, int offX, int offY) {
  if (smallDim + offX > bigDim || smallDim + offY > bigDim) {
    return 1;
  } else {
    for (int i = 0; i < smallDim; i++) {
      for (int j = 0; j < smallDim; j++) {
        (small[i*smallDim+j])[0] = (big[(i+offX)*bigDim+(j+offY)])[0];
        (small[i*smallDim+j])[1] = (big[(i+offX)*bigDim+(j+offY)])[1];
      }
    }
    return 0;
  }
}
