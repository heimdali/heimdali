#include <sstream>
#include <iostream>

#include "heimdali/inrimage.hxx"
#include "heimdali/error.hxx"

#include "itkChangeRegionImageFilter.h"

static int ZD=2, YD=1, XD=0;

namespace Heimdali {

//======================================================================
// Constructors
//======================================================================

template <typename PixelType>
InrImage<PixelType>::InrImage():
    m_filename(""),
    m_data(NULL),
    m_realz(1),
    m_sx(0),
    m_sy(0),
    m_sz(0),
    m_sv(0),
    m_syxv(0),
    m_sxv(0)
{
}

template <typename PixelType>
InrImage<PixelType>::InrImage(std::string filename):
    m_filename(filename),
    m_data(NULL),
    m_realz(1),
    m_sx(0),
    m_sy(0),
    m_sz(0),
    m_sv(0),
    m_syxv(0),
    m_sxv(0)
{
}

/*
 * ignoreArg give image type in Inr++. in InrImage, with use the PixelType
 * template parameter.
 */
template <typename PixelType>
InrImage<PixelType>::InrImage(int sx, int sy, int sz, int sv, int ignoredArg):
    m_data(NULL),
    m_realz(1),
    m_sx(sx),
    m_sy(sy),
    m_sz(sz),
    m_sv(sv),
    m_syxv(sy*sx*sv),
    m_sxv(sx*sv)
{
    if (ignoredArg != INR_ENCODE_UNSUPPORTED) {
        std::cerr << "WARNING: in InrImage constructor, ignore argument <ignoredArg>. "
                  << "<ignoredArg> is provied for backward compatibility" 
                  << std::endl;
    }
}

template <typename PixelType>
void
InrImage<PixelType>::setFilename(std::string filename)
{
    m_filename = filename;
}

//======================================================================
// Getters and setters
//======================================================================

template <typename PixelType>
void
InrImage<PixelType>::setRealz(int realz)
{
    this->m_realz = realz;
}

template <typename PixelType>
int
InrImage<PixelType>::getRealz(void) const
{
    return m_realz;
}

template <typename PixelType>
int
InrImage<PixelType>::getDim(int dim) const
{
  switch(dim){
      case Heimdali::INR_ALONGX: return this->m_sx;
      case Heimdali::INR_ALONGY: return this->m_sy;
      case Heimdali::INR_ALONGZ: return this->m_sz;
      case Heimdali::INR_ALONGV: return this->m_sv;
      default: return Heimdali::INR_UNKNOWN ;
  }
}

template <typename PixelType>
PixelType*
InrImage<PixelType>::getData(void)
{
    return m_data;
}

template <typename PixelType>
PixelType
InrImage<PixelType>::operator()(int ix, int iy, int iz, int iv) const
{
    return m_data[iz*m_syxv + iy*m_sxv + ix*m_sv + iv];
}

template <typename PixelType>
PixelType&
InrImage<PixelType>::operator()(int ix, int iy, int iz, int iv) 
{
    return m_data[iz*m_syxv + iy*m_sxv + ix*m_sv + iv];
}

template <typename PixelType>
typename InrImage<PixelType>::ImageType::Pointer
InrImage<PixelType>::getImage() const
{
    return m_image;
}

//======================================================================
// Write methods.
//======================================================================

template <typename PixelType>
void
InrImage<PixelType>::openForWrite(void)
{
    // Create an empty file. Pasting in ITK assumes the file already exists.

    // Create writer.
    this->m_writer = InrImage<PixelType>::WriterType::New();
    this->m_writer->SetFileName(this->m_filename);

    // Create region index.
    typename InrImage<PixelType>::ImageType::IndexType index;
    index.Fill(0);

    // Create region size.
    typename InrImage<PixelType>::ImageType::SizeType size;
    size[0] = m_sx;
    size[1] = m_sy;
    size[2] = m_realz;

    // Create region.
    typename InrImage<PixelType>::ImageType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    // Create image.
    this->m_image = InrImage<PixelType>::ImageType::New();
    this->m_image->SetRegions(region);
    this->m_image->SetNumberOfComponentsPerPixel(m_sv);
    this->m_image->Allocate();

    // Initialize image.
    typename InrImage<PixelType>::ImageType::PixelType pixel;
    pixel.SetSize(this->m_sv);
    pixel.Fill(0.);
    this->m_image->FillBuffer(pixel);

    this->m_data = this->m_image->GetBufferPointer();
}

//! Write all planes in memory (see setRealz()) 
template <typename PixelType>
void
InrImage<PixelType>::write(void)
{
    this->m_writer->SetInput(this->m_image);
    this->m_writer->Update();
}

//! Write all plane in memory to file at offsetz.
template <typename PixelType>
void
InrImage<PixelType>::write(int offsetz)
{
    typename InrImage<PixelType>::ImageType::IndexType index;
    typename InrImage<PixelType>::ImageType::SizeType size;

    // Buffered and IO regions.
    index[ZD] = offsetz;
    index[YD] = 0;
    index[XD] = 0;
    size[ZD] = this->m_realz;
    size[YD] = this->m_sy;
    size[XD] = this->m_sx;
    typename InrImage<PixelType>::ImageType::RegionType bregion(index,size);
    itk::ImageIORegion ioregion(3);
    ioregion.SetIndex(ZD,index[ZD]);
    ioregion.SetIndex(YD,index[YD]);
    ioregion.SetIndex(XD,index[XD]);
    ioregion.SetSize(ZD,size[ZD]);
    ioregion.SetSize(YD,size[YD]);
    ioregion.SetSize(XD,size[XD]);

    // Largest possible region.
    index[ZD] = 0;
    index[YD] = 0;
    index[XD] = 0;
    size[ZD] = this->m_sz;
    size[YD] = this->m_sy;
    size[XD] = this->m_sx;
    typename InrImage<PixelType>::ImageType::RegionType lregion(index,size);

    // Origin.
    typename InrImage<PixelType>::ImageType::PointType origin;
    origin[2] = 0;
    origin[1] = 0;
    origin[0] = 0;

    // Change regions.
    typedef itk::ChangeRegionImageFilter<InrImage<PixelType>::ImageType> ChangeRegionType;
    typename ChangeRegionType::Pointer changeRegion = ChangeRegionType::New();
    changeRegion = ChangeRegionType::New();
    changeRegion->SetLargestPossibleRegion(lregion);
    changeRegion->SetBufferedRegion(bregion);
    changeRegion->SetOrigin(origin);
    changeRegion->SetInput(this->m_image);
    changeRegion->Update();

    // Update writer.
    this->m_writer->SetInput(changeRegion->GetOutput());
    this->m_writer->SetIORegion(ioregion);
    this->m_writer->Update();
}

//! Write planes in memory between 0 and nz to file at offsetz
template <typename PixelType>
void
InrImage<PixelType>::write(int offsetz, int nz)
{
    if (nz > this->m_realz) {
        std::ostringstream msg;
        msg << "Trying to write " << nz << " planes, but there are only "
            << this->m_realz << " planes in memory";
        throw(ValueError(msg.str()));
    }

}

//======================================================================
// Read methods.
//======================================================================

template <typename PixelType>
void
InrImage<PixelType>::openForRead(void)
{

    this->m_reader = InrImage<PixelType>::ReaderType::New();
    this->m_reader->SetFileName(this->m_filename);

    // Read image size only.
    this->m_reader->UpdateOutputInformation();
    this->m_sz = this->m_reader->GetImageIO()->GetDimensions(ZD);
    this->m_sy = this->m_reader->GetImageIO()->GetDimensions(YD);
    this->m_sx = this->m_reader->GetImageIO()->GetDimensions(XD);
    this->m_sv = this->m_reader->GetImageIO()->GetNumberOfComponents();

    this->m_index[YD] = 0;
    this->m_index[XD] = 0;

    this->m_size[YD] = m_sy;
    this->m_size[XD] = m_sx;

    this->m_syxv = this->m_sy * this->m_sx * this->m_sv;
    this->m_sxv = this->m_sx * this->m_sv;
}

//! Read all planes. 
template <typename PixelType>
void
InrImage<PixelType>::read(void)
{
    this->m_index[ZD] = 0;
    this->m_size[ZD] = m_sz;
    this->m_requestedRegion.SetIndex(this->m_index);
    this->m_requestedRegion.SetSize(this->m_size);
    this->m_reader->GetOutput()->SetRequestedRegion(this->m_requestedRegion);
    this->m_reader->Update();
    this->m_image = this->m_reader->GetOutput();
    this->m_data = this->m_image->GetBufferPointer();
    this->m_realz = m_sz;
}

//! Read one plane
template <typename PixelType>
void
InrImage<PixelType>::read(int offsetz)
{
    this->m_index[ZD] = offsetz;
    this->m_size[ZD] = 1;
    this->m_requestedRegion.SetIndex(this->m_index);
    this->m_requestedRegion.SetSize(this->m_size);
    this->m_reader->GetOutput()->SetRequestedRegion(this->m_requestedRegion);
    this->m_reader->Update();
    this->m_image = this->m_reader->GetOutput();
    this->m_data = this->m_image->GetBufferPointer();
    this->m_realz = 1;
}

//! Read N planes.
template <typename PixelType>
void
InrImage<PixelType>::read(int offsetz, int nz)
{
    this->m_index[ZD] = offsetz;
    this->m_size[ZD] = nz;
    this->m_requestedRegion.SetIndex(this->m_index);
    this->m_requestedRegion.SetSize(this->m_size);
    this->m_reader->GetOutput()->SetRequestedRegion(this->m_requestedRegion);
    this->m_reader->Update();
    this->m_image = this->m_reader->GetOutput();
    this->m_data = this->m_image->GetBufferPointer();
    this->m_realz = nz;
}

}
