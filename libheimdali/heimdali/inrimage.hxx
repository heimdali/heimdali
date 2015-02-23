#ifndef INRIMAGE_HXX_INCLUDED
#define INRIMAGE_HXX_INCLUDED

#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkINRImageIOFactory.h"

namespace Heimdali {

enum {INR_ALONGX, INR_ALONGY, INR_ALONGZ, INR_ALONGV, INR_UNKNOWN};

enum { INR_ENCODE_UCHAR, INR_ENCODE_USHORT, INR_ENCODE_UINT, INR_ENCODE_FLOAT, 
       INR_ENCODE_UNSUPPORTED };

template <typename PixelType>
class ITK_ABI_EXPORT InrImage 
{
public:
    // typedef.
    typedef itk::VectorImage<PixelType, 3> ImageType;
    typedef itk::ImageFileReader<InrImage<PixelType>::ImageType> ReaderType;
    typedef itk::ImageFileWriter<InrImage<PixelType>::ImageType> WriterType;
    // Constructors.
    InrImage();
    InrImage(unsigned int sx, unsigned int sy, unsigned int sz,
            unsigned int sv, unsigned int type=INR_ENCODE_UNSUPPORTED);
    InrImage(std::string filename);
    // Getters and setters.
    void setRealz(unsigned int realz);
    unsigned int getRealz(void) const;
    unsigned int getDim(unsigned int dim) const;
    PixelType* getData(void);
    PixelType  operator()(unsigned int ix, unsigned int iy,
                          unsigned int iz=0, unsigned int iv=0) const;
    PixelType& operator()(unsigned int ix, unsigned int iy,
                          unsigned int iz=0, unsigned int iv=0) ;
    typename InrImage<PixelType>::ImageType::Pointer getImage() const;
    void setFilename(std::string filename);
    // Write methods.
    void openForWrite(void);
    void write();
    void write(unsigned int offsetz);
    void write(unsigned int offsetz, unsigned int nz);
    // Read methods.
    void openForRead(void);
    void read(void);
    void read(unsigned int offsetz);
    void read(unsigned int offsetz, unsigned int nz);
private:
    std::string m_filename;
    PixelType* m_data;
    unsigned int m_realz; //! Number of buffered plane.
    typename ReaderType::Pointer m_reader;
    typename WriterType::Pointer m_writer;
    unsigned int m_sx, m_sy, m_sz, m_sv, m_syxv, m_sxv;;
    typename InrImage<PixelType>::ImageType::RegionType m_requestedRegion;
    typename InrImage<PixelType>::ImageType::IndexType m_index;
    typename InrImage<PixelType>::ImageType::SizeType m_size;
    typename InrImage<PixelType>::ImageType::Pointer m_image;
};

};

#include "inrimage.txx"

#endif
