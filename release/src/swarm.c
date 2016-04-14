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
 *  @param[in,out] itf The source for FT and destination for IFT
 *  @param[in,out] tf The source for IFT and destination for FT
 *
 *  This function does the following:
 *
 *  a = TF(thumb)
 *  b = Disk(a,radius)
 *  c = ITF(b)
 *  d = mod(thumb)*ei^arg(c)
 *  e = TF(d)
 *
 *  e is actually stored in tf parameter and is available for use
 *  in the calling function
 *
 */
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
      index_y--;
      break;
    case LEFT:
      index_x--;
      break;
    case UP:
      index_y++;
      break;
    case RIGHT:
      index_x++;
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
int move_streak(fftw_complex **thumbnails, fftw_complex *itf,
                fftw_complex *tf, fftw_complex *out,
                fftw_plan forward, fftw_plan backward,
                int th_dim, int radius, int delta, int side,
                int pos_x, int pos_y, int side_leds,
                int direction) {
  int error = 0;
  int mid = (side-1)/2 + 1;  // = jorga+1
  /* the center of the disk in out */
  int centerX, centerY;
  for (int remaining = side_leds; remaining != 0; remaining--) {
    error = move_one(&pos_x, &pos_y, direction);
    centerX = (pos_x-mid)*delta;
    centerY = (pos_y-mid)*delta;
    update_spectrum(thumbnails[pos_x*side+pos_y],
                    th_dim, radius, forward, backward, itf, tf);
    if (copy_disk_with_offset(tf, out, th_dim, radius, centerX, centerY))
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
int swarm(fftw_complex **thumbnails, int th_dim, int out_dim, int delta,
          int radius, int jorga, fftw_complex *out) {
  /** @todo check these formula */
  /* check if out is big enough */
  if (jorga*delta + th_dim/2 < out_dim/2)
    return 1;
  /* check if the circle are correctly entangled */
  if (delta > 2*radius)
    return 1;
  if (delta < 1.4*radius)
    return 1;

  fftw_complex *itf;
  fftw_complex *tf;
  fftw_plan forward;
  fftw_plan backward;

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

    /* special: no adjacent circle */
    update_spectrum(thumbnails[pos_x*side+pos_y],
                    th_dim, radius, forward, backward, itf, tf);

    /*
     * one whorl correspond of a move going from one corner
     * to the same but farther from the center by going in spiral
     * example : from [-2,2] to [-3,3]
     *
     * a whorl correspond to four streaks
     */
    for (int whorl = 1; whorl <= jorga; whorl++) {
      /* side leds */
      move_streak(thumbnails, itf, tf, out, forward, backward,
                  th_dim, radius, delta, side, pos_x, pos_y,
                  side_leds, direction);

      /* special: corner led */
      update_spectrum(thumbnails[pos_x*side+pos_y],
                      th_dim, radius, forward, backward, itf, tf);

      /* direction change: clockwise route */
      direction = (direction+1)%4;

      /* side leds */
      move_streak(thumbnails, itf, tf, out, forward, backward,
                  th_dim, radius, delta, side, pos_x, pos_y,
                  side_leds, direction);

      /* special: corner led */
      update_spectrum(thumbnails[pos_x*side+pos_y],
                      th_dim, radius, forward, backward, itf, tf);

      direction = (direction+1)%4;
      side_leds++;
    }

    /* at this point side_leds = side */
    /* we just need to finish the spiral */

    move_streak(thumbnails, itf, tf, out, forward, backward,
                th_dim, radius, delta, side, pos_x, pos_y,
                side_leds, direction);

    /* there is no corner led here */
    /* the spiral lap is done at this point */
  }

  /** @todo free itf tf forward backward ?? */

  return 0;
}
