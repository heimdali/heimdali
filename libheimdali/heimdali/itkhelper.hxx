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

typedef float PixelFloat;
const unsigned int ImageDimension = 3;
typedef itk::VectorImage<PixelFloat, ImageDimension> ImageFloat;

ImageFloat::RegionType
CreateRegion(int ix, int nx, int iy, int ny, int iz=0, int nz=1);

ImageFloat::RegionType::IndexType
CreateIndex(int ix, int iy, int iz=0);

void AssertImageSize(ImageFloat::Pointer image, string label, 
  int nx, int ny, int nz=1, int nv=1);

ImageFloat::Pointer CreateImage(int nx, int ny, int nz=1, int nv=1);

ImageFloat::Pointer OpenAndReadImage(string fileName);

void OpenAndWriteImage(string fileName, ImageFloat::Pointer image);

};

#endif
