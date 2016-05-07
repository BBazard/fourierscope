/* Copyright [2016] <Alexis Lescouet, Benoit Bazard> */
/**
 *  @file
 *
 *  Tiff input/output header
 *
 */

#ifndef RELEASE_INCLUDE_TIFFIO_H_
#define RELEASE_INCLUDE_TIFFIO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tiff.h>
#include <tiffio.h>
#include "include/matrix.h"

int tiff_getsize(const char *name, uint32 *diml, uint32 *dimw);
int tiff_fullscale(double min, double max, double tosample);
int tiff_maxnormalized(double max, double tosample);

int tiff_tomatrix(const char *name, double *matrix, uint32 diml, uint32 dimw);
int tiff_frommatrix(const char *name, double *matrix, uint32 diml, uint32 dimw);
char* tiff_getname(int x, int y, char* name);

#endif /* RELEASE_INCLUDE_TIFFIO_H_ */
