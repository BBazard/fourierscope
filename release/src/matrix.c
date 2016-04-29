/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file implements several basic functions for
 *  matrices and complex numbers manipulation.
 *
 */

#include "include/matrix.h"

/*
 * This pragma is needed for GCC not to display a [-Wunused-parameter]
 * warning in the next function. The unused parameter is obviously
 * args, but we have to declare the function this way to avoid
 * type conflict when using it in matrix_operation function
 * which takes a double (*fun)(double, void**) as one of its
 * parameters.
 *
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 *  @brief The identity function used in matrix_operation
 *
 *  Here, args should be equal to the NULL pointer
 *
 */
double identity(double d, void **args) {return d;}
#pragma GCC diagnostic pop

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
 *  @brief Return an equivalent modulo dim
 *  @param[in] dim The dimension of the matrix
 *  @param[in] ind The index to change
 *  @param[out] int The value of the new index
 *
 *  Given any integer, return a value between 0 and dim-1
 *
 */
int matrix_cyclic(int ind, int dim) {
  int ret = ind%dim;
  return ret < 0 ? ret+dim : ret;
}

/**
 *  @brief Initialize a fftw_complex matrix
 *  @param[in] dim The matrix dimension
 *  @param[in,out] mat The matrix to initialize
 *  @param[in] value The value with which to initialize the matrix
 *
 *  This function initializes a matrix with a value in complexity O(n^{2})
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
 *  @brief Convert a complex from an algebraic to an exponential form
 *  @param[in] in The complex in algebraic form (real imaginary)
 *  @param[out] out The complex in exponential form (module argument)
 *
 *  This function takes an algebraic complex number a+ib,
 *  stored in the in parameter,
 *  and computes the module and argument, returned in the out parameter.
 *
 *  in and out parameters can be the same memory address.
 *
 */
void alg2exp(fftw_complex in, fftw_complex out) {
  fftw_complex tmp;
  tmp[0] = sqrt(in[0]*in[0] + in[1]*in[1]);
  if (tmp[0] == 0) {
    tmp[1] = 0;
  } else {
    tmp[1] = acos(in[0]/tmp[0]);
  }
  out[0] = tmp[0];
  out[1] = tmp[1];
}

/**
 *  @brief Convert a complex from an exponential to an algebraic form
 *  @param[in] out The complex in exponential form (module argument)
 *  @param[out] in The complex in algebraic form (real imaginary)
 *
 *  This function computes the algebraic form of a complex
 *  given its module and argument.
 *
 *  in and out parameters can be the same memory address.
 *
 */
