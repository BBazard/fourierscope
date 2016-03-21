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
    tmp[1] = acos(in[0]/tmp[0]);
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
 *  in and out parameters can be the same memory address.
 *
 */
void get_algebraic(fftw_complex in, fftw_complex out) {
  fftw_complex tmp;
  tmp[0] = in[0]*cos(in[1]);
  tmp[1] = in[0]*sin(in[1]);
  out[0] = tmp[0];
  out[1] = tmp[1];
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

/**
 *  @brief Set all the matrix cells outside of a centered disk to 0
 *  @param[in] in The fftw_complex 2d matrix used as input
 *  @param[out] out The fftw_complex 2d matrix used as output
 *  @param[in] dim The dimension of the matrix (assumed square)
 *  @param[in] radius The radius of the disk
 *  @return 1 If the radius of the circle cannot fit in the matrix
 *  @return 0 Otherwise
 *
 *  This is just a wrapper of cut_disk_with_offset
 *
 */
int cut_disk(fftw_complex* in, fftw_complex* out, int dim, int radius) {
  int mid = dim/2+dim%2;
  return cut_disk_with_offset(in, out, dim, radius, mid, mid);
}

/**
 *  @brief Set all the matrix cells outside of a disk to 0
 *  @param[in] in The fftw_complex 2d matrix used as input
 *  @param[out] out The fftw_complex 2d matrix used as output
 *  @param[in] dim The dimension of the matrix (assumed square)
 *  @param[in] radius The radius of the disk
 *  @param[in] centerX The X coordinate of the center of the disk
 *  @param[in] centerX The Y coordinate of the center of the disk
 *  @return 1 If the radius of the circle cannot fit in the matrix
 *  @return 0 Otherwise
 *
 *  This function computes a disk in the input matrix using taxicab geometry.
 *  The center of the circle has the coordinates centerX;centerY
 *
 */
int cut_disk_with_offset(fftw_complex* in, fftw_complex* out, int dim,
                         int radius, int centerX, int centerY) {
  int mid = dim/2+dim%2;
  int offX = centerX-mid;
  int offY = centerY-mid;

  int radius_max = (dim-1)/2;  // without offset
  // when the dimension is even
  // a negative offset is mitigated by one
  // for exemple with dim = 4 the "mid" is bottom-right
  // even with a negative offset of one, the maximum radius is still 0
  if (dim%2 == 0) {
    // as offX is only use to test if the disk fit
    // offX++ only reduce abs(offX)
    if (offX < 0)
      offX++;
    if (offY < 0)
      offY++;
  }
  int off_max = 0;
  if (abs(offX) > off_max)
    off_max = abs(offX);
  if (abs(offY) > off_max)
    off_max = abs(offY);

  radius_max -= off_max;

  if (dim <= 0 || radius <= 0 ||
      radius > radius_max) {
    return 1;
  } else {
    int *ref;
    ref = (int*) malloc(dim * dim * sizeof(int));

    for (int i = 0; i < dim * dim; i++)
      ref[i] = -1;

    ref[centerX*dim+centerY] = radius;
    von_neumann(centerX, centerY, radius-1, ref, dim, in, out);
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
