#include "heimdali/itkhelper.hxx"
#include "heimdali/cmdwriter.hxx"

using namespace std;

namespace Heimdali {

template <typename ImageType>
CmdWriter<ImageType>* CmdWriter<ImageType>::make_cmd_writer(string filename)
{
    if (filename == "" || filename == "-") {
        return new CmdWriterToStdout<ImageType>();
    } else {
        return new CmdWriterToFile<ImageType>(filename);
    }
}

//////////////////////////////////////////////////////////////////////////////
// CmdWriterToFile
//////////////////////////////////////////////////////////////////////////////

/*!
 *
 * \param[in] sz Size of the ouptut file (z dimension)
 * \param[in] sy Size of the ouptut file (y dimension)
 * \param[in] sx Size of the ouptut file (x dimension)
 *
 */
template <typename ImageType>
CmdWriterToFile<ImageType>::CmdWriterToFile(string filename)
{
    this->m_writer = CmdWriter<ImageType>::WriterType::New();
    this->m_writer->SetFileName(filename);
    this->m_Dimension = 3;
}


template <typename ImageType>
void CmdWriterToFile<ImageType>::Write(typename ImageType::Pointer image)
{
    unsigned int ZD=2, YD=1, XD=0;

    typename ImageType::SizeType size = image->GetBufferedRegion().GetSize();
    typename ImageType::IndexType index = image->GetBufferedRegion().GetIndex();

    // ioregion (same as buffered region)
    itk::ImageIORegion ioregion(this->m_Dimension);
    ioregion.SetIndex(ZD,index[ZD]);
    ioregion.SetIndex(YD,index[YD]);
    ioregion.SetIndex(XD,index[XD]);
    ioregion.SetSize(ZD,size[ZD]);
    ioregion.SetSize(YD,size[YD]);
    ioregion.SetSize(XD,size[XD]);

    this->m_writer->SetInput(image);
    this->m_writer->SetIORegion(ioregion);
    this->m_writer->Update();
}

template <typename ImageType>
CmdWriterToFile<ImageType>::~CmdWriterToFile()
{
}

template <typename ImageType>
void CmdWriterToFile<ImageType>::Update()
{
}


//////////////////////////////////////////////////////////////////////////////
// CmdWriterToStdout
//////////////////////////////////////////////////////////////////////////////


template <typename ImageType>
CmdWriterToStdout<ImageType>::CmdWriterToStdout()
{
    this->m_Dimension = 3;
}

template <typename ImageType>
void CmdWriterToStdout<ImageType>::Write(typename ImageType::Pointer image)
{
    unsigned int ZD=2, YD=1;

    image->Update();
    typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::IndexType index = image->GetBufferedRegion().GetIndex();

    // Set region of interest to buffered region.
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > RegionOfInterestType;
    typename RegionOfInterestType::Pointer regionOfInterest = RegionOfInterestType::New();
    regionOfInterest->SetRegionOfInterest(image->GetBufferedRegion());
    regionOfInterest->SetInput(image);
    regionOfInterest->Update();

    // Add metadata.
    itk::Array<unsigned int> sz_sy_iz_iy(4);
    sz_sy_iz_iy.SetElement(0, size[ZD]);
    sz_sy_iz_iy.SetElement(1, size[YD]);
    sz_sy_iz_iy.SetElement(2, index[ZD]);
    sz_sy_iz_iy.SetElement(3, index[YD]);
    itk::EncapsulateMetaData< itk::Array<unsigned int> >(dictionary,"sz_sy_iz_iy",sz_sy_iz_iy);
    regionOfInterest->GetOutput()->SetMetaDataDictionary(dictionary);

    // Write to standard output.
    this->m_fileimage = h5unixpipe::create_new_fileimage();
    m_HDF5io = itk::HDF5ImageIO::New();
    m_HDF5io->SetH5File(this->m_fileimage);
    this->m_writer = CmdWriter<ImageType>::WriterType::New();
    this->m_writer->SetImageIO(m_HDF5io);
    this->m_writer->SetFileName("ghost.h5");
    this->m_writer->SetInput( regionOfInterest->GetOutput() );
    this->m_writer->Update();
    h5unixpipe::fileimage_to_stdout(this->m_fileimage);

    // Force m_writer close.
    this->m_writer = typename CmdWriter<ImageType>::WriterType::Pointer();
    m_HDF5io = typename itk::HDF5ImageIO::Pointer();
    regionOfInterest = typename RegionOfInterestType::Pointer();
}

};
