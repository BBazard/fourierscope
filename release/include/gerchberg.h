/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  Gerchberg-Saxon algorithm header
 *
 */

#ifndef RELEASE_INCLUDE_GERCHBERG_H_
#define RELEASE_INCLUDE_GERCHBERG_H_

#include "include/matrix.h"

double identity(double d);
double div_dim(double d);

int cut_disk(fftw_complex* in, fftw_complex* out, int dim, int radius);
void von_neumann(int x, int y, int radius, int *mat, int dim,
                 fftw_complex *in, fftw_complex *out);
void gerchberg(int dim, fftw_complex *input, fftw_complex *output,
               int exec_limit, int radius);

#endif /* RELEASE_INCLUDE_GERCHBERG_H_ */
