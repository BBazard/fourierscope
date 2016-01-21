/**
 *  @file
 *
 *  Matrix functions test file
 *
 */

#include "include/matrix.h"
#include "gtest/gtest.h"

class matrix_suite : public ::testing::Test {
protected:
  int dim;
  
  fftw_complex *a;
  fftw_complex *b;
  fftw_complex c;
  fftw_complex d;

  virtual void SetUp() {
    dim = 10;
    a = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    b = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    for (int i=0; i < dim*dim; i++) {
      (a[i])[0] = i;
      (a[i])[1] = -i;
    }
    matrix_init(dim, b, 0);
  }

  virtual void TearDown() {
    fftw_free(a);
    fftw_free(b);
  }

  static double fun(double d) {return 2*d+1;}
};

TEST_F(matrix_suite, matrix_operation_test) {
  matrix_operation(a, b, dim, fun);

  for (int i=0; i < dim*dim; i++) {
    ASSERT_EQ((b[i])[0], 2*(a[i])[0]+1);
    ASSERT_EQ((b[i])[1], 2*(a[i])[1]+1);
  }
}

TEST_F(matrix_suite, get_modarg_test_zero) {
  c[0] = 0;
  c[1] = 0;

  get_modarg(c,d);
  ASSERT_DOUBLE_EQ(0, d[0]);
  ASSERT_DOUBLE_EQ(0, d[1]);
}

TEST_F(matrix_suite, get_modarg_test_values) {
  c[0] = 3;
  c[1] = 5;

  get_modarg(c,d);
  EXPECT_DOUBLE_EQ(sqrt(34), d[0]);
  EXPECT_DOUBLE_EQ((M_PI/2.)-atan(3.0/5.0), d[1]);
}

TEST_F(matrix_suite, get_algebraic_test_zero) {
  c[0];
  d[0];

  get_algebraic(c,d);
  //  ASSERT_DOUBLE_EQ(
}
