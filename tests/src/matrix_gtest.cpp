/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Matrix functions test file
 *
 */

#include <limits.h>
#include "include/matrix.h"
#include "gtest/gtest.h"
#include <inttypes.h>
#define __STDC_FORMAT_MACROS

/**
 *  @brief matrix.c file test suite
 *
 */
class benchmark : public ::testing::Test {
 private:
  struct timespec start;
  struct timespec stop;
 protected:
  virtual void SetUp() {
    clock_gettime(CLOCK_MONOTONIC, &start);
  }
  virtual void TearDown() {
    clock_gettime(CLOCK_MONOTONIC, &stop);
    uint64_t res = ((uint64_t)stop.tv_sec - (uint64_t)start.tv_sec)*((uint64_t)1000000000) + ((uint64_t)stop.tv_nsec - (uint64_t)start.tv_nsec);
    printf("time: %" PRIu64 "\n", (res/((uint64_t) 1000)));
  }
};

class matrix_suite : public benchmark {
 protected:
  int dim; /**< The dimension of the matrix used in the tests */

  fftw_complex *a; /**< A first fftw_complex matrix */
  fftw_complex *b; /**< A second fftw_complex matrix */
  fftw_complex c; /**< A fftw_complex */
  fftw_complex d; /**< Another fftw_complex */

  double *mod; /**< A double matrix used to test the realpart function */

  /**
   *  @brief setup function for matrix_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the matrix_suite.
   *
   */
  virtual void SetUp() {
    dim = 10000;
    a = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    b = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    mod = (double *) malloc(dim * dim * sizeof(double));
    for (int i=0; i < dim*dim; i++) {
      (a[i])[0] = i;
      (a[i])[1] = -i;
    }
    matrix_init(dim, b, 0);
  }

  /**
   *  @brief teardown function for matrix_suite tests
   *
   *  Free all memory allocations
   *
   */
  virtual void TearDown() {
    fftw_free(a);
    fftw_free(b);
    free(mod);
  }
};

/**
 *  @brief pi_value test
 *
 *  Asserts that defined pi value is good enough
 *  by comparing it to a hard-coded value
 *
 */
TEST_F(matrix_suite, pi_value) {
  ASSERT_DOUBLE_EQ(3.141592653589793, PI);
}

/**
 *  @brief alg2exp function test
 *
 *  Test if the alg2exp applied on a complex equal to 0
 *  get 0 for both mod and arg
 *
 */
TEST_F(matrix_suite, alg2exp_test_zero) {
  c[0] = 0;
  c[1] = 0;

  alg2exp(c, d);
  ASSERT_DOUBLE_EQ(0, d[0]);
  ASSERT_DOUBLE_EQ(0, d[1]);
}

/**
 *  @brief alg2exp function test
 *
 *  Test the alg2exp function with a hard-coded value
 *
 */
TEST_F(matrix_suite, alg2exp_test_values) {
  c[0] = 3;
  c[1] = 5;

  alg2exp(c, d);
  EXPECT_DOUBLE_EQ(sqrt(34), d[0]);
  EXPECT_DOUBLE_EQ((M_PI/2.)-atan(3.0/5.0), d[1]);
}

/**
 *  @brief exp2alg function test
 *
 *  Test if a complex with mod and arg both equal to 0
 *  get an algebraic form equal to 0
 *
 */
TEST_F(matrix_suite, exp2alg_test_zero) {
  c[0] = 0;
  c[1] = 0;

  exp2alg(c, d);
  ASSERT_DOUBLE_EQ(0, d[0]);
  ASSERT_DOUBLE_EQ(0, d[1]);
}

/**
 *  @brief exp2alg function test
 *
 *  Test the exp2alg function with a hard-coded value
 *
 */
TEST_F(matrix_suite, exp2alg_test_values) {
  c[0] = sqrt(34);
  c[1] = (M_PI/2.)-atan(3.0/5.0);

  exp2alg(c, d);
  EXPECT_DOUBLE_EQ(3, d[0]);
  EXPECT_DOUBLE_EQ(5, d[1]);
}

/**
 *  @brief matrix_realpart function test
 *
 *  Call the matrix_realpart function and then test if the result matrix
 *  contains the realpart for each cell
 *
 */
TEST_F(matrix_suite, matrix_realpart_test) {
  matrix_random(dim, a, 1000);
  matrix_realpart(dim, a, mod);

  for (int i=0; i < dim; i++) {
    for (int j=0; j < dim; j++) {
      ASSERT_DOUBLE_EQ(mod[i*dim+j], (a[i*dim+j])[0]);
    }
  }
}

/**
 *  @brief matrix_cyclic function test
 *
 *  Test with different values:
 *  * 0 < ind < dim
 *  * -dim < ind < 0
 *  * dim < ind
 *  * ind < -dim
 *
 */
TEST_F(matrix_suite, matrix_cyclic) {
  EXPECT_EQ(10, matrix_cyclic(10, 100));
  EXPECT_EQ(10, matrix_cyclic(-90, 100));
  EXPECT_EQ(10, matrix_cyclic(110, 100));
  EXPECT_EQ(10, matrix_cyclic(-190, 100));
}

TEST_F(matrix_suite, show_copy_disk) {
  EXPECT_EQ(0, copy_disk(a, b, dim, 4));

  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++)
      printf("%3.f", (a[i*dim+j])[0]);
    printf("\n");
  }
  printf("\n");
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++)
      printf("%3.f", (b[i*dim+j])[0]);
    printf("\n");
  }
}

TEST_F(matrix_suite, matrix_copy) {
  benchmark::SetUp();
  matrix_copy(a, b, dim);
  benchmark::TearDown();
}

/**
 *  @brief copy_disk_with_offset function test
 *
 *  Call the copy_disk_with_offset function and test when
 *  called with wrong arguments
 *
 */
TEST_F(matrix_suite, copy_disk_with_offset_test) {
  int radius_max = (dim-1)/2;  // radius maximum when no offset
  int mid = dim/2+dim%2;  // coordinates of the center

  int radius = dim/4;
  int centerX = mid, centerY = mid;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));

  radius = 2*radius_max+1;
  ASSERT_EQ(1, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));

  // folding
  radius = dim/4;
  centerX = 2*dim;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
  centerX = -mid;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));

  centerX = mid;
  centerY = 2*dim;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
  centerY = -dim;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));

  // dim = 2*radius_max+1 when no offset
  centerX = mid, centerY = mid;
  radius = (dim-1)/2;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
  radius++;
  ASSERT_EQ(1, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
  // the radius should fold
  // X X X 3 X
  // X X X 3 X
  // X X X 2 X
  // 3 3 2 1 2
  // X X X 2 X
  //
  radius--;
  centerX++;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
  centerX--;
  centerY++;
  ASSERT_EQ(0, copy_disk_with_offset(a, b, dim, radius, centerX, centerY));
}

