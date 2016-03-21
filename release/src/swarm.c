/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file implements the function that unite several images in
 *  one.
 *
 */

#include "include/swarm.h"

/**
 *  @brief Unite multiples small images in a big one
 *  @param[in] thumbnails All the thumbnails in one big matrix
 *  @param[in] th_dim The dimension of each thumbnail
 *  @param[in] out_dim The dimension of the final image
 *  @param[in] delta The distance between two thumbnail centers
 *  @param[out] out The image retrieved after algorithm is comple
 *
 *  @return 1 If memory allocation failed
 *  @return 0 0therwise
 */
int swarm(fftw_complex **thumbnails, int th_dim, int out_dim, int delta,
          int radius, fftw_complex *out) {
  fftw_complex *itf;
  fftw_complex *tf;
  fftw_plan forward;
  fftw_plan backward;

  int i = 0;
  int j = 0;
  int s = 1;
  int jorga_x = 2;

  if ( (itf = (fftw_complex *) malloc(th_dim*th_dim*
                                      sizeof(fftw_complex))) == NULL )
    return 1;

  if ( (tf = (fftw_complex *) malloc(th_dim*th_dim*
                                     sizeof(fftw_complex))) == NULL )
    return 1;

  forward = fftw_plan_dft_2d(th_dim, th_dim, itf, tf,
                              FFTW_FORWARD, FFTW_ESTIMATE);
  backward = fftw_plan_dft_2d(th_dim, th_dim, tf, itf,
                              FFTW_BACKWARD, FFTW_ESTIMATE);

  /* Spiral loop*/
  for (int p = 1; p < 2*jorga_x+1; p++) {
    for (int a = 0; a < p; i-=s, a++) {
      printf("%d, %d\n", i, j);
    }
    for (int a = 0; a < p; j+=s, a++) {
      printf("%d, %d\n", i, j);
    }
    s *= -1;
  }
  for (int a = 0; a < 2*jorga_x+1; i-=s, a++) {
    printf("%d, %d\n", i, j);
  }
  return 0;
}
