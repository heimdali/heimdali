#ifndef HEIMDALI_CMDREADER_HXX_INCLUDED
#define HEIMDALI_CMDREADER_HXX_INCLUDED

#include <itkImageFileReader.h>
#include <itkHDF5ImageIO.h>
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include "itkChangeRegionImageFilter.h"
#include <itkDivideImageFilter.h>

#include <string>

extern "C" {
#include "h5unixpipe.h"
}
#include "heimdali/error.hxx"
#include "heimdali/regionreader.hxx"
#include "h5unixpipe_cxx.hxx"

namespace Heimdali {

/** \brief Read HDF5 image from file or stdin, with streaming support.
 *
 * The abstract base class CmdReader has two implementations:
 *     - CmdReaderFromFile
 *     - CmdReaderFromStdin
 *
 */
template <typename ImageType>
class ITK_ABI_EXPORT CmdReader
{
    public:
        typedef itk::ImageFileReader< ImageType >  ReaderType;
        typedef itk::ImageRegion<ImageType::ImageDimension> RegionType;
        typedef itk::Image<typename ImageType::InternalPixelType, ImageType::ImageDimension> ScalarImageType;
        typedef itk::DivideImageFilter<ImageType, ScalarImageType, ImageType> DivideImageFilterType;
    public:
        static CmdReader* make_cmd_reader(
            unsigned int nlines_per_loop, std::string filename);
        typename ReaderType::Pointer reader();
        virtual void next_iteration() = 0;
        virtual typename ImageType::Pointer GetOutput() = 0;
        virtual void Update() = 0;
        bool is_complete(){return m_is_complete;};
        unsigned int get_sz() const {return m_sz;};
        unsigned int get_sy() const {return m_sy;};
        unsigned int get_sx() const {return m_sx;};
        unsigned int get_sc() const {return m_sc;};
        void convert_fixed_point_to_floating_point_on(){m_convert_fixed_point_to_floating_point=true;};
        void convert_fixed_point_to_floating_point_off(){m_convert_fixed_point_to_floating_point=false;};
    protected:
        unsigned int m_nlines_per_loop;
        bool m_is_complete;
        typename ReaderType::Pointer m_reader;
        unsigned int m_iz, m_iy, m_ix;
        unsigned int m_nz, m_ny, m_nx;
        unsigned int m_sz, m_sy, m_sx, m_sc;
        bool m_convert_fixed_point_to_floating_point;
        bool m_convert_fixed_point_to_floating_point_required;
        typename DivideImageFilterType::Pointer m_divider;
};

template <typename ImageType>
class ITK_ABI_EXPORT CmdReaderFromFile: public CmdReader<ImageType>
{
    public:
        CmdReaderFromFile(){};
        CmdReaderFromFile(unsigned int nlines_per_loop, std::string filename);
        ~CmdReaderFromFile();
        void next_iteration();
        typename ImageType::Pointer GetOutput();
        void Update();
    private:
        std::string m_filename;
        RegionReader* m_region_reader;
        typename CmdReader<ImageType>::RegionType m_requestedRegion;
        typename ImageType::IndexType m_index;
        typename ImageType::SizeType m_size;
};

template <typename ImageType>
class ITK_ABI_EXPORT CmdReaderFromStdin: public CmdReader<ImageType>
{
    public:
        typedef itk::ChangeRegionImageFilter<ImageType> ChangeRegionType;
        CmdReaderFromStdin():
            m_is_streamed_subregion(false)
        {};
        CmdReaderFromStdin(unsigned int nlines_per_loop);
        ~CmdReaderFromStdin();
        void next_iteration();
        typename ImageType::Pointer GetOutput();
        void Update(){};
    private:
        hid_t m_fileimage_id; 
        MTB_T* m_traceback;
        itk::HDF5ImageIO::Pointer m_HDF5io;
        H5::H5File* m_fileimage;
        typename ChangeRegionType::Pointer m_changeRegion;
        bool m_is_streamed_subregion;
        //Metadata.
        itk::MetaDataDictionary dictionary;
};

};

#include "cmdreader.txx"

#endif
