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
 *  @brief Computes the some operations for one thumbnail
 *  @param[in] thumb The treated thumbnail
 *  @param[in] th_dim The dimension of the thumbnail
 *  @param[in] radius The radius of the disk
 *  @param[in] forward The plan used for fourier transforms
 *  @param[in] backward The plan used for inverse transforms
 *  @param[in,out] time The source for FT and destination for IFT
 *  @param[in,out] freq The source for IFT and destination for FT
 *
 *  This function does the following:
 *
 *  c = ITF(b)
 *  d = mod(thumb)*ei^arg(c)
 *  e = TF(d)
 *
 *  where b = Disk((out[x][y]),radius)
 *
 *  The matrix extracted from out must be located in the freq
 *  fftw_complex * matrix corresponding to the plans
 *  e is actually stored in freq parameter and is available for use
 *  in the calling function
 *
 */
void update_spectrum(double *thumb, int th_dim, fftw_plan forward,
                     fftw_plan backward, fftw_complex *time) {
  /** @todo optimize fftw_plans */
  fftw_execute(backward);

  for (int i = 0; i < th_dim*th_dim; i++) {
    alg2exp(time[i], time[i]);
    (time[i])[0] = thumb[i];
    exp2alg(time[i], time[i]);
  }

  fftw_execute(forward);
}

/**
 *  @brief move the index used for thumbnails
 *
 *  index_x and index_y are increased of decreased of
 *  one according to direction.
 *
 */
int move_one(int* index_x, int* index_y, int direction) {
  switch (direction) {
    case DOWN:
      (*index_y)--;
      break;
    case LEFT:
      (*index_x)--;
      break;
    case UP:
      (*index_y)++;
      break;
    case RIGHT:
      (*index_x)++;
      break;
    default:
      return 1;
    }
  return 0;
}

/*
 *  @brief Update the leds between two corners in a row
 *  @return 1 if move_one error
 *  @return 2 if radius too big
 *
 *  the leds in the corner should be updated by another function
 */
int move_streak(double **thumbnails, fftw_complex *time,
                fftw_complex *freq, fftw_complex *out,
                fftw_plan forward, fftw_plan backward,
                int th_dim, int out_dim, int radius, int delta, int side,
                int *pos_x, int *pos_y, int side_leds,
                int direction) {
  int error = 0;
  int mid = (side-1)/2 + 1;  // = jorga+1
  /* the center of the disk in out */
  int centerX, centerY;
  for (int remaining = side_leds; remaining != 0; remaining--) {
    error = move_one(pos_x, pos_y, direction);
    centerX = (*pos_x-mid)*delta;
    centerY = (*pos_y-mid)*delta;
    if (copy_disk_ultimate(out, freq, out_dim, th_dim, *pos_x*delta,
                           *pos_y*delta, centerX, centerY, radius))
      error = 2;
    update_spectrum(thumbnails[*pos_x*side+*pos_y],
                    th_dim, forward, backward, time);
    if (copy_disk_ultimate(freq, out, th_dim, out_dim, centerX, centerY,
                           *pos_x*delta, *pos_y*delta, radius))
      error = 2;
  }
  return error;
}

/**
 *  @brief Unite multiple small images in a big one
 *  @param[in] thumbnails All the thumbnails in one big matrix
 *  @param[in] th_dim The dimension of each thumbnail
 *  @param[in] out_dim The dimension of the final image
 *  @param[in] delta The distance between two thumbnail centers
 *  @param[in] radius The radius of the extracted circle
 *  @param[in] jorga The dimension of thumbnails is (2*jorga+1)^2
 *  @param[out] out The retrieved image after the algorithm is done
 *
 *  @return 1 If memory allocation failed or incompatible parameters
 *  @return 0 0therwise
 */
