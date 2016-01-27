/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  Tiff input/output header file
 *
 */

#ifndef RELEASE_INCLUDE_TIFFIO_H_
#define RELEASE_INCLUDE_TIFFIO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tiff.h>
#include <tiffio.h>

void tiff_getsize(const char *name, uint32 *diml, uint32 *dimw);
int tiff_tomatrix(const char *name, double *matrix, uint32 diml, uint32 dimw);
int tiff_frommatrix(const char *name, double *matrix, uint32 diml, uint32 dimw);

#endif /* RELEASE_INCLUDE_TIFFIO_H_ */
