/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Swarm functions test file
 *  @todo why swarm
 *
 */

#include "include/swarm.h"
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

  void **args; /**< arguments vector for matrix_operation function */

  /**
   *  The number of thumbnails from the center to the extremities
   *  in the Fourier domain
   */
  int jorga_x, jorga_y;

  /** The distance in pixel between deux thumbnails */
  int delta_x, delta_y;

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

    args = (void**) malloc(5*sizeof(void*));
    args[0] = &toSplitDim;
    args[1] = NULL;

    srand(time(NULL));  // @todo is it fine to do this ?
  }

  /**
   *  @brief teardown function for swarm_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
    free(args);
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
};

/**
 *  @brief fftw_complex_units class
 *
 *  This class should be inherited if fftw_complex allocations are needed
 *
 */
class fftw_complex_units : public virtual swarm_suite {
 protected:
  fftw_complex *toSplit; /**< A fftw_complex matrix to split in thumbnails */
  fftw_complex **thumbnail; /**< @todo what is a thumbnail */
  fftw_complex *thumbnail_buf[2]; /**< buffers same dimensions as a thumbnail */

  /** The fftw_plan in which is executed the fft */
  fftw_plan forward; /**< A fftw_plan for fourier transform */
  fftw_plan backward; /**< A fftw_plan for inverse fourier transform */

  /**
   *  @brief fftw_complex_units SetUp function
   *
   *  Allocate memory for fftw_complex and double matrix structures.
   *
   */
  virtual void SetUp() {
    swarm_suite::SetUp();
    toSplit = (fftw_complex*) fftw_malloc(toSplitDim * toSplitDim *
                                          sizeof(fftw_complex));
    thumbnail_buf[0] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim *
                                                   sizeof(fftw_complex));
    thumbnail_buf[1] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim *
                                                   sizeof(fftw_complex));

    thumbnail = (fftw_complex**) malloc((2*jorga_x+1)*(2*jorga_y+1)*
                                        sizeof(fftw_complex*));
    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      thumbnail[i] = (fftw_complex*) fftw_malloc(thumbnailDim * thumbnailDim *
                                                 sizeof(fftw_complex));

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
   *  @brief fftw_complex_units TearDown function
   *
   *  Free previously allocated memory
   *
   */
  virtual void TearDown() {
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      fftw_free(thumbnail[i]);
    free(thumbnail);

    fftw_free(thumbnail_buf[0]);
    fftw_free(thumbnail_buf[1]);

    fftw_free(toSplit);

    fftw_cleanup();
    swarm_suite::TearDown();
  }
};

class fftw_soft_units : public virtual swarm_suite {
 protected:
  fftw_complex *thumbnail;
  fftw_complex *itf;
  fftw_complex *tf;

  fftw_plan forward;
  fftw_plan backward;

  virtual void SetUp() {
    swarm_suite::SetUp();
    thumbnail = (fftw_complex *) fftw_malloc(thumbnailDim * thumbnailDim *
                                             sizeof(fftw_complex));
    itf = (fftw_complex *) fftw_malloc(thumbnailDim * thumbnailDim *
                                             sizeof(fftw_complex));
    tf = (fftw_complex *) fftw_malloc(thumbnailDim * thumbnailDim *
                                             sizeof(fftw_complex));

    forward = fftw_plan_dft_2d(thumbnailDim, thumbnailDim, itf, tf,
                                FFTW_FORWARD, FFTW_ESTIMATE);
    backward = fftw_plan_dft_2d(thumbnailDim, thumbnailDim, tf, itf,
                                FFTW_BACKWARD, FFTW_ESTIMATE);

    for (int i = 0; i < thumbnailDim*thumbnailDim; i++) {
      (thumbnail[i])[0] = 0;
      (thumbnail[i])[1] = 0;
      (itf[i])[0] = 0;
      (itf[i])[1] = 0;
      (tf[i])[0] = 0;
      (tf[i])[1] = 0;
    }
  }

  virtual void TearDown() {
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);

    fftw_free(tf);
    fftw_free(itf);
    fftw_free(thumbnail);
    fftw_cleanup();
    swarm_suite::TearDown();
  }
};

/**
 *  @brief io_units class
 *
 *  This class should be inherited if there is a need
 *  for images input or output
 *
 */
class io_units : public virtual swarm_suite {
 protected:
  const char *input = "images/square.tiff"; /**< Image input */

  double *io_small; /**< For small dim outputs */
  double *io_big; /**< For big dim outputs */

