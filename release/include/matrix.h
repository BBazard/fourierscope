/**
 *  @file
 *
 *  Matrix functions header
 *
 */

#ifndef EPICS_INCLUDE_MATRIX_H
#define EPICS_INCLUDE_MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <fftw3.h>

void matrix_init(int dim, fftw_complex *mat, double value);
void matrix_random(int dim, fftw_complex *mat, int max_rand);
void matrix_print(int dim, fftw_complex *mat);
void matrix_operation(fftw_complex *from, fftw_complex *to, int dim,
                      double (*fun)(double));
void get_modarg(fftw_complex in, fftw_complex out);
void get_algebraic(fftw_complex in, fftw_complex out);

#endif /* EPICS_INCLUDE_MATRIX_H */
