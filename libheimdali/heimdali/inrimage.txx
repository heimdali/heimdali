#include "heimdali/inrimage.hxx"

static int ZD=2, YD=1, XD=0;

namespace Heimdali {

//======================================================================
// Constructors
//======================================================================

template <typename PixelType>
InrImage<PixelType>::InrImage():
    m_filename(""),
    m_data(NULL),
    m_realz(1)
{
}

template <typename PixelType>
InrImage<PixelType>::InrImage(std::string filename):
    m_filename(filename),
    m_data(NULL),
    m_realz(1)
{
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
}

//======================================================================
// Read methods.
//======================================================================

template <typename PixelType>
void
InrImage<PixelType>::openForRead(void)
{

    //this->m_reader = ReaderType::New();
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
}

}
