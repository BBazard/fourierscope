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

static const double PI = acos(-1.0);

void matrix_init(int dim, fftw_complex *mat, double value);
void matrix_random(int dim, fftw_complex *mat, int max_rand);
void matrix_print(int dim, fftw_complex *mat);
void matrix_operation(fftw_complex *from, fftw_complex *to, int dim,
                      double (*fun)(double, void**), void **args);
void get_modarg(fftw_complex in, fftw_complex out);
void get_algebraic(fftw_complex in, fftw_complex out);

void matrix_realpart(int dim, fftw_complex *in, double *out);

double matrix_max(int diml, int dimw, double *matrix);
double matrix_min(int diml, int dimw, double *matrix);

int matrix_extract(int smallDim, int bigDim, fftw_complex* small,
                   fftw_complex* big, int offX, int offY);

double identity(double d, void **args);
double div_dim(double d, void **args);

int cut_disk(fftw_complex* in, fftw_complex* out, int dim, int radius);
int cut_disk_with_offset(fftw_complex* in, fftw_complex* out, int dim,
                         int radius, int centerX, int centerY);
void von_neumann(int x, int y, int radius, int *mat, int dim,
                 fftw_complex *in, fftw_complex *out);

void matrix_recenter(fftw_complex *in, fftw_complex *out, int dim, int offset);

#endif /* RELEASE_INCLUDE_MATRIX_H_ */
