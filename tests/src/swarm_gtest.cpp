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
  fftw_plan backward;

  /** The matrix to be printed in external images */
  double *to_print;
  double *import;

  void **args;

  /**
   *  @brief setup function for swarm_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the swarm_suite.
   *
   */
  virtual void SetUp() {
    toSplitDim = 1000;
    jorga_x = 3;
    jorga_y = 3;
    thumbnailDim = 100;
    radius = 10;
    delta_x = 20, delta_y = 20;

    toSplit = (fftw_complex*) fftw_malloc(toSplitDim * toSplitDim * sizeof(fftw_complex));
    thumbnail_buf[0] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));
    thumbnail_buf[1] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));

    thumbnail = (fftw_complex**) malloc((2*jorga_x+1)*(2*jorga_y+1)*sizeof(fftw_complex*));
    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
        thumbnail[i] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim * sizeof(fftw_complex));

    to_print = (double*) malloc(thumbnailDim * thumbnailDim * sizeof(double));
    import = (double*) malloc(toSplitDim * toSplitDim * sizeof(double));

    args = (void**) malloc(5*sizeof(void*));
    args[0] = &toSplitDim;
    args[1] = NULL;

    srand(time(NULL));  // @todo is it fine to do this ?

    forward = fftw_plan_dft_2d(toSplitDim, toSplitDim, toSplit, toSplit,
                                       FFTW_FORWARD, FFTW_ESTIMATE);
    backward = fftw_plan_dft_2d(thumbnailDim, thumbnailDim, thumbnail_buf[1],
                                thumbnail_buf[0], FFTW_BACKWARD, FFTW_ESTIMATE);

    for (int i=0; i < toSplitDim*toSplitDim; i++) {
      (toSplit[i])[0] = 0;
      (toSplit[i])[1] = 0;
    }

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      for (int j = 0; j < thumbnailDim*thumbnailDim ; j++) {
        ((thumbnail[i])[j])[0] = 0.0;
        ((thumbnail[i])[j])[1] = 0.0;
      }
  }

  /**
   *  @brief teardown function for swarm_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);

    fftw_free(thumbnail_buf[0]);
    fftw_free(thumbnail_buf[1]);

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      fftw_free(thumbnail[i]);
    free(thumbnail);
    free(to_print);
    fftw_free(toSplit);
  }
  /**
   *  @brief A function to divide by dim used in matrix_operation
   *
   *  args[0] MUST contains the pointer to the dimension of the matrix
   *
   */
  static double div_dim(double d, void **args) {
    int dim = *((int*) args[0]);
    return d/(double) (dim);
  }

  static double by_log(double d, void **args) {return log(d);}
};

/**
 *  @brief swarming
 *
 *  Take a image and create multiple thumbnails
 *  from this image
 *
 */
TEST_F(swarm_suite, swarm_test) {
  ASSERT_EQ(1, (jorga_x*delta_x+thumbnailDim < toSplitDim));
  ASSERT_EQ(1, (jorga_y*delta_y+thumbnailDim < toSplitDim));

  ASSERT_EQ(0, tiff_tomatrix("images/square.tiff", import, toSplitDim, toSplitDim));
  for (int i = 0; i < toSplitDim * toSplitDim; i++)
    (toSplit[i])[0] = import[i];

  /* fourier transform toSplit */
  fftw_execute(forward);
  matrix_operation(toSplit, toSplit, toSplitDim, div_dim, args);

  double *printable = (double*) malloc(toSplitDim * toSplitDim * sizeof(double));
  // for (int p = 0; p < toSplitDim * toSplitDim; p++) {
  //   // printf("alg: %f;%f\n", (toSplit[p])[0], (toSplit[p])[1]);
  //   get_modarg(toSplit[p], toSplit[p]);
  //   // printf("mod: %f;%f\n", (toSplit[p])[0], (toSplit[p])[1]);
  // }
  matrix_realpart(toSplitDim, toSplit, printable);
  // matrix_operation(toSplit, toSplit, toSplitDim, by_log, NULL);
  // for (int p = 0; p < toSplitDim * toSplitDim; p++)
  //   printf("%f\n", printable[p]);
  tiff_frommatrix("build/test.tiff", printable, toSplitDim, toSplitDim);

  int mid = toSplitDim/2 + toSplitDim%2;
  char name[14] = "build/xx.tiff";

  for (int i = -jorga_x; i < jorga_x; i++)
    for (int j = -jorga_y; j < jorga_y; j++) {
      int offX = mid + i*delta_x;
      int offY = mid + j*delta_y;

      /* extract the thumbnail */
      ASSERT_EQ(0, matrix_extract(thumbnailDim, toSplitDim, thumbnail_buf[0], toSplit, offX, offY)); /**< @bug mid is not in mid but in left-high corner */
      // matrix_realpart(thumbnailDim, thumbnail_buf[0], to_print);
      // tiff_frommatrix(name, to_print, thumbnailDim, thumbnailDim);

      /* set zero outside the disk (default value) */
      for (int k = 0; k < thumbnailDim*thumbnailDim; k++) {
        ((thumbnail_buf[1])[k])[0] = 0;
        ((thumbnail_buf[1])[k])[1] = 0;
      }

      ASSERT_EQ(0, cut_disk(thumbnail_buf[0], thumbnail_buf[1], thumbnailDim, radius));
      // matrix_realpart(thumbnailDim, thumbnail_buf[1], to_print);
      // tiff_frommatrix(name, to_print, thumbnailDim, thumbnailDim);
      // invert fourier transform
      fftw_execute(backward);

      matrix_operation(thumbnail_buf[0],
                       thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)],
                       thumbnailDim, identity, args);

      /* get module */
      for (int k = 0; k < thumbnailDim * thumbnailDim; k++)
        get_modarg((thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[k], (thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[k]); /**< @bug good type but not good place*/
      name[6] = (i + jorga_x) + '0';
      name[7] = (j + jorga_y) + '0';
      matrix_realpart(thumbnailDim, thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)], to_print);
      tiff_frommatrix(name, to_print, thumbnailDim, thumbnailDim);
    }
}
