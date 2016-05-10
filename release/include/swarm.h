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

enum direction {DOWN, LEFT, UP, RIGHT};

void update_spectrum(double *thumb, int th_dim, fftw_plan forward,
                     fftw_plan backward, fftw_complex *time);
int abs_decrease(int a);
int move_one(int* index_x, int* index_y, int direction);
int move_streak(double **thumbnails, fftw_complex *time,
                fftw_complex *freq, fftw_complex *out,
                fftw_plan forward, fftw_plan backward,
                int th_dim, int out_dim, int radius, int delta, int side,
                int *pos_x, int *pos_y, int side_leds,
                int direction);
int swarm(double **thumbnails, int th_dim, int out_dim, int delta,
          const int lap_nbr, int radius, int jorga, fftw_complex *out);

#endif /* RELEASE_INCLUDE_SWARM_H_ */
