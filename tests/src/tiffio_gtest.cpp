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

class tiffio_suite : public ::testing::Test {
 protected:
  const char *true_input = "/home/al/fourierscope/images/grayscale.tiff";
  const char *false_input = "/home/al/fourierscope/images/false.tiff";

  const char *true_output = "/home/al/fourierscope/images/test_write.tiff";

  double *matrix;
  uint32 diml, dimw;

  virtual void SetUp() {
    diml = 0;
    dimw = 0;
  }

  virtual void TearDown() {
  }
};

TEST_F(tiffio_suite, tiff_getsize_true) {
  ASSERT_EQ(0, tiff_getsize(true_input, &diml, &dimw));

  ASSERT_EQ(800, diml);
  ASSERT_EQ(600, dimw);
}

TEST_F(tiffio_suite, tiff_getsize_false) {
  ASSERT_EQ(1, tiff_getsize(false_input, &diml, &dimw));
}


TEST_F(tiffio_suite, tiff_tomatrix) {
  tiff_getsize(true_input, &diml, &dimw);

  matrix = (double*) malloc(diml * dimw * sizeof(double));

  ASSERT_EQ(0, tiff_tomatrix(true_input, matrix, diml, dimw));

  free(matrix);
}

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
