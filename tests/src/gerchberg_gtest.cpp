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
  int threshold;
  
  fftw_complex *input;
  fftw_complex *output;

  fftw_plan forward;

  virtual void SetUp() {
    dim = 10;
    it = 100;
    radius = 5;
    threshold = 0.0000001;

    input = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    output = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    forward = fftw_plan_dft_2d(dim, dim, output, output, FFTW_FORWARD, FFTW_ESTIMATE);

    matrix_random(dim, input, 256);
    matrix_init(dim, output, 0);
  }
  
  virtual void TearDown() {
    fftw_free(input);
    fftw_free(output);
  }
  
  static double by_log(double d) {return log(fabs(d));}
};

TEST_F(gerchberg_suite, gerchberg_test) {
  gerchberg(dim, input, output, it, radius);
  fftw_execute(forward);

  matrix_init(dim, input, 0);
  cut_disk(input, output, dim, radius);

  for (int i=0; i < dim*dim; i++) {
    EXPECT_NEAR(0, (output[i])[0], threshold);
    EXPECT_NEAR(0, (output[i])[1], threshold);
  }
}
