/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Matrix functions test file
 *
 */

#include "include/matrix.h"
#include "gtest/gtest.h"

/**
 *  @brief matrix.c file test suite
 *
 */
class matrix_suite : public ::testing::Test {
 protected:
  int dim; /**< The dimension of the matrix used in the tests */

  fftw_complex *a; /**< A first fftw_complex matrix */
  fftw_complex *b; /**< A second fftw_complex matrix */
  fftw_complex c; /**< A fftw_complex */
  fftw_complex d; /**< Another fftw_complex */

  double *mod; /**< A double matrix used to test the realpart function */

  /** An array of pointers to parameters for matrix_operation function */
  void **args;

  /**
   *  @brief setup function for matrix_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the matrix_suite.
   *
   */
  virtual void SetUp() {
    dim = 10;
    a = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    b = (fftw_complex*) fftw_malloc(dim * dim * sizeof(fftw_complex));
    mod = (double *) malloc(dim * dim * sizeof(double));
    for (int i=0; i < dim*dim; i++) {
      (a[i])[0] = i;
      (a[i])[1] = -i;
    }
    matrix_init(dim, b, 0);

    args = (void**) malloc(2*sizeof(void*));
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
    free(args);
  }

  /**
   *  @brief a random linear function used in matrix_operation test
   *
   */
  static double fun(double d, void **args) {return 2*d+1;}

  /**
   *  @brief a more complicated function used in matrix_operation
   *
   *  This one function uses the array of pointers to parameters
   *
   */
  static double complicated_fun(double d, void **args) {
    return d/(double) *((int*) args[0]);
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
 *  @brief matrix_operation function test
 *
 *  Test if the matrix_operation function applied to the matrix a
 *  stored the good result in the matrix b
 *
 */
TEST_F(matrix_suite, matrix_operation_test_no_args) {
  matrix_operation(a, b, dim, fun, NULL);

  for (int i=0; i < dim*dim; i++) {
    ASSERT_EQ((b[i])[0], 2*(a[i])[0]+1);
    ASSERT_EQ((b[i])[1], 2*(a[i])[1]+1);
  }
}

/**
 *  @brief matrix_operation function test
 *
 *  A more complicated test involving the args array of parameters
 *
 */
TEST_F(matrix_suite, matrix_operation_test_with_args) {
  args[0] = &dim;
  args[1] = NULL;
  matrix_operation(a, b, dim, complicated_fun, args);

  for (int i=0; i < dim*dim; i++) {
    ASSERT_EQ((b[i])[0], (a[i])[0]/dim);
    ASSERT_EQ((b[i])[1], (a[i])[1]/dim);
  }
}

/**
 *  @brief get_modarg function test
 *
 *  Test if the get_modarg applied on a complex equal to 0
 *  get 0 for both mod and arg
 *
 */
TEST_F(matrix_suite, get_modarg_test_zero) {
  c[0] = 0;
  c[1] = 0;

  get_modarg(c, d);
  ASSERT_DOUBLE_EQ(0, d[0]);
  ASSERT_DOUBLE_EQ(0, d[1]);
}

/**
 *  @brief get_modarg function test
 *
 *  Test the get_modarg function with a hard-coded value
 *
 */
TEST_F(matrix_suite, get_modarg_test_values) {
  c[0] = 3;
  c[1] = 5;

  get_modarg(c, d);
  EXPECT_DOUBLE_EQ(sqrt(34), d[0]);
  EXPECT_DOUBLE_EQ((M_PI/2.)-atan(3.0/5.0), d[1]);
}

/**
 *  @brief get_algebraic function test
 *
 *  Test if a complex with mod and arg both equal to 0
 *  get an algebraic form equal to 0
 *
 */
TEST_F(matrix_suite, get_algebraic_test_zero) {
  c[0] = 0;
  c[1] = 0;

  get_algebraic(c, d);
  ASSERT_DOUBLE_EQ(0, d[0]);
  ASSERT_DOUBLE_EQ(0, d[1]);
}

/**
 *  @brief get_algebraic function test
 *
 *  Test the get_algebraic function with a hard-coded value
 *
 */
TEST_F(matrix_suite, get_algebraic_test_values) {
  c[0] = sqrt(34);
  c[1] = (M_PI/2.)-atan(3.0/5.0);

  get_algebraic(c, d);
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

