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
#include "include/tiffio.h"

void update_spectrum(fftw_complex *thumb, int th_dim, int radius,
                     fftw_plan forward, fftw_plan backward, fftw_complex *itf,
                     fftw_complex *tf);
int abs_decrease(int a);
int move_one(int* index_x, int* index_y, int* direction_x, int* direction_y);
int move_streak(fftw_complex **thumbnails, fftw_complex *itf,
                fftw_complex *tf, fftw_plan forward, fftw_plan backward,
                int th_dim, int radius,
                int side, int pos_x, int pos_y, int X, int Y);
int swarm(fftw_complex **thumbnails, int th_dim, int out_dim, int delta,
          int radius, int jorga, fftw_complex *out);

#endif /* RELEASE_INCLUDE_SWARM_H_ */
