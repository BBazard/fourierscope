/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file contains the functions to make input and output
 *  on tiff files.
 *
 */

#include "include/tiffio.h"

/**
 *  @brief Get the size of a tiff image
 *  @param[in] name The path to the tiff image
 *  @param[out] diml The length of the image
 *  @param[out] dimw The width of the image
 *  @return 1 If the image could not be opened
 *  @return 0 Otherwise
 *
 *  Get the size of the image which path is "name" and store it in
 *  diml and dimw.
 *
 */
int tiff_getsize(const char *name, uint32 *diml, uint32 *dimw) {
  TIFF* tiff = TIFFOpen(name, "r");
  if (tiff) {
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, diml);
    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, dimw);

    TIFFClose(tiff);
    return 0;
  } else {
    return 1;
  }
}

/**
 * @todo doc
 *
 *
 */
int tiff_fullscale(double min, double max, double tosample) {
  return (int) (tosample - min)*255/(max-min);
}

/**
 *  @todo doc
 *
 */
int tiff_maxnormalized(double max, double tosample) {
  return (int) tosample*255/max;
}

/**
 *  @brief Import a extern tiff file
 *  @param[in] name The path to the image to import
 *  @param[out] matrix The matrix where to put the imported image
 *  @param[out] diml The length of the created matrix (line)
 *  @param[out] dimw The width of the created matrix (column)
 *  @return 1 Error while reading from tiff image
 *  @return 0 Otherwise
 *
 *  This function imports an image which path is "name" into a matrix
 *  given the length and the width. By convention, length means number of lines
 *  and width means number of columns.
 *
 */
int tiff_tomatrix(const char *name, double *matrix, uint32 diml, uint32 dimw) {
  tdata_t buf;
  unsigned char *data;

  uint16 bits;
  uint16 sample;

  TIFF* tiff = TIFFOpen(name, "r");
  if (tiff) {
    buf = _TIFFmalloc(TIFFScanlineSize(tiff));
    data = (unsigned char*) malloc(dimw * sizeof(unsigned char));

    for (uint32 row=0; row < diml; row++) {
      if (TIFFReadScanline(tiff, buf, row, 0) == -1) {
        free(data);
        _TIFFfree(buf);
        TIFFClose(tiff);
        return 1;
      }
      memcpy(data, buf, dimw*sizeof(char));
      for (int i=0; i < dimw; i++)
        matrix[row*dimw+i] = (double) data[i];
    }

    free(data);
    _TIFFfree(buf);
    TIFFClose(tiff);
    return 0;
  } else {
    return 1;
  }
}


/**
 *  @brief Export an matrix into a tiff image
 *  @param[in] name The path in which the image is saved
 *  @param[in] matrix The matrix to save
 *  @param[in] diml The number of lines
 *  @param[in] dimw The number of columns
 *  @return 1 If there is a writing error
 *  @return 0 Otherwise
 *
 *  This function exports a matrix into an image which path is "name"
 *  given the two dimensions of the matrix.
 *
 */
int tiff_frommatrix(const char *name, double *matrix,
                    uint32 diml, uint32 dimw) {
  tdata_t buf;
  unsigned char *data;

  TIFF* tiff = TIFFOpen(name, "w");
  if (tiff) {
    buf = _TIFFmalloc(dimw*sizeof(char));
    data = (unsigned char *) malloc(dimw*sizeof(unsigned char));

    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, diml);
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, dimw);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);

    double max = matrix_max(diml, dimw, matrix); /**< @todo change this diml */
    double min = matrix_min(diml, dimw, matrix);

    for (uint32 row=0; row < diml; row++) {
      for (int i=0; i < dimw; i++) {
        data[i] = tiff_fullscale(min, max, matrix[row*dimw+i]);
      }
      memcpy(buf, data, dimw*sizeof(char));

      if (TIFFWriteScanline(tiff, buf, row, 0) == -1) {
        free(data);
        _TIFFfree(buf);
        TIFFClose(tiff);
        return 1;
      }
    }

    free(data);
    _TIFFfree(buf);
    TIFFClose(tiff);

    return 0;
  } else {
    return 1;
  }
}
