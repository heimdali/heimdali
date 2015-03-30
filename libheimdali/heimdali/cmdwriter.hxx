#ifndef HEIMDALI_CMDWRITER_HXX_INCLUDED
#define HEIMDALI_CMDWRITER_HXX_INCLUDED

#include <itkImageFileWriter.h>
#include <itkHDF5ImageIO.h>
#include "itkRegionOfInterestImageFilter.h"
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>

#include <string>

extern "C" {
#include "h5unixpipe.h"
}
#include "heimdali/error.hxx"
#include "h5unixpipe_cxx.hxx"

namespace Heimdali {

template <typename ImageType>
class ITK_ABI_EXPORT CmdWriter
{
    public:
        typedef itk::ImageFileWriter< ImageType >  WriterType;
        typedef itk::ImageRegion<ImageType::ImageDimension> RegionType;
    public:
        static CmdWriter* make_cmd_writer(std::string filename);
        virtual void Write(typename ImageType::Pointer image) = 0;
        virtual void Update() = 0;
    protected:
        unsigned int m_Dimension;
        unsigned int m_nlines_per_loop;
        bool m_is_complete;
        typename WriterType::Pointer m_writer;
        unsigned int m_iz, m_iy, m_ix;
        unsigned int m_nz, m_ny, m_nx;
        unsigned int m_sz, m_sy, m_sx, m_sc;
};

template <typename ImageType>
class ITK_ABI_EXPORT CmdWriterToFile: public CmdWriter<ImageType>
{
    public:
        CmdWriterToFile(){};
        CmdWriterToFile(std::string filename);
        ~CmdWriterToFile();
        void Write(typename ImageType::Pointer image);
        void Update();
    private:
        std::string m_filename;
        typename CmdWriter<ImageType>::RegionType m_requestedRegion;
        typename ImageType::IndexType m_index;
        typename ImageType::SizeType m_size;
        typename CmdWriter<ImageType>::RegionType m_LargestPossibleRegion;
        typename CmdWriter<ImageType>::RegionType m_lregion;
};

template <typename ImageType>
class ITK_ABI_EXPORT CmdWriterToStdout: public CmdWriter<ImageType>
{
    public:
        CmdWriterToStdout();
        ~CmdWriterToStdout();
        void Write(typename ImageType::Pointer image);
        void Update(){};
    private:
        hid_t m_fileimage_id; 
        MTB_T* m_traceback;
        H5::H5File* m_fileimage;
        itk::HDF5ImageIO::Pointer m_HDF5io;
        itk::MetaDataDictionary dictionary;
};
};

#include "cmdwriter.txx"

#endif
