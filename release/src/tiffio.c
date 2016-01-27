/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  This file contains the function to make input and output
 *  on tiff files
 *
 */

#include "include/tiffio.h"


void tiff_getsize(const char *name, uint32 *diml, uint32 *dimw) {
  TIFF* tiff = TIFFOpen(name, "r");

  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, diml);
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, dimw);

  TIFFClose(tiff);
}

/**
 *  @brief Import a extern tiff file
 *  @param[in] name The path to the image to import
 *  @param[out] matrix The matrix where to put the imported image
 *  @param[out] diml The length of the created matrix (line)
 *  @param[out] dimw The width of the created matrix (column)
 *
 *  @return 1 Error while reading from tiff image
 *  @return 0 Otherwise
 *
 */
int tiff_tomatrix(const char *name, double *matrix, uint32 diml, uint32 dimw) {
  tdata_t buf;
  unsigned char *data;

  uint16 bits;
  uint16 sample;

  TIFF* tiff = TIFFOpen(name, "r");

  buf = _TIFFmalloc(TIFFScanlineSize(tiff));
  data = (unsigned char*) malloc(dimw * sizeof(char));

  for (uint32 row=0; row < diml; row++) {
    if (TIFFReadScanline(tiff, buf, row, 0) == -1)
      return 1;
    memcpy(data, buf, dimw*sizeof(char));
    for (int i=0; i < dimw; i++)
      matrix[row*dimw+i] = (double) data[i];
  }

  free(data);
  _TIFFfree(buf);
  TIFFClose(tiff);
}

int tiff_frommatrix(const char *name, double *matrix,
                    uint32 diml, uint32 dimw) {
  tdata_t buf;
  unsigned char *data;

  TIFF* tiff = TIFFOpen(name, "w");

  buf = _TIFFmalloc(dimw*sizeof(char));
  data = (unsigned char *) malloc(dimw*sizeof(char));

  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, diml);
  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, dimw);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);

  for (uint32 row=0; row < diml; row++) {
    for (int i=0; i < dimw; i++)
      data[i] = (int) matrix[row*dimw+i];
    memcpy(buf, data, dimw*sizeof(char));

    if (TIFFWriteScanline(tiff, buf, row, 0) == -1)
      return 1;
  }

  free(data);
  _TIFFfree(buf);
  TIFFClose(tiff);
}