  /**
   *  @brief io_units SetUp function
   *
   *  Allocate memory for both a small and big input/ouput buffers
   *
   */
  virtual void SetUp() {
    io_small = (double*) malloc(thumbnailDim * thumbnailDim * sizeof(double));
    io_big = (double*) malloc(toSplitDim * toSplitDim * sizeof(double));
  }

  /**
   *  @brief io_units TearDown function
   *
   *  Free previously allocated memory
   *
   */
  virtual void TearDown() {
    free(io_small);
    free(io_big);
  }
};

/**
 *  @brief complex_and_io_units class
 *
 *  Inherits both io_units and fftw_complex_units classes
 *
 */
class complex_and_io_units : public io_units, public fftw_complex_units {
  virtual void SetUp() {
    fftw_complex_units::SetUp();
    io_units::SetUp();
  }

  virtual void TearDown() {
    io_units::TearDown();
    fftw_complex_units::TearDown();
  }
};

/**
 *  @brief soft_and_io_units class
 *
 *  Inherits both io_units and fftw_complex_units classes
 *
 */
class soft_and_io_units : public io_units, public fftw_soft_units {
 protected:
  const char* input = "images/small.tiff";
  const char* output = "build/update_spectrum.tiff";

  virtual void SetUp() {
    fftw_soft_units::SetUp();
    io_units::SetUp();
    tiff_tomatrix(input, io_small, thumbnailDim, thumbnailDim);
    for (int i = 0; i < thumbnailDim*thumbnailDim; i++) {
      (thumbnail[i])[0] = io_small[i];
    }
  }

  virtual void TearDown() {
    io_units::TearDown();
    fftw_soft_units::TearDown();
  }
};


TEST_F(soft_and_io_units, update_spectrum) {
  update_spectrum(thumbnail, thumbnailDim, radius, forward, backward,
                  itf, tf);
  for (int i = 0; i < thumbnailDim*thumbnailDim; i++) {
    get_modarg(tf[i], tf[i]);
    io_small[i] = (tf[i])[0];
  }

  tiff_frommatrix(output, io_small, thumbnailDim, thumbnailDim);
}

/**
 *  @brief swarming
 *
 *  Take a image and create multiple thumbnails
 *  from this image
 *
 */
TEST_F(complex_and_io_units, swarm) {
  ASSERT_EQ(1, (jorga_x*delta_x+thumbnailDim < toSplitDim));
  ASSERT_EQ(1, (jorga_y*delta_y+thumbnailDim < toSplitDim));

  ASSERT_EQ(0, tiff_tomatrix(input, io_big, toSplitDim, toSplitDim));
  for (int i = 0; i < toSplitDim * toSplitDim; i++)
    (toSplit[i])[0] = io_big[i];

  /* fourier transform toSplit */
  fftw_execute(forward);
  matrix_operation(toSplit, toSplit, toSplitDim, div_dim, args);

  matrix_realpart(toSplitDim, toSplit, io_big);
  tiff_frommatrix("build/test.tiff", io_big, toSplitDim, toSplitDim);

  char name[14] = "build/xx.tiff";

  for (int i = -jorga_x; i <= jorga_x; i++)
    for (int j = -jorga_y; j <= jorga_y; j++) {
      int offX = i*delta_x;
      int offY = j*delta_y;

      /* extract the thumbnail */
      /** @bug mid is not in mid but in left-high corner */
      ASSERT_EQ(0, matrix_extract(thumbnailDim, toSplitDim, thumbnail_buf[0],
                                  toSplit, offX, offY));

      /* set zero outside the disk (default value) */
      for (int k = 0; k < thumbnailDim*thumbnailDim; k++) {
        ((thumbnail_buf[1])[k])[0] = 0;
        ((thumbnail_buf[1])[k])[1] = 0;
      }

      ASSERT_EQ(0, cut_disk(thumbnail_buf[0], thumbnail_buf[1],
                            thumbnailDim, radius));

      /* invert fourier transform */
      fftw_execute(backward);

      matrix_operation(thumbnail_buf[0],
                       thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)],
                       thumbnailDim, identity, args);

      /* get module */
      for (int k = 0; k < thumbnailDim * thumbnailDim; k++)
        /** @bug good type but not good place*/
        get_modarg((thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[k],
                   (thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)])[k]);
      name[6] = (i + jorga_x) + '0';
      name[7] = (j + jorga_y) + '0';
      matrix_realpart(thumbnailDim,
                      thumbnail[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)],
                      io_small);
      tiff_frommatrix(name, io_small, thumbnailDim, thumbnailDim);
    }
}
