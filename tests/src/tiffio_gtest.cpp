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
  const char *name = "/home/al/fourierscope/images/grayscale.tiff";
  const char *name2 = "/home/al/fourierscope/images/test_write.tiff";
  double *matrix;
  uint32 diml, dimw;

  virtual void SetUp() {
    diml = 0;
    dimw = 0;
  }

  virtual void TearDown() {
  }
};

TEST_F(tiffio_suite, tiff_getsize) {
  tiff_getsize(name, &diml, &dimw);

  ASSERT_EQ(800, diml);
  ASSERT_EQ(600, dimw);
}

TEST_F(tiffio_suite, tiff_tomatrix) {
  tiff_getsize(name, &diml, &dimw);

  matrix = (double*) malloc(diml * dimw * sizeof(double));

  tiff_tomatrix(name, matrix, diml, dimw);
  // for (int i=0; i < diml; i++) {
  //   for (int j=0; j < dimw; j++) {
  //     printf("%3.0f ", matrix[i*dimw+j]);
  //   }
  //   printf("\n");
  // }

  free(matrix);
}

TEST_F(tiffio_suite, tiff_frommatrix) {
  tiff_getsize(name, &diml, &dimw);

  matrix = (double*) malloc(diml * dimw * sizeof(double));

  tiff_tomatrix(name, matrix, diml, dimw);
  // for (int i=0; i < diml; i++) {
  //   for (int j=0; j < dimw; j++) {
  //     printf("%3.0f ", matrix[i*dimw+j]);
  //   }
  //   printf("\n");
  // }

  for (int i=0; i < diml; i++) {
    for (int j=0; j < dimw/2; j++) {
      matrix[i*dimw+j] = 120;
    }
  }

  tiff_frommatrix(name2, matrix, diml, dimw);

  free(matrix);
}
