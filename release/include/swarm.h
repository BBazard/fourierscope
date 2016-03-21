/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Swarm function header
 *
 */

#ifndef RELEASE_INCLUDE_SWARM_H_
#define RELEASE_INCLUDE_SWARM_H_

#include "include/matrix.h"

void update_spetrum(fftw_complex *thumb, int th_dim, fftw_plan forward,
                    fftw_plan backward, fftw_complex *tf, fftw_complex *itf);
int swarm(fftw_complex **thumbnails, int th_dim, int out_dim, int delta,
          int radius, fftw_complex *out);

#endif /* RELEASE_INCLUDE_SWARM_H_ */
