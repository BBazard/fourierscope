/* Copyright [2016] <Alexis Lescouet, Benoît Bazard> */
/**
 *  @file
 *
 *  Swarm functions test file
 *  @todo why swarm
 *
 */

#include "include/matrix.h"
#include "include/tiffio.h"
#include "gtest/gtest.h"

/**
 *  @brief tiffio.c file test suite
 *
 */
class fftw_suite : public ::testing::Test {
 protected:
  /** Path of an existing tiff image */
  const char *input = "../images/square.tiff";

  /** Existing path for an output */
  const char *before = "../images/fftw_before.tiff";
  const char *ft = "../images/fftw_ft.tiff";
  const char *ift = "../images/fftw_ift.tiff";

  double *matrix; /**< The matrix storing the image when using doubles */
  fftw_complex *comp_mat; /**< The matrix storing the complex image */
  fftw_complex *comp_mat2; /**< The matrix storing the complex image */
  fftw_plan forward;
  fftw_plan backward;

  uint32 diml; /**< Number of lines in the image */
  uint32 dimw; /**< Number of columns in the image */

  void **args;

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
    args = (void**) malloc(5*sizeof(void*));
  }

  /**
   *  @brief teardown function for tiffio_suite tests
   *
   *  Free all memory allocations (here for conventions
   *  but currently doing nothing)
   *
   */
  virtual void TearDown() {
    free(matrix);
    fftw_free(comp_mat);
    fftw_destroy_plan(forward);
    fftw_destroy_plan(backward);
  }

  static double div_dim(double d, void **args) {
    int dimw = *((int*) args[0]);
    int diml = *((int*) args[1]);
    return d/(double) (dimw*diml);
  }

  static double identity(double d, void **args) {return d;}
};

TEST_F(fftw_suite, fftw_execute) {
  ASSERT_EQ(0, tiff_getsize(input, &diml, &dimw));
  args[0] = &dimw;
  args[1] = &diml;

  matrix = (double *) malloc(diml * dimw * sizeof(double));
  comp_mat = (fftw_complex *) fftw_malloc(diml * dimw * sizeof(fftw_complex));
  comp_mat2 = (fftw_complex *) fftw_malloc(diml * dimw * sizeof(fftw_complex));
  forward = fftw_plan_dft_2d(diml, dimw, comp_mat, comp_mat2,
                             FFTW_FORWARD, FFTW_ESTIMATE);
  backward = fftw_plan_dft_2d(diml, dimw, comp_mat, comp_mat2,
                              FFTW_FORWARD, FFTW_ESTIMATE);

  ASSERT_EQ(0, tiff_tomatrix(input, matrix, diml, dimw));
  for (int i = 0; i < diml; i++)
    for (int j = 0; j < dimw; j++)
      (comp_mat[i*dimw+j])[0] = matrix[i*dimw+j];

  for (int i = 0; i < diml; i++)
    for (int j = 0; j < dimw; j++) {
      get_modarg(comp_mat[i*dimw+j], comp_mat2[i*dimw+j]);
      matrix[i*dimw+j] = (comp_mat2[i*dimw+j])[0];
    }
  tiff_frommatrix(before, matrix, diml, dimw);

  fftw_execute(forward);
  matrix_operation(comp_mat2, comp_mat, dimw, div_dim, args);

  for (int i = 0; i < diml; i++)
    for (int j = 0; j < dimw; j++) {
      get_modarg(comp_mat[i*dimw+j], comp_mat2[i*dimw+j]);
      matrix[i*dimw+j] = (comp_mat2[i*dimw+j])[0];
    }
  tiff_frommatrix(ft, matrix, diml, dimw);

  fftw_execute(backward);
  matrix_operation(comp_mat2, comp_mat, dimw, identity, NULL);

  for (int i = 0; i < diml; i++) {
    for (int j = 0; j < dimw; j++) {
      get_modarg(comp_mat[i*dimw+j], comp_mat2[i*dimw+j]);
      matrix[i*dimw+j] = (comp_mat2[i*dimw+j])[0];
      // printf("%f\n", matrix[i*dimw+j]);
    }
    // printf("\n");
  }

  tiff_frommatrix(ift, matrix, diml, dimw);
}
