/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Matrix functions header
 *
 */

#ifndef RELEASE_INCLUDE_MATRIX_H_
#define RELEASE_INCLUDE_MATRIX_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <fftw3.h>

#define PI acos(-1.0)

void matrix_copy(fftw_complex *in, fftw_complex *out, int dim);
#define matrix_copy _matrix_copy

void div_dim(fftw_complex *in, fftw_complex *out, int dim);

int matrix_cyclic(int ind, int dim);
void matrix_init(int dim, fftw_complex *mat, double value);
void matrix_random(int dim, fftw_complex *mat, int max_rand);
void matrix_print(int dim, fftw_complex *mat);
void alg2exp(fftw_complex in, fftw_complex out);
void exp2alg(fftw_complex in, fftw_complex out);

void matrix_realpart(int dim, fftw_complex *in, double *out);

double matrix_max(int diml, int dimw, double *matrix);
double matrix_min(int diml, int dimw, double *matrix);

int matrix_extract(int smallDim, int bigDim, fftw_complex* small,
                   fftw_complex* big, int offX, int offY);

int copy_disk_with_offset(fftw_complex* in, fftw_complex* out, int dim,
                         int radius, int centerX, int centerY);
int copy_disk(fftw_complex* in, fftw_complex* out, int dim, int radius);
int copy_disk_ultimate(fftw_complex* in, fftw_complex* out,
                       int dimIn, int dimOut,
                       int inX, int inY, int outX, int outY,
                       int radius);
void von_neumann(int x, int y, int radius, int *mat, int dim,
                 fftw_complex *in, fftw_complex *out);
void von_neumann_ultimate(fftw_complex* in, fftw_complex* out,
                          int dimIn, int dimOut,
                          int inX, int inY, int outX, int outY,
                          int *ref, int refX, int refY,
                          int radius, int radius_max);
void matrix_recenter(fftw_complex *in, fftw_complex *out, int dim, int offset);

#endif /* RELEASE_INCLUDE_MATRIX_H_ */
