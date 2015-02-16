#include "heimdali/inrimage.hxx"

static int ZD=2, YD=1, XD=0;

namespace Heimdali {

//======================================================================
// Constructors
//======================================================================

template <typename PixelType>
InrImage<PixelType>::InrImage():
    m_filename(""),
    m_realz(1)
{
}

template <typename PixelType>
InrImage<PixelType>::InrImage(std::string filename):
    m_filename(filename),
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
}

template <typename PixelType>
void
read(void)
{
}

template <typename PixelType>
void
read(int start_plane)
{
}

template <typename PixelType>
void
read(int start_plane, int size_plane)
{
}

}
