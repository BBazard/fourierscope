/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Swarm functions test file
 *  @todo why swarm
 *
 */

#include "include/matrix.h"
#include "include/tiffio.h"
#include "include/gerchberg.h"
#include "gtest/gtest.h"

/**
 *  @brief swarm.c file test suite
 *
 */
class swarm_suite : public ::testing::Test {
 protected:
  int toSplitDim; /**< The dimension of the toSplit matrix used in the tests */
  int thumbnailDim; /**< The dimension of the thumbnails created in the tests */
  int radius;

  /**
   *  The number of thumbnails from the center to the extremities
   *  in the Fourier domain
   */
  int jorga_x, jorga_y;

  /** The distance in pixel between deux thumbnails */
  int delta_x, delta_y;

  fftw_complex *toSplit; /**< A fftw_complex matrix to split in thumbnails */
  fftw_complex **thumbnail; /**< @todo what is a thumbnail */
  fftw_complex *thumbnail_buf[2]; /**< buffers the same dimensions as a thumbnail */

  /** The fftw_plan in which is executed the fft */
  fftw_plan forward;

  /** The matrix to be printed in external images */
  double *to_print;
  
  /**
   *  @brief setup function for swarm_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the swarm_suite.
   *
   */
  virtual void SetUp() {
    toSplitDim = 100;
    radius = 10;
    jorga_x = 3;
    jorga_y = 3;
    thumbnailDim = radius;
    delta_x = 2, delta_y = 2;

    // todo : jorga*delta+smalldim < bigdim

    toSplit = (fftw_complex*) fftw_malloc(toSplitDim* toSplitDim * sizeof(fftw_complex));
    thumbnail_buf[0] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));
    thumbnail_buf[1] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));

    thumbnail = (fftw_complex**) malloc((2*jorga_x+1)*(2*jorga_y+1)*sizeof(fftw_complex*));
    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
        thumbnail[i] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));

    to_print = (double *) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(double));
    
    srand(time(NULL)); // @todo is it fine to do this ?

    for (int i=0; i < toSplitDim*toSplitDim; i++) {
      (toSplit[i])[0] = rand()%1000;
      (toSplit[i])[1] = rand()%1000;
    }

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      for (int j = 0; j < thumbnailDim*thumbnailDim ; j++) {
        ((thumbnail[i])[j])[0] = 0.0;
        ((thumbnail[i])[j])[1] = 0.0;
      }

    forward = fftw_plan_dft_2d(toSplitDim, toSplitDim, toSplit, toSplit,
                                       FFTW_FORWARD, FFTW_ESTIMATE);
  }

  /**
   *  @brief teardown function for swarm_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
    fftw_destroy_plan(forward);

    fftw_free(thumbnail_buf[0]);
    fftw_free(thumbnail_buf[1]);

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      fftw_free(thumbnail[i]);
    free(thumbnail);
    fftw_free(toSplit);
  }
};

/**
 *  @brief swarming
 *
 *  Take a image and create multiple thumbnails
 *  from this image
 *
 */
TEST_F(swarm_suite, swarm_test) {

  /* fourier transform toSplit */
  fftw_execute(forward);

  int mid = toSplitDim/2 + toSplitDim%2;
  char name[7];
  name[0] = 'i';
  name[1] = 'm';
  name[2] = 'a';
  name[3] = 'g';
  name[4] = 'e';
  for (int i = -jorga_x; i < jorga_x; i++)
    for (int j = -jorga_y; j < jorga_y; j++) {
      int offX = mid + i*delta_x;
      int offY = mid + j*delta_y;

      // extract the thumbnail
      matrix_extract(thumbnailDim, toSplitDim, thumbnail_buf[0], toSplit, offX, offY);

      // set zero outside the disk (default value)
      for (int k = 0; k < thumbnailDim*thumbnailDim; k++) {
        ((thumbnail_buf[1])[k])[0] = 0;
        ((thumbnail_buf[1])[k])[1] = 0;
      }
      cut_disk(thumbnail_buf[0], thumbnail_buf[1], thumbnailDim, radius);
      // invert fourier transform
      fftw_plan backward = fftw_plan_dft_2d(thumbnailDim, thumbnailDim,
          thumbnail_buf[1], thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)], FFTW_BACKWARD, FFTW_ESTIMATE);
      fftw_execute(backward);
      fftw_destroy_plan(backward);

      // get module
      get_modarg((thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[0], (thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[0]);
      get_modarg((thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[1], (thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[1]);
      name[5] = i + '0';
      name[6] = j + '0';
      matrix_realpart(thumbnailDim, thumbnail[i*thumbnailDim+j], to_print);
      tiff_frommatrix(name, to_print, thumbnailDim, thumbnailDim);
    }
}
