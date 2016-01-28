/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  Gerchberg-Saxon algorithm test file
 *
 *
 */

#include "include/gerchberg.h"
#include "gtest/gtest.h"

/**
 *  @brief gerchberg.c file test suite
 *
 */
class gerchberg_suite : public ::testing::Test {
 protected:
  int dim; /**< The dimension of the matrix */
  int it; /**< The number of iterations */
  int radius; /**< The radius of the circle */
  double threshold; /**< The threshold for "char" representation */

  char *display; /**< The char matrix used to display human readable */
  double *mod; /**< Contains the module of the result matrix */
  fftw_complex *input; /**< The complexe input of the gerchberg function */
  fftw_complex *output; /**< The complexe output of the gerchberg function */

  fftw_plan forward; /**< The fftw_plan in which is executed the fft */

  /**
   *  @brief setup function for gerchberg_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the gerchberg_suite.
   *
   */
  virtual void SetUp() {
    dim = 100;
    it = 1;
    radius = 20;
    threshold = 0.20;

    display = (char*) malloc(dim * dim * sizeof(char));
    mod = (double*) malloc(dim * dim * sizeof(double));
    input = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    output = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));

    forward = fftw_plan_dft_2d(dim, dim, output, output,
                                         FFTW_FORWARD, FFTW_ESTIMATE);

    matrix_random(dim, input, 256);
    matrix_init(dim, output, 0);

    for (int i=0; i < dim*dim; i++) {
      mod[i] = 0;
      display[i] = 0;
    }
  }


  /**
   *  @brief teardown function for gerchberg_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
    free(display);
    free(mod);
    fftw_destroy_plan(forward);
    fftw_free(input);
    fftw_free(output);
    fftw_cleanup();
  }

  /**
   *  @brief Average of a matrix
   *  @param[in] dim The dimension of the matrix
   *  @param[in] mat The matrix used for computation
   *
   *  Computes the average of a given double matrix
   *
   */
  static double average(int dim, double *mat) {
    double average = 0;
    for (int i=0; i < dim*dim; i++)
      average += mat[i];
    average /= dim*dim;

    return average;
  }
};


/**
 *  @brief gerchberg function test
 *
 *  Applies the gerchberg algorithm to a random matrix
 *  and then displays a char matrix composed of three different
 *  symbols:
 *  - '_' if the result matrix cell is less than (1-thershold)*average
 *  - '*' if the result matrix cell is between  (1-thershold)*average and
 *   (1+thershold)*average
 *  - '#' if the result matrix cell is less than (1+thershold)*average
 *
 */
TEST_F(gerchberg_suite, gerchberg_test) {
  gerchberg(dim, input, output, it, radius);
  fftw_execute(forward);

  matrix_init(dim, input, 0);

  for (int i=0; i < dim*dim; i++)
    get_modarg(output[i], input[i]);

  matrix_realpart(dim, input, mod);

  for (int i=0; i < dim*dim; i++)
    mod[i] = log(mod[i]);

  double mod_average = average(dim, mod);

  for (int i=0; i < dim*dim; i++) {
    if (mod[i] < (1-threshold)*mod_average) {
      display[i] = '_';
    } else if (mod[i] > (1+threshold)*mod_average) {
      display[i] = '#';
    } else {
      display[i] = '*';
    }
  }

  for (int i=0; i < dim; i++) {
    for (int j=0; j < dim; j++)
      printf("%c", display[i*dim+j]);
    printf("\n");
  }

  printf("%lf\n", mod_average);

  // for (int i=0; i < dim*dim; i++) {
  //   EXPECT_NEAR(0, (output[i])[0], threshold);
  //   EXPECT_NEAR(0, (output[i])[1], threshold);
  // }
}
