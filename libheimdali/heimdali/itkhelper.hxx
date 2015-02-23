#ifndef ITK_HELPER_HXX_INCLUDED
#define ITK_HELPER_HXX_INCLUDED

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkINRImageIOFactory.h"

#define GetP(image,ix,iy) image->GetPixel(Heimdali::CreateIndex(ix,iy))
#define SetP(image,ix,iy,value) image->SetPixel(Heimdali::CreateIndex(ix,iy),value)

using namespace std;

namespace Heimdali {

unsigned int ZD=2, YD=1, XD=0;

typedef float PixelFloat;

//! All images are 3-dimensional.
const unsigned int ImageDimension = 3;

typedef itk::VectorImage<PixelFloat, ImageDimension> ImageFloat;

ImageFloat::RegionType
CreateRegion(unsigned int ix, unsigned int nx,
             unsigned int iy, unsigned int ny,
             unsigned int iz=0, unsigned int nz=1);

ImageFloat::RegionType::IndexType
CreateIndex(unsigned int ix, unsigned int iy, unsigned int iz=0);

void AssertImageSize(ImageFloat::Pointer image, string label, 
                     unsigned int nx, unsigned int ny,
                     unsigned int nz=1, unsigned int nv=1);

ImageFloat::Pointer CreateImage(unsigned int nx, unsigned int ny,
                                unsigned int nz=1, unsigned int nv=1);

ImageFloat::Pointer OpenAndReadImage(string fileName);

void OpenAndWriteImage(string fileName, ImageFloat::Pointer image);

};

#endif
