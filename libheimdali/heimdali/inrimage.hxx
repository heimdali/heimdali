#ifndef INRIMAGE_HXX_INCLUDED
#define INRIMAGE_HXX_INCLUDED

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkINRImageIOFactory.h"

namespace Heimdali {

enum {INR_ALONGX, INR_ALONGY, INR_ALONGZ, INR_ALONGV, INR_UNKNOWN};

template <typename PixelType>
class ITK_ABI_EXPORT InrImage 
{
public:
    // typedef.
    typedef itk::VectorImage<PixelType, 3> ImageType;
    typedef itk::ImageFileReader<InrImage<PixelType>::ImageType> ReaderType;
    // Constructors.
    InrImage();
    InrImage(std::string filename);
    // Getters and setters.
    void setRealz(int realz);
    int getRealz(void) const;
    int getDim(int dim) const;
    PixelType* getData(void);
    PixelType operator()(int ix, int iy, int iz=0, int iv=0) const;
    typename InrImage<PixelType>::ImageType::Pointer getImage() const;
    // Write methods.
    void openForWrite(void);
    // Read methods.
    void openForRead(void);
    void read( void);
    void read( int offsetz);
    void read( int offsetz, int nz);
private:
    std::string m_filename;
    PixelType* m_data;
    int m_realz; //! Number of buffered plane.
    typename ReaderType::Pointer m_reader;
    int m_sz, m_sy, m_sx, m_sv, m_syxv, m_sxv;;
    typename InrImage<PixelType>::ImageType::RegionType m_requestedRegion;
    typename InrImage<PixelType>::ImageType::IndexType m_index;
    typename InrImage<PixelType>::ImageType::SizeType m_size;
    typename InrImage<PixelType>::ImageType::Pointer m_image;

};

};

#include "inrimage.txx"

#endif
