/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This is the main file
 *
 */

#include "include/main.h"


/**
 *  @brief Main function
 *
 *  @todo WRITE IT (this just for filling the hole)
 *
 */
int main(int argc, char **argv) {
  int out_dim;
  int th_dim;
  int radius;

  int jorga_x, jorga_y;

  int delta_x, delta_y;

  int lap_nbr;

  out_dim = 1000;
  jorga_x = jorga_y = 3;
  th_dim = 100;
  radius = 40;
  delta_x = delta_y = 50; //0.3*radius
  lap_nbr = 2;

  srand(time(NULL));

  fftw_complex *out;
  double **thumbnails;
  fftw_complex *thumbnail_buf[2];
  char *name;

  double *out_io;

  fftw_plan forward;
  fftw_plan backward;

  fftw_init_threads();
  fftw_plan_with_nthreads(omp_get_max_threads());

  out = (fftw_complex*) fftw_malloc(out_dim * out_dim *
                                    sizeof(fftw_complex));
  thumbnail_buf[0] = (fftw_complex*) fftw_malloc(th_dim * th_dim *
                                                 sizeof(fftw_complex));
  thumbnail_buf[1] = (fftw_complex*) fftw_malloc(th_dim * th_dim *
                                                 sizeof(fftw_complex));

  thumbnails = (double**) malloc((2*jorga_x+1)*(2*jorga_y+1)*
                                 sizeof(double*));
  for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
    thumbnails[i] = (double*) fftw_malloc(th_dim * th_dim *
                                          sizeof(double));

  name = (char*) malloc(sizeof(char)*(strlen("build/xxxxxyyyyy.tiff")+1));
  out_io = (double*) malloc(out_dim * out_dim * sizeof(double));

  forward = fftw_plan_dft_2d(out_dim, out_dim, out, out,
                             FFTW_FORWARD, FFTW_ESTIMATE);
  backward = fftw_plan_dft_2d(th_dim, th_dim, thumbnail_buf[1],
                              thumbnail_buf[0], FFTW_BACKWARD, FFTW_ESTIMATE);

  for (int i=0; i < out_dim*out_dim; i++) {
    (out[i])[0] = 0;
    (out[i])[1] = 0;
    out_io[i] = 0;
  }

  for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
    for (int j = 0; j < th_dim*th_dim ; j++)
      ((thumbnails[i])[j]) = 0;

  int name_size;
  name_size = strlen("build/swarm_with_jorga_eq_nn.tiff")+1;
  free(name);
  name = (char*) malloc(sizeof(char)*name_size);
  backward = fftw_plan_dft_2d(out_dim, out_dim, out, out,
                              FFTW_BACKWARD, FFTW_ESTIMATE);

  for (int i = 0; i < out_dim * out_dim; i++)
    (out[i])[0] = (out[i])[1] = 0;

  swarm(thumbnails, th_dim, out_dim,
        delta_x, lap_nbr, radius, jorga_x, out);

  fftw_execute(backward);
  div_dim(out, out, out_dim);

  for (int i = 0; i < out_dim * out_dim; i++) {
    alg2exp(out[i], out[i]);
    out_io[i] = (out[i])[0];
  }

  snprintf(name, name_size,
           "build/swarm_with_j%.2d_d%.2d_r%.2d.tiff",
           jorga_x, delta_x, radius);
  tiff_frommatrix(name, out_io, out_dim, out_dim);
  fftw_destroy_plan(forward);
  fftw_destroy_plan(backward);

  free(out_io);
  free(name);

  for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
    free(thumbnails[i]);
  free(thumbnails);

  fftw_free(thumbnail_buf[0]);
  fftw_free(thumbnail_buf[1]);

  fftw_free(out);
  fftw_cleanup_threads();

  return 0;
}
