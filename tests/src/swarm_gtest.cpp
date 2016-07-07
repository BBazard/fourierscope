/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Swarm functions test file
 *
 */

#include "include/swarm.h"
#include <tuple>
#include "gtest/gtest.h"

/**
 *  @brief swarm.c file test suite
 *
 */
class swarm_suite : public ::testing::
  TestWithParam<std::tuple<int, int, int, int> > {
 protected:
  int out_dim; /**< The dimension of the output used in the tests */
  int th_dim; /**< The dimension of the thumbnails created in the tests */
  int radius; /**< The radius of the circle centered on the zero frequency */

  /**@{*/
  /**
   *  The number of thumbnails from the center to the extremities
   *  in the Fourier domain
   */
  int jorga_x, jorga_y;
  /**@}*/

  /**@{*/
  /** The distance in pixel between deux thumbnails */
  int delta_x, delta_y;
  /**@}*/

  /** The number of iteration in the swarm execution */
  int lap_nbr;

  /**
   *  @brief setup function for swarm_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the swarm_suite.
   *
   */
  virtual void SetUp() {
    out_dim = 1000;
    jorga_x = jorga_y = std::get<0>(GetParam());
    th_dim = 100;
    radius = std::get<2>(GetParam());
    delta_x = delta_y = std::get<1>(GetParam()); //0.3*radius
    lap_nbr = std::get<3>(GetParam());

    srand(time(NULL));  // @todo is it fine to do this ?
  }

  /**
   *  @brief teardown function for swarm_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
  }
};

/**
 *  @brief fftw_complex_units class
 *
 *  This class should be inherited if fftw_complex allocations are needed
 *
 */
class fftw_complex_units : public swarm_suite {
 protected:
  fftw_complex *out; /**< A fftw_complex matrix the size of the output */
  double **thumbnails; /**< A matrix of inputs thumbnails */
  fftw_complex *thumbnail_buf[2]; /**< buffers same dimensions as a thumbnail */
  const char input[60] = "images/square.tiff"; /**< Image input */
  char *name; /**< Name for output (changing with tiff_getname) */

  double *out_io; /**< For big dim outputs */

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

    free(out_io);
    free(name);

    for (int i = 0; i < (2*jorga_x+1)*(2*jorga_y+1); i++)
      free(thumbnails[i]);
    free(thumbnails);

    fftw_free(thumbnail_buf[0]);
    fftw_free(thumbnail_buf[1]);

    fftw_free(out);
    fftw_cleanup_threads();
    swarm_suite::TearDown();
  }

  /**
   *  @brief Function to generate thumbnails
   *
   */
  void thumbs_gen(bool output) {
    ASSERT_EQ(1, (jorga_x*delta_x+th_dim < out_dim));
    ASSERT_EQ(1, (jorga_y*delta_y+th_dim < out_dim));

    ASSERT_EQ(0, tiff_tomatrix(input, out_io, out_dim, out_dim));
    for (int i = 0; i < out_dim * out_dim; i++)
      (out[i])[0] = out_io[i];

    /* fourier transform out */
    fftw_execute(forward);
    div_dim(out, out, out_dim);

    for (int i = -jorga_x; i <= jorga_x; i++)
      for (int j = -jorga_y; j <= jorga_y; j++) {
        int offX = i*delta_x;
        int offY = j*delta_y;

        /* extract the thumbnail */
        /** @bug mid is not in mid but in left-high corner */
        ASSERT_EQ(0, matrix_extract(th_dim, out_dim, thumbnail_buf[0],
                                    out, offX, offY));

        /* set zero outside the disk (default value) */
        for (int k = 0; k < th_dim*th_dim; k++) {
          ((thumbnail_buf[1])[k])[0] = 0;
          ((thumbnail_buf[1])[k])[1] = 0;
        }

        ASSERT_EQ(0, copy_disk(thumbnail_buf[0], thumbnail_buf[1],
                               th_dim, radius));

        /* invert fourier transform */
        fftw_execute(backward);
        div_dim(thumbnail_buf[0], thumbnail_buf[0], th_dim);

        /* get module */
        for (int k = 0; k < th_dim * th_dim; k++)
          alg2exp(thumbnail_buf[0][k], thumbnail_buf[0][k]);
        matrix_realpart(th_dim,
                        thumbnail_buf[0],
                        thumbnails[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)]);
        if (output)
          tiff_frommatrix(tiff_getname((i+jorga_x), (j+jorga_y), name),
                          thumbnails[(i+jorga_x)*(2*jorga_y+1)+(j+jorga_y)],
                          th_dim, th_dim);
      }
  }
};

/**
 *  @brief swarming
 *
 *  Take an image and create multiple thumbnails
 *  from this image
 *
 */
