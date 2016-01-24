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

class gerchberg_suite : public ::testing::Test {
 protected:
  int dim;
  int it;
  int radius;
  double threshold;

  char *display;
  double *mod;
  fftw_complex *input;
  fftw_complex *output;
  fftw_complex *fft;

  fftw_plan forward;

  virtual void SetUp() {
    dim = 100;
    it = 100;
    radius = 20;
    threshold = 0.20;

    display = (char*) malloc(dim * dim * sizeof(char));
    mod = (double*) malloc(dim * dim * sizeof(double));
    input = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    output = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    fft =  (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    forward = fftw_plan_dft_2d(dim, dim, output, output,
                               FFTW_FORWARD, FFTW_ESTIMATE);

    matrix_random(dim, input, 256);
    matrix_init(dim, output, 0);

    for (int i=0; i < dim*dim; i++) {
      mod[i] = 0;
      display[i] = 0;
    }
  }

  virtual void TearDown() {
    free(display);
    free(mod);
    fftw_free(input);
    fftw_free(output);
  }

  static double by_log(double d) {return log(fabs(d));}

  static double average(int dim, double *mat) {
    double average = 0;
    for (int i=0; i < dim*dim; i++)
      average += mat[i];
    average /= dim*dim;

    return average;
  }
};

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
