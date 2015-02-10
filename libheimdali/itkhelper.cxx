#include "heimdali/itkhelper.hxx"

namespace Heimdali {


ImageFloat::RegionType
CreateRegion(int ix, int nx, int iy, int ny, int iz, int nz)
{
   int XD=0, YD=1, ZD=2;

   Heimdali::ImageFloat::RegionType::IndexType start;
   Heimdali::ImageFloat::RegionType::SizeType size;
   Heimdali::ImageFloat::RegionType region;
   start[ZD] = iz;
   start[YD] = iy;
   start[XD] = ix;
   size[ZD] = nz;
   size[YD] = ny;
   size[XD] = nx;
   region.SetSize(size);
   region.SetIndex(start);
   return region;
}

ImageFloat::RegionType::IndexType
CreateIndex(int ix, int iy, int iz)
{
  Heimdali::ImageFloat::RegionType::IndexType index;
  index[XD] = ix;
  index[YD] = iy;
  index[ZD] = iz;
  return index;
}

// Check image size
void AssertImageSize(ImageFloat::Pointer image, string label, 
  int nx, int ny, int nz, int nv)
{
  ImageFloat::RegionType region;
  region = image->GetLargestPossibleRegion();
  ImageFloat::SizeType imageSize = region.GetSize();

  if (imageSize[0] != nx) {
      cerr << "Image " << label << " has incorrect number of rows: " << imageSize[0]
           << ", but " << nx << " is required." << endl;
      exit(1);
  }

  if (imageSize[1] != ny) {
      cerr << "Image " << label << " has incorrect number of columns: " << imageSize[1]
           << ", but " << ny << " is required." << endl;
      exit(1);
  }

  if (imageSize[2] != nz) {
      cerr << "Image " << label << " has incorrect number of frames: " << imageSize[2]
           << ", but " << nz << " is required." << endl;
      exit(1);
  }

  if (image->GetNumberOfComponentsPerPixel() != nv) {
      cerr << "Image " << label << " has incorrect number of components: " 
           << image->GetNumberOfComponentsPerPixel()
           << ", but " << nv << " is required." << endl;
      exit(1);
  }
}

//Create image.
ImageFloat::Pointer CreateImage(int nx, int ny, int nz, int nv)
{
  ImageFloat::Pointer image = ImageFloat::New();
  ImageFloat::IndexType index;
  ImageFloat::SizeType size;
  ImageFloat::RegionType region;

  index.Fill(0);
  size[0] = nx;
  size[1] = ny;
  size[2] = nz;
  region.SetIndex(index);
  region.SetSize(size);

  image->SetRegions(region);
  image->SetNumberOfComponentsPerPixel(nv);
  image->Allocate();

  ImageFloat::PixelType pixel;
  pixel.SetSize(nv);
  pixel.Fill(0.);
  image->FillBuffer(pixel);

  return image;
}

//Load image.
ImageFloat::Pointer OpenAndReadImage(string fileName)
{
  typedef itk::ImageFileReader<ImageFloat> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName);
  reader->Update();
  return reader->GetOutput();
}

//Save image.
void OpenAndWriteImage(string fileName, ImageFloat::Pointer image)
{
  typedef itk::ImageFileWriter<ImageFloat> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fileName);
  writer->SetInput(image);
  writer->Update();
}

};
