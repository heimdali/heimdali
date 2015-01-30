#include "cmdwriter.hxx"

using namespace std;

namespace Heimdali {

template <typename ImageType>
CmdWriter<ImageType>* CmdWriter<ImageType>::make_cmd_writer(string filename)
{
    if (filename == "") {
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
    size_t zd=2, yd=1, xd=0;
    typename ImageType::SizeType size = image->GetBufferedRegion().GetSize();
    typename ImageType::IndexType index = image->GetBufferedRegion().GetIndex();

    // ioregion (same as buffered region)
    itk::ImageIORegion ioregion(this->m_Dimension);
    ioregion.SetIndex(zd,index[zd]);
    ioregion.SetIndex(yd,index[yd]);
    ioregion.SetIndex(xd,index[xd]);
    ioregion.SetSize(zd,size[zd]);
    ioregion.SetSize(yd,size[yd]);
    ioregion.SetSize(xd,size[xd]);

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
    image->Update();
    typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::IndexType index = image->GetBufferedRegion().GetIndex();
    size_t zd=2, yd=1, xd=0;

    // Set region of interest to buffered region.
    typedef itk::RegionOfInterestImageFilter< ImageType, ImageType > RegionOfInterestType;
    typename RegionOfInterestType::Pointer regionOfInterest = RegionOfInterestType::New();
    regionOfInterest->SetRegionOfInterest(image->GetBufferedRegion());
    regionOfInterest->SetInput(image);
    regionOfInterest->Update();

    // Add metadata.
    itk::Array<size_t> sz_sy_iz_iy(4);
    sz_sy_iz_iy.SetElement(0, size[zd]);
    sz_sy_iz_iy.SetElement(1, size[yd]);
    sz_sy_iz_iy.SetElement(2, index[zd]);
    sz_sy_iz_iy.SetElement(3, index[yd]);
    itk::EncapsulateMetaData< itk::Array<size_t> >(dictionary,"sz_sy_iz_iy",sz_sy_iz_iy);
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