TEST_P(fftw_complex_units, thumbs_gen) {
  fftw_complex_units::thumbs_gen(true);
}

/**
 *  @cond DEV
 *  @brief Instantiate the generation of some thumbnails with origin values
 *
 *  The values are:
 *  * 3 for jorga_x and jorga_y
 *  * 50 for delta_x and delta_y
 *  * 40 for radius
 *
 *  The last value is not used but must still be provided.
 *
 */
INSTANTIATE_TEST_CASE_P(swarm_gen, fftw_complex_units,
                        ::testing::Combine(::testing::Values(3),
                                           ::testing::Values(50),
                                           ::testing::Values(40),
                                           ::testing::Values(2)));
/** @endcond */

/**
 *  @brief Class for swarm testing
 *
 */
class swarm_unit : public fftw_complex_units {
 protected:
  int name_size; /**< The size of the output name */
  virtual void SetUp() {
    fftw_complex_units::SetUp();
    fftw_complex_units::thumbs_gen(false);
    name_size = strlen("build/swarm_with_jnn_dnn_rnn.tiff")+1;
    free(name);
    name = (char*) malloc(sizeof(char)*name_size);
    backward = fftw_plan_dft_2d(out_dim, out_dim, out, out,
                                FFTW_BACKWARD, FFTW_ESTIMATE);

    for (int i = 0; i < out_dim * out_dim; i++)
      (out[i])[0] = (out[i])[1] = 0;
  }

  virtual void TearDown() {
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
    fftw_complex_units::TearDown();
  }
};

/**
 *  @brief swarm testcase
 *
 *  Following are the different tests cases for swarm.
 *  Some of them are redundant.
 *  In particular, don't execute all the tests plus full, because
 *  full includes all the others.
 *
 */
TEST_P(swarm_unit, swarm) {
  EXPECT_EQ(0, swarm(thumbnails, th_dim, out_dim,
                     delta_x, lap_nbr, radius, jorga_x, out));
}

/**
 * @brief Base parameters
 *
 * This test considers only one configuration.
 * @todo Change parameters
 *
 */
INSTANTIATE_TEST_CASE_P(origin, swarm_unit,
                        ::testing::Combine(::testing::Values(3),
                                           ::testing::Values(50),
                                           ::testing::Values(40),
                                           ::testing::Values(2)));
/**
 * @brief Jorga variation on base parameters
 *
 * Variation of the number of thumbnails all others parameters
 * stay identical to the base parameters.
 *
 */
INSTANTIATE_TEST_CASE_P(jorga, swarm_unit,
                        ::testing::Combine(::testing::Values(2, 3, 4, 5,
                                                             6, 10, 15),
                                           ::testing::Values(50),
                                           ::testing::Values(40),
                                           ::testing::Values(2)));
/**
 * @brief Delta variation on base parameters
 *
 * Variation of the distance between the thumbnails
 *
 */
INSTANTIATE_TEST_CASE_P(delta, swarm_unit,
                        ::testing::Combine(::testing::Values(3),
                                           ::testing::Values(30, 40, 50, 60),
                                           ::testing::Values(40),
                                           ::testing::Values(2)));

/**
 * @brief Radius variation on base parameters
 *
 * Variation of the radius of the disk used to computes
 * the result in swarm
 *
 */
INSTANTIATE_TEST_CASE_P(radius, swarm_unit,
                        ::testing::Combine(::testing::Values(3),
                                           ::testing::Values(50),
                                           ::testing::Values(30, 35, 40,
                                                             45, 50),
                                           ::testing::Values(2)));

/**
 * @brief Lap_nbr variation on base parameters
 *
 * Variation of the number of lap (number of iteration) done
 * during the computation
 *
 */
INSTANTIATE_TEST_CASE_P(lap_nbr, swarm_unit,
                        ::testing::Combine(::testing::Values(3),
                                           ::testing::Values(50),
                                           ::testing::Values(40),
                                           ::testing::Values(2, 5, 10)));
/**
 * @brief Full variation.
 *
 * This test groups all the previous variations and should
 * not be used with the others test applying to swarm
 *
 * Use either --gtest_filter="full/" to execute only swarm tests (but all
 * swarm tests) or --gtest_filter="-full/" to exclude this test (and others
 * if needed)
 *
 */
INSTANTIATE_TEST_CASE_P(full, swarm_unit,
                        ::testing::Combine(::testing::Values(2, 3, 4, 5,
                                                             6, 10, 15),
                                           ::testing::Values(30, 40, 50, 60),
                                           ::testing::Values(30, 35, 40,
                                                             45, 50),
                                           ::testing::Values(2, 5, 10)));
