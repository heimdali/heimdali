#ifndef ITK_HELPER_HXX_INCLUDED
#define ITK_HELPER_HXX_INCLUDED

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkINRImageIOFactory.h"

using namespace std;

typedef float PixelFloat;
const unsigned int ImageDimension = 3;
typedef itk::VectorImage<PixelFloat, ImageDimension> ImageFloat;

void AssertImageSize(ImageFloat::Pointer image, string label, 
  int nx, int ny, int nz=1, int nv=1);

ImageFloat::Pointer CreateImage(int nx, int ny, int nz=1, int nv=1);

ImageFloat::Pointer OpenAndReadImage(string fileName);

void OpenAndWriteImage(string fileName, ImageFloat::Pointer image);

void double_dash_for_longopt(int argc, char **argv);

#endif
