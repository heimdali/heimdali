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
    typedef itk::ImageFileReader< InrImage<PixelType>::ImageType > ReaderType;
    // Constructors.
    InrImage();
    InrImage(std::string filename);
    // Getters and setters.
    void setRealz(int realz);
    int getRealz(void) const;
    int getDim(int dim) const;
    // Write methods.
    void openForWrite(void);
    // Read methods.
    void openForRead(void);
    void read( void);
    void read( int start_plane);
    void read( int start_plane, int size_plane);
private:
    std::string m_filename;
    int m_realz; //! Number of buffered plane.
    typename ReaderType::Pointer m_reader;
    int m_sz, m_sy, m_sx, m_sv;

};

};

#include "inrimage.txx"

#endif
