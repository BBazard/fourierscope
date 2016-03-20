/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  TIFF Import and Export tests file
 *
 *
 */

#include "include/tiffio.h"
#include "gtest/gtest.h"

/**
 *  @brief tiffio.c file test suite
 *
 */
class tiffio_suite : public ::testing::Test {
 protected:
  /** Path of an existing tiff image */
  const char *true_input = "../images/grayscale.tiff";
  /** Path of an non-existing tiff image */
  const char *false_input = "../images/false.tiff";

  /** Existing path for an output */
  const char *true_output = "../images/test_write.tiff";

  double *matrix; /**< The matrix storing the image when using doubles */
  uint32 diml; /**< Number of lines in the image */
  uint32 dimw; /**< Number of columns in the image */

  /**
   *  @brief setup function for tiffio_suite tests
   *
   *  It prepares all the memory allocations and initializes the members
   *  of the tiffio_suite.
   *
   */
  virtual void SetUp() {
    diml = 0;
    dimw = 0;
  }

  /**
   *  @brief teardown function for tiffio_suite tests
   *
   *  Free all memory allocations (here for conventions
   *  but currently doing nothing)
   *
   */
  virtual void TearDown() {
  }
};

/**
 *  @brief tiff_getsize function test
 *
 *  Test if the getsize function used on an existing image
 *  returns 0 and the good dimensions
 *
 */
TEST_F(tiffio_suite, tiff_getsize_true) {
  ASSERT_EQ(0, tiff_getsize(true_input, &diml, &dimw));

  ASSERT_EQ(800, diml);
  ASSERT_EQ(600, dimw);
}

/**
 *  @brief tiff_getsize function test
 *
 *  Test if the getsize function used on a non-existing image
 *  returns 1 and exit
 *
 */
TEST_F(tiffio_suite, tiff_getsize_false) {
  ASSERT_EQ(1, tiff_getsize(false_input, &diml, &dimw));
}

/**
 *  @brief tiff_tomatrix function test
 *
 *  Test the tiff_tomatrix import function.
 *
 *  @todo Write a better test
 *
 */
TEST_F(tiffio_suite, tiff_tomatrix) {
  tiff_getsize(true_input, &diml, &dimw);

  matrix = (double*) malloc(diml * dimw * sizeof(double));

  ASSERT_EQ(0, tiff_tomatrix(true_input, matrix, diml, dimw));

  free(matrix);
}

/**
 *  @brief tiff_frommatrix function test
 *
 *  Test the tiff_frommatrix export function by importing
 *  an existing one, then changing half of it in the matrix
 *  and finally exporting it in a new image.
 *
 *  The tester must check the new image.
 *
 *  @todo write a better test
 *
 */
TEST_F(tiffio_suite, tiff_frommatrix) {
  tiff_getsize(true_input, &diml, &dimw);

  matrix = (double*) malloc(diml * dimw * sizeof(double));

  tiff_tomatrix(true_input, matrix, diml, dimw);

  for (int i=0; i < diml; i++)
    for (int j=0; j < dimw/2; j++)
      matrix[i*dimw+j] = 120;

  tiff_frommatrix(true_output, matrix, diml, dimw);

  free(matrix);
}
