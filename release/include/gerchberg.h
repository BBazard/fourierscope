/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  Gerchberg-Saxton algorithm header
 *
 */

#ifndef RELEASE_INCLUDE_GERCHBERG_H_
#define RELEASE_INCLUDE_GERCHBERG_H_

#include "include/matrix.h"

void gerchberg(int dim, fftw_complex *input, fftw_complex *output,
               int exec_limit, int radius);

#endif /* RELEASE_INCLUDE_GERCHBERG_H_ */