void exp2alg(fftw_complex in, fftw_complex out) {
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
 *  This can also be used with alg2exp to get a matrix of module.
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
  if (smallDim > bigDim) {
    return 1;
  } else {
    for (int i = 0; i < smallDim; i++) {
      for (int j = 0; j < smallDim; j++) {
        int X = matrix_cyclic(i+offX, bigDim);
        int Y = matrix_cyclic(j+offY, bigDim);

        (small[i*smallDim+j])[0] = (big[X*bigDim+Y])[0];
        (small[i*smallDim+j])[1] = (big[X*bigDim+Y])[1];
      }
    }
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
  if (radius < 0)
    return;

  (out[matrix_cyclic(x, dim)*dim+matrix_cyclic(y, dim)])[0] =
    (in[matrix_cyclic(x, dim)*dim+matrix_cyclic(y, dim)])[0];
  (out[matrix_cyclic(x, dim)*dim+matrix_cyclic(y, dim)])[1] =
    (in[matrix_cyclic(x, dim)*dim+matrix_cyclic(y, dim)])[1];

  if (mat[matrix_cyclic(x+1, dim)*dim+matrix_cyclic(y, dim)] < radius) {
    mat[matrix_cyclic(x+1, dim)*dim+matrix_cyclic(y, dim)] = radius;
    von_neumann(matrix_cyclic(x+1, dim), matrix_cyclic(y, dim), radius-1, mat,
                dim, in, out);
  }
  if (mat[matrix_cyclic(x-1, dim)*dim+matrix_cyclic(y, dim)] < radius) {
    mat[matrix_cyclic(x-1, dim)*dim+matrix_cyclic(y, dim)] = radius;
    von_neumann(matrix_cyclic(x-1, dim), matrix_cyclic(y, dim), radius-1, mat,
                dim, in, out);
  }
  if (mat[matrix_cyclic(x, dim)*dim+matrix_cyclic(y+1, dim)] < radius) {
    mat[matrix_cyclic(x, dim)*dim+matrix_cyclic(y+1, dim)] = radius;
    von_neumann(matrix_cyclic(x, dim), matrix_cyclic(y+1, dim), radius-1, mat,
                dim, in, out);
  }
  if (mat[matrix_cyclic(x, dim)*dim+matrix_cyclic(y-1, dim)] < radius) {
    mat[matrix_cyclic(x, dim)*dim+matrix_cyclic(y-1, dim)] = radius;
    von_neumann(matrix_cyclic(x, dim), matrix_cyclic(y-1, dim), radius-1, mat,
                dim, in, out);
  }
}

/**
 *  @brief Copy a cell and call itself on the adjacent cells
 *  @param[in,out] ref The reference matrix used to remember the previous states
 *  @param[in] refX The coordinate in ref
 *  @param[in] refY The coordinate in ref
 *  @see copy_disk_ultimate
 *
 *  This function is a recursive function spanning from the center
 *  of a disk to its border by decreasing the
 *  radius by one each time it is called.
 *
 *  In fact it copies the part which is in the disk to the output matrix.
 *
 *  There is a folding of the disk in the in and out matrices.
 *
 */
void von_neumann_ultimate(fftw_complex* in, fftw_complex* out,
                          int dimIn, int dimOut,
                          int inX, int inY, int outX, int outY,
                          int *ref, int refX, int refY, int radius) {
  if (radius <= 0)
    return;

  int out_X = matrix_cyclic(outX+refX, dimOut);
  int out_Y = matrix_cyclic(outY+refY, dimOut);
  int in_X = matrix_cyclic(inX+refX, dimIn);
  int in_Y = matrix_cyclic(inY+refY, dimIn);

  (out[out_X*dimOut+out_Y])[0] = (in[in_X*dimIn+in_Y])[0];
  (out[out_X*dimOut+out_Y])[1] = (in[in_X*dimIn+in_Y])[1];

  if (ref[(refX+1)*(2*radius + 1) + (refY)] < radius)
    von_neumann_ultimate(in, out, dimIn, dimOut,
                         inX, inY, outX, outY, ref, refX+1, refY, radius-1);
  if (ref[(refX-1)*(2*radius + 1) + (refY)] < radius)
    von_neumann_ultimate(in, out, dimIn, dimOut,
                         inX, inY, outX, outY, ref, refX-1, refY, radius-1);
  if (ref[(refX)*(2*radius + 1) + (refY+1)] < radius)
    von_neumann_ultimate(in, out, dimIn, dimOut,
                         inX, inY, outX, outY, ref, refX, refY+1, radius-1);
  if (ref[(refX)*(2*radius + 1) + (refY-1)] < radius)
    von_neumann_ultimate(in, out, dimIn, dimOut,
                         inX, inY, outX, outY, ref, refX, refY-1, radius-1);
}

/**
 *  @brief Copy a disk from one matrix to another
 *  @param[in] in The fftw_complex 2d matrix used as input
 *  @param[out] out The fftw_complex 2d matrix used as output
 *  @param[in] dimIn The dimension of in (square matrix)
 *  @param[in] dimOut The dimension of out (square matrix)
 *  @param[in] inX Coordinate of the center of the disk in in
 *  @param[in] inY Coordinate of the center of the disk in in
 *  @param[in] outX Coordinate of the center of the disk in out
 *  @param[in] outY Coordinate of the center of the disk in out
 *  @param[in] radius The radius of the disk
 *  @return 1 If the radius is not adapted
 *  @return 0 Otherwise
 *
 *  This function computes a disk in the input matrix using
 *  taxicab geometry and copy this disk from in to out.
 *  The center of the circle has the coordinates
 *  [inX;inY] in the matrix in and 
 *  [outX;outY] in the matrix out.
 *  It folds like this:
 *
 *  0 1 X X 1
 *  1 X X X X
 *  X X X X X
 *  X X X X X
 *  1 X X X X
 *
 */
int copy_disk_ultimate(fftw_complex* in, fftw_complex* out,
                       int dimIn, int dimOut,
                       int inX, int inY, int outX, int outY,
                       int radius) {
  /* maybe useless */
  inX = inX % dimIn;
  inY = inY % dimIn;
  outX = outX % dimOut;
  outY = outY % dimOut;

  int minDim = (dimIn <= dimOut) ? dimIn : dimOut;
  int radius_max = (minDim-1)/2;

  if (minDim <= 0 || radius <= 0 ||
      radius > radius_max) {
    return 1;
  } else {
    int *ref = (int*) malloc((2*radius + 1) * sizeof(int));

    for (int i = 0; i < 2*radius + 1; i++)
      ref[i] = -1;

    int refX = radius;
    int refY = radius;
    ref[refX*(2*radius + 1) + refY] = radius;

    von_neumann_ultimate(in, out, dimIn, dimOut,
                         inX, inY, outX, outY, ref, refX, refY, radius);
    free(ref);
    return 0;
  }
}

/**
 *  @brief Copy a disk from one matrix to another
 *  @param[in] in The fftw_complex 2d matrix used as input
 *  @param[out] out The fftw_complex 2d matrix used as output
 *  @param[in] dim The dimension of both matrix (assumed square)
 *  @param[in] radius The radius of the disk
 *  @return 1 If the radius is not adapted
 *  @return 0 Otherwise
 *
 *  This function computes a disk in the input matrix using taxicab geometry
 *  And copy this disk from in to out
 *  The center of the circle has the coordinates [centerX;centerY]
 *  and stretch like this:
 *
 *  0 1 X X 1
 *  1 X X X X
 *  X X X X X
 *  X X X X X
 *  1 X X X X
 *
 */
int copy_disk_with_offset(fftw_complex* in, fftw_complex* out, int dim,
                         int radius, int centerX, int centerY) {
  return copy_disk_ultimate(in, out, dim, dim, centerX, centerY, centerX, centerY, radius);
}

/**
 *  @brief wrapper for copy_disk_with_offset
 *
 */
int copy_disk(fftw_complex* in, fftw_complex* out, int dim, int radius) {
  return copy_disk_with_offset(in, out, dim, radius, 0, 0);
}

/**
 *  @brief Recenter the matrix
 *  @param[in] in The matrix to modify
 *  @param[out] out The resulting matrix
 *  @param[in] dim The dimension of both matrices
 *  @param[in] offset The offset between the old the new center \
 *                    the offset is the same in X and Y
 *
 *  Examples assuming offset = dim/2
 *
 *  If dimension even
 *  0 1 X 3     5 X X X
 *  2 X X X ->  X X 4 X
 *  X X 5 X ->  X 3 0 1
 *  4 X X X     X X 2 X
 * 
 *  If dimension is odd
 *  0 1 X X 3     5 X X X X
 *  2 X X X X     X X 4 X X
 *  X X X X X ->  X 3 0 1 X
 *  X X X 5 X     X X 2 X X
 *  4 X X X X     X X X X X
 * 
 *  Applying for +offset and then -offset give the original matrix
 *
 *  "in" and "out" matrices must be different instances, namely
 *  the algorithm is not-in-place
 *  @bug Does not rotate the sub-matrix
 */
void matrix_recenter(fftw_complex *in, fftw_complex *out, int dim, int offset) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      int X = (i+offset)%dim;
      int Y = (j+offset)%dim;
      (out[X*dim+Y])[0] = (in[i*dim+j])[0];
      (out[X*dim+Y])[1] = (in[i*dim+j])[1];
    }
  }
}