int swarm(double **thumbnails, int th_dim, int out_dim, int delta,
          int radius, int jorga, fftw_complex *out) {
  printf("### STARTING SWARM ###\n");
  /** @todo check these formula */
  /* check if out is big enough */
  if (jorga*delta + th_dim/2 > out_dim/2)
    return 1;

  fftw_complex *time;
  fftw_complex *freq;
  fftw_plan forward;
  fftw_plan backward;

  if ( (time = (fftw_complex *) malloc(th_dim*th_dim*
                                      sizeof(fftw_complex))) == NULL )
    return 1;

  if ( (freq = (fftw_complex *) malloc(th_dim*th_dim*
                                     sizeof(fftw_complex))) == NULL )
    return 1;

  forward = fftw_plan_dft_2d(th_dim, th_dim, time, freq,
                              FFTW_FORWARD, FFTW_ESTIMATE);
  backward = fftw_plan_dft_2d(th_dim, th_dim, freq, time,
                              FFTW_BACKWARD, FFTW_ESTIMATE);

  /*
   *  Spiral loop
   *
   *  Side is an odd number (2*jorga+1)
   *
   *  In one "lap" all the thumbnails are exploited
   *  In one "streak" all the leds in one side (not a full side) are exploited
   *  In one "whorl" all the leds in half a square are exploited
   *
   *  One whorl is 4 streak
   *  One lap is (side-1)/2  whorls plus one special streak
   */

  /* the coordinates of the thumbnail in the center
   * of thumbnails are [mid;mid] */
  const int mid = jorga+1;

  /* the side of thumbnails */
  const int side = 2*jorga+1;

  const int lap_nbr = 2;

  for (int lap = 0; lap < lap_nbr; lap++) {
    /* the direction of the next led */
    int direction = DOWN;

    /*
     * the number of leds exploited (update_spectrum) in the same streak
     * exluding the leds in the corner
     *
     * for example: for the first move of the lap, side_leds is 0 cause
     * we have two successive leds that are in a corner
     */
    int side_leds = 0;

    /* index in thumbnails */
    int pos_x = mid;
    int pos_y = mid;


    /* !! debug_start !! */
    char name[60];
    int step = 0;

    double *out_io0 = (double*) malloc(out_dim*out_dim*sizeof(double));
    double *th_io0 = (double*) malloc(th_dim*th_dim*sizeof(double));
    double *out_io1 = (double*) malloc(out_dim*out_dim*sizeof(double));
    double *th_io1 = (double*) malloc(th_dim*th_dim*sizeof(double));
    fftw_complex *out_tmp;
    fftw_complex *th_tmp;
    out_tmp = (fftw_complex*) fftw_malloc(out_dim*out_dim*
                                          sizeof(fftw_complex));
    th_tmp = (fftw_complex*) fftw_malloc(th_dim*th_dim*
                                         sizeof(fftw_complex));

    for (int i = 0; i < out_dim * out_dim; i++) {
      alg2exp(out[i],out_tmp[i]);
      out_io0[i] = (out_tmp[i])[0];
      out_io1[i] = (out_tmp[i])[1];
    }
    snprintf(name, 50, "build/swarm0_%.4d.tiff", step++);
    tiff_frommatrix(name, out_io0, out_dim, out_dim);
    snprintf(name, 50, "build/swarm1_%.4d.tiff", step++);
    tiff_frommatrix(name, out_io1, out_dim, out_dim);
    /* !! debug_end !! */

    copy_disk_ultimate(out, freq, out_dim, th_dim, 0, 0, 0, 0, radius);
    /* special: no adjacent circle */
    update_spectrum(thumbnails[pos_x*side+pos_y],
                    th_dim, forward, backward, time);
    copy_disk_ultimate(freq, out, th_dim, out_dim, 0, 0, 0, 0, radius);

    /* !! debug_start !! */
    for (int i = 0; i < out_dim * out_dim; i++) {
      alg2exp(out[i],out_tmp[i]);
      out_io0[i] = (out_tmp[i])[0];
      out_io1[i] = (out_tmp[i])[1];
    }
    snprintf(name, 50, "build/swarm0_%.4d.tiff", step++);
    tiff_frommatrix(name, out_io0, out_dim, out_dim);
    snprintf(name, 50, "build/swarm1_%.4d.tiff", step++);
    tiff_frommatrix(name, out_io1, out_dim, out_dim);

    for (int i = 0; i < th_dim * th_dim; i++) {
      alg2exp(freq[i], th_tmp[i]);
      th_io0[i] = (th_tmp[i])[0];
      th_io1[i] = (th_tmp[i])[1];
    }
    snprintf(name, 50, "build/freq0_%.4d.tiff", step++);
    tiff_frommatrix(name, th_io0, th_dim, th_dim);
    snprintf(name, 50, "build/freq1_%.4d.tiff", step++);
    tiff_frommatrix(name, th_io1, th_dim, th_dim);
    /* !! debug_end !! */

    /*
     * one whorl correspond of a move going from one corner
     * to the same but farther from the center by going in spiral
     * example : from [-2,2] to [-3,3]
     *
     * a whorl correspond to four streaks
     */
    for (int whorl = 1; whorl <= jorga; whorl++) {
      /* side leds */
      move_streak(thumbnails, time, freq, out, forward, backward,
                  th_dim, out_dim, radius, delta, side, &pos_x, &pos_y,
                  side_leds, direction);

      /* @bug error management */
      int error;
      int centerX, centerY;

      /* special: corner led */
      centerX = (pos_x-mid)*delta;
      centerY = (pos_y-mid)*delta;
      if (copy_disk_ultimate(out, freq, out_dim, th_dim, pos_x*delta,
                             pos_y*delta, centerX, centerY, radius))
        error = 2;
      update_spectrum(thumbnails[pos_x*side+pos_y],
                      th_dim, forward, backward, time);
      if (copy_disk_ultimate(freq, out, th_dim, out_dim, centerX, centerY,
                             pos_x*delta, pos_y*delta, radius))
        error = 2;

      /* !! debug_start !! */
      for (int i = 0; i < out_dim * out_dim; i++) {
        alg2exp(out[i],out_tmp[i]);
        out_io0[i] = (out_tmp[i])[0];
        out_io1[i] = (out_tmp[i])[1];
      }
      snprintf(name, 50, "build/swarm0_%.4d.tiff", step++);
      tiff_frommatrix(name, out_io0, out_dim, out_dim);
      snprintf(name, 50, "build/swarm1_%.4d.tiff", step++);
      tiff_frommatrix(name, out_io1, out_dim, out_dim);

      for (int i = 0; i < th_dim * th_dim; i++) {
        alg2exp(freq[i], th_tmp[i]);
        th_io0[i] = (th_tmp[i])[0];
        th_io1[i] = (th_tmp[i])[1];
      }
      snprintf(name, 50, "build/freq0_%.4d.tiff", step++);
      tiff_frommatrix(name, th_io0, th_dim, th_dim);
      snprintf(name, 50, "build/freq1_%.4d.tiff", step++);
      tiff_frommatrix(name, th_io1, th_dim, th_dim);
      /* !! debug_end !! */

      /* direction change: clockwise route */
      direction = (direction+1)%4;

      /* side leds */
      move_streak(thumbnails, time, freq, out, forward, backward,
                  th_dim, out_dim, radius, delta, side, &pos_x, &pos_y,
                  side_leds, direction);

      /* special: corner led */
      centerX = (pos_x-mid)*delta;
      centerY = (pos_y-mid)*delta;
      if (copy_disk_ultimate(out, freq, out_dim, th_dim, pos_x*delta,
                             pos_y*delta, centerX, centerY, radius))
        error = 2;
      update_spectrum(thumbnails[pos_x*side+pos_y],
                      th_dim, forward, backward, time);
      if (copy_disk_ultimate(freq, out, th_dim, out_dim, centerX, centerY,
                             pos_x*delta, pos_y*delta, radius))
        error = 2;

      /* !! debug_start !! */
      for (int i = 0; i < out_dim * out_dim; i++) {
        alg2exp(out[i],out_tmp[i]);
        out_io0[i] = (out_tmp[i])[0];
        out_io1[i] = (out_tmp[i])[1];
      }
      snprintf(name, 50, "build/swarm0_%.4d.tiff", step++);
      tiff_frommatrix(name, out_io0, out_dim, out_dim);
      snprintf(name, 50, "build/swarm1_%.4d.tiff", step++);
      tiff_frommatrix(name, out_io1, out_dim, out_dim);

      for (int i = 0; i < th_dim * th_dim; i++) {
        alg2exp(freq[i], th_tmp[i]);
        th_io0[i] = (th_tmp[i])[0];
        th_io1[i] = (th_tmp[i])[1];
      }
      snprintf(name, 50, "build/freq0_%.4d.tiff", step++);
      tiff_frommatrix(name, th_io0, th_dim, th_dim);
      snprintf(name, 50, "build/freq1_%.4d.tiff", step++);
      tiff_frommatrix(name, th_io1, th_dim, th_dim);
      /* !! debug_end !! */

      direction = (direction+1)%4;
      side_leds++;
    }

    /* at this point side_leds = side */
    /* we just need to finish the spiral */

    move_streak(thumbnails, time, freq, out, forward, backward,
                th_dim, out_dim, radius, delta, side, &pos_x, &pos_y,
                side_leds, direction);

    /* there is no corner led here */
    /* the spiral lap is done at this point */

    /* !! debug_start !! */
    free(out_io0);
    free(out_io1);
    free(th_io0);
    free(th_io1);
    fftw_free(out_tmp);
    fftw_free(th_tmp);
    /* !! debug_end !! */
  }

  fftw_destroy_plan(forward);
  fftw_destroy_plan(backward);

  fftw_free(time);
  fftw_free(freq);

  return 0;
}
