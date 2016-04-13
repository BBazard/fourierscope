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
 *  @brief Decrease a int in absolute
 *  @todo use this function to refactor move_one
 *
 *  The sign stay the same and the absolute value
 *  decrease by one
 */
int abs_decrease(int a) {
  if (a > 0)
    return a-1;

  else if (a < 0)
    return a+1;

  else
    return 0;
}

/**
 *  @brief move the index use for thumbnails
 *
 *  index_x and index_y are increased of decreased of
 *  one in the direction of direction_x and direction_y.
 *  direction_x and direction_y have their absolute
 *  value decreased by one.
 *  One and only one direction must be null.
 *
 */
int move_one(int* index_x, int* index_y, int* direction_x, int* direction_y) {
  int error = 0;

  if (*direction_x == 0) {
    if (*direction_y > 0) {
      (*index_y)++;
      (*direction_y)--;
    } else if (*direction_y < 0) {
      (*index_y)--;
      (*direction_y)++;
    } else {
      error = 1;
    }
  } else if (*direction_y == 0) {
    if (*direction_x > 0) {
      (*index_x)++;
      (*direction_x)--;
    } else if (*direction_x < 0) {
      (*index_x)--;
      (*direction_x)++;
    } else {
      error = 1;
    }
  } else {
    error = 1;
  }

  return error;
}

/*
 *  @brief update leds in a row
 *
 *  max(abs(X), abs(Y) leds are exploited
 *
 */
int move_streak(fftw_complex **thumbnails, fftw_complex *itf,
                fftw_complex *tf, fftw_plan forward, fftw_plan backward,
                int th_dim, int radius,
                int side, int pos_x, int pos_y, int X, int Y) {
  while (X != 0 || Y != 0) {
    update_spectrum(thumbnails[pos_x*side+pos_y],
                    th_dim, radius, forward, backward, itf, tf);
    /* update out with tf */
    move_one(&pos_x, &pos_y, &X, &Y);
  }
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
 *  @return 1 If memory allocation failed
 *  @return 0 0therwise
 */
int swarm(fftw_complex **thumbnails, int th_dim, int out_dim, int delta,
          int radius, int jorga, fftw_complex *out) {
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

  /*
   * Spiral loop
   *
   * Side is an odd number (2*jorga+1)
   *
   * In one "lap" all the thumbnails are exploited
   * In one "streak" all the leds in one side (not a full side) are exploited
   * In one "whorl" all the leds in half a square are exploited
   *
   * One whorl is 4 streak
   * One lap is (side-1)/2  whorls plus one special streak
   *
   *
   */

  /* the coordinates of the thumbnail in the center
   * are [mid;mid] */
  const int mid = jorga+1;

  /* the side of thumbnails */
  const int side = 2*jorga+1;

  const int lap_nbr = 2;

  for (int lap = 0; lap < lap_nbr; lap++) {
    /* the difference between the position before and after a streak */
    int X = 0;
    int Y = 0;

    /* the number of leds exploited (update_spectrum) in the same streak  */
    int intensity = 0;

    /* index in thumbnails */
    int pos_x = mid;
    int pos_y = mid;

    /*
     * one whorl correspond of a move going from one corner
     * to the same but farther from the center by going in spiral
     * example : from [-2,2] to [-3,3]
     *
     * a whorl correspond to four streaks
     *
     */
    for (int whorl = 1; whorl <= jorga; whorl++) {
      intensity = 2*whorl-1;

      /* down */
      X = 0;
      Y = -intensity;
      move_streak(thumbnails, itf, tf, forward, backward,
                  th_dim, radius, side, pos_x, pos_y, X, Y);

      /* right */
      X = +intensity;
      Y = 0;
      move_streak(thumbnails, itf, tf, forward, backward,
                  th_dim, radius, side, pos_x, pos_y, X, Y);

      intensity = 2*whorl;

      /* up */
      X = 0;
      Y = intensity;
      move_streak(thumbnails, itf, tf, forward, backward,
                  th_dim, radius, side, pos_x, pos_y, X, Y);

      /* left */
      X = -intensity;
      Y = 0;
      move_streak(thumbnails, itf, tf, forward, backward,
                  th_dim, radius, side, pos_x, pos_y, X, Y);
    }

    /* we just need to finish the spiral */

    /* down */
    X = 0;
    Y = -side;
      move_streak(thumbnails, itf, tf, forward, backward,
                  th_dim, radius, side, pos_x, pos_y, X, Y);
    /* the spiral lap is done at this point */
  }

  /* @bug the big matrix out is never updated */

  return 0;
}
