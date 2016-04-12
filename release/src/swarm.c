/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file implements the function that unite several images in
 *  one.
 *
 */

#include "include/swarm.h"

/* image centrale D(0,0) -> TF -> Disk D(0,0,r) -> TFI -> */
/* ei^{\phi}*Image départ -> TF -> actualisation du spectre dans D(0,0,R) */
void update_spectrum(fftw_complex *thumb, int th_dim, int radius,
                     fftw_plan forward, fftw_plan backward, fftw_complex *itf,
                     fftw_complex *tf) {
  matrix_operation(thumb, itf, th_dim, identity, NULL);
  fftw_execute(forward);

  for (int i = 0; i < th_dim*th_dim; i++) {
    (itf[i])[0] = 0;
    (itf[i])[1] = 0;
  }

  copy_disk(tf, itf, th_dim, radius);

  /** @todo optimize fftw_plans */
  matrix_operation(itf, tf, th_dim, identity, NULL);
  fftw_execute(backward);

  for (int i = 0; i < th_dim*th_dim; i++) {
    get_modarg(thumb[i], tf[i]);
    get_modarg(itf[i], itf[i]);
    (itf[i])[0] = (tf[i])[0];
    get_algebraic(itf[i], itf[i]);
  }

  fftw_execute(forward);
}

#if 0
/**
 *  @brief Unite multiple small images in a big one
 *  @param[in] thumbnails All the thumbnails in one big matrix
 *  @param[in] th_dim The dimension of each thumbnail
 *  @param[in] out_dim The dimension of the final image
 *  @param[in] delta The distance between two thumbnail centers
 *  @param[out] out The retrieved image after the algorithm is done
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
      /** @todo matrix ind */
      /* update_spectrum(thumbnails[??], th_dim, radius, forward, backward, */
      /*                 itf, tf); */
      /** @todo add spectrum to matrix */
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
#endif
