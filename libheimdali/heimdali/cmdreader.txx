#include "heimdali/itkhelper.hxx"
#include "heimdali/cmdreader.hxx"

using namespace std;

namespace Heimdali {

template <typename ImageType>
CmdReader<ImageType>*
CmdReader<ImageType>::make_cmd_reader(
    unsigned int nlines_per_loop, string filename)
{
    if (filename == "" or filename == "-") {
        return new CmdReaderFromStdin<ImageType>(nlines_per_loop);
    } else {
        return new CmdReaderFromFile<ImageType>(nlines_per_loop,filename);
    }
}

template <typename ImageType>
typename itk::ImageFileReader< ImageType >::Pointer
CmdReader<ImageType>::reader()
{
    return m_reader;
}

//////////////////////////////////////////////////////////////////////////////
// CmdReaderFromFile
//////////////////////////////////////////////////////////////////////////////

template <typename ImageType>
CmdReaderFromFile<ImageType>::CmdReaderFromFile(
    unsigned int nlines_per_loop, string filename)
{
    this->m_is_complete = false;
    this->m_nlines_per_loop = nlines_per_loop;
    this->m_filename = filename;
    this->m_reader = CmdReader<ImageType>::ReaderType::New();
    this->m_reader->SetFileName(filename);
    this->m_convert_fixed_point_to_floating_point = false;
    this->m_convert_fixed_point_to_floating_point_required = false;

    // Read image size only.
    this->m_reader->UpdateOutputInformation();
    this->m_sz = this->m_reader->GetImageIO()->GetDimensions(ZD);
    this->m_sy = this->m_reader->GetImageIO()->GetDimensions(YD);
    this->m_sx = this->m_reader->GetImageIO()->GetDimensions(XD);
    this->m_sc = this->m_reader->GetImageIO()->GetNumberOfComponents();
    this->m_region_reader = RegionReader::make_region_reader(
        this->m_sz,this->m_sy,nlines_per_loop);

    // Always read a full line.
    this->m_ix = 0;
    this->m_nx = this->m_sx;

    // Region to read.
    this->m_index[XD] = this->m_ix;
    this->m_size[XD] = this->m_nx;

    // DivideImageFilter.
    this->m_divider = CmdReader<ImageType>::DivideImageFilterType::New();
}

int get_fixed_point_divider(itk::ImageIOBase::Pointer io)
{
    if ( (string) io->GetNameOfClass() != (string) "HDF5ImageIO" &&
         (string) io->GetNameOfClass() != (string) "INRImageIO" ) return 0;

    ostringstream error_msg;

    switch (io->GetComponentType())
    {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Component type is unknown";
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        case itk::ImageIOBase::FLOAT:
            return 0;
            break;
        case itk::ImageIOBase::DOUBLE:
            return 0;
            break;
        case itk::ImageIOBase::UCHAR:
            return 255;
            break;
        case itk::ImageIOBase::USHORT:
            return 65535;
            break;
        case itk::ImageIOBase::UINT:
            return 4294967295;
            break;
        default:
            error_msg << "Expected pixel component type to be"
                      << "FLOAT, DOUBLE, UCHAR, USHORT or UINT"
                      << "but, got " << io->GetComponentTypeAsString(io->GetComponentType());
            throw(Heimdali::ValueError(error_msg.str()));
            break;
    }
}

template <typename ImageType>
void
CmdReaderFromFile<ImageType>::next_iteration()
{

    // Compute next region to region.
    this->m_region_reader->next_iteration();
    if (this->m_region_reader->is_complete()) {
        this->m_is_complete = true;
        return;
    }
    this->m_region_reader->values(this->m_iz,this->m_iy,this->m_nz,this->m_ny);

    // Set requested region.
    this->m_index[ZD] = this->m_iz;
    this->m_index[YD] = this->m_iy;
    this->m_size[ZD] = this->m_nz;
    this->m_size[YD] = this->m_ny;
    this->m_requestedRegion.SetIndex(this->m_index);
    this->m_requestedRegion.SetSize(this->m_size);
    this->m_reader->GetOutput()->SetRequestedRegion(this->m_requestedRegion);

    int fixed_point_divider = get_fixed_point_divider(this->m_reader->GetImageIO());

    this->m_convert_fixed_point_to_floating_point_required = 
        this->m_convert_fixed_point_to_floating_point && 
        (fixed_point_divider != 0);

    if (this->m_convert_fixed_point_to_floating_point_required) {
        this->m_divider->SetInput1(this->m_reader->GetOutput());
        this->m_divider->SetConstant2(fixed_point_divider);
    }
}

template <typename ImageType>
CmdReaderFromFile<ImageType>::~CmdReaderFromFile()
{
    delete m_region_reader;
}

template <typename ImageType>
typename ImageType::Pointer
CmdReaderFromFile<ImageType>::GetOutput()
{
    if (this->m_convert_fixed_point_to_floating_point_required) {
        return this->m_divider->GetOutput();
    } else {
        return this->m_reader->GetOutput();
    }
}

template <typename ImageType>
void
CmdReaderFromFile<ImageType>::Update()
{
    if (this->m_convert_fixed_point_to_floating_point_required) {
        this->m_divider->Update();
    } else {
        this->m_reader->Update();
    }
}


//////////////////////////////////////////////////////////////////////////////
// CmdReaderFromStdin
//////////////////////////////////////////////////////////////////////////////


template <typename ImageType>
CmdReaderFromStdin<ImageType>::CmdReaderFromStdin(
    unsigned int nlines_per_loop)
{
    this->m_convert_fixed_point_to_floating_point = false;
    this->m_convert_fixed_point_to_floating_point_required = false;
    this->m_nlines_per_loop = nlines_per_loop;
    this->m_HDF5io = itk::HDF5ImageIO::New();
    this->m_is_complete = false;
    m_traceback = MTB_new_tb();
}

template <typename ImageType>
CmdReaderFromStdin<ImageType>::~CmdReaderFromStdin()
{
    delete m_traceback;
}

template <typename ImageType>
void
CmdReaderFromStdin<ImageType>::next_iteration()
{
    int end_of_stdin=0;
    this->m_fileimage_id = H5UPfileimage_from_stdin(&end_of_stdin, m_traceback);
    if (end_of_stdin==1) {
        this->m_is_complete = true;
        return;
    }
    this->m_fileimage = h5unixpipe::fileid_to_h5file(this->m_fileimage_id);
    this->m_HDF5io->SetH5File(this->m_fileimage);
    this->m_reader = typename CmdReader<ImageType>::ReaderType::Pointer();
    this->m_reader = CmdReader<ImageType>::ReaderType::New();
    this->m_reader->SetFileName("ghost.h5");
    this->m_reader->SetImageIO(this->m_HDF5io);
    this->m_reader->Update();

    this->m_sx = m_HDF5io->GetDimensions(ZD);
    this->m_sy = m_HDF5io->GetDimensions(YD);
    this->m_sz = m_HDF5io->GetDimensions(XD);
    this->m_sc = m_HDF5io->GetNumberOfComponents();

    // Get metadata.
    itk::Array<unsigned int> sz_sy_iz_iy(4);
    dictionary = this->m_reader->GetMetaDataDictionary();
    string key = "sz_sy_iz_iy";
    if (dictionary.HasKey(key)) {
      this->m_is_streamed_subregion = true;
      itk::ExposeMetaData< itk::Array<unsigned int> >(dictionary,key,sz_sy_iz_iy);
      unsigned int sz = sz_sy_iz_iy[0];
      unsigned int sy = sz_sy_iz_iy[1];
      unsigned int iz = sz_sy_iz_iy[2];
      unsigned int iy = sz_sy_iz_iy[3];

      // Buffered region.
      typename ImageType::IndexType bindex;
      bindex[ZD] = iz;
      bindex[YD] = iy;
      bindex[XD] =  0;
      typename ImageType::SizeType bsize = this->m_reader->GetOutput()->GetBufferedRegion().GetSize();
      typename ImageType::RegionType bregion(bindex,bsize);

      // Largest possible region.
      typename ImageType::IndexType lindex;
      lindex[ZD] = 0;
      lindex[YD] = 0;
      lindex[XD] = 0;
      typename ImageType::SizeType lsize;
      lsize[ZD] = sz;
      lsize[YD] = sy;
      lsize[XD] = bsize[XD];
      typename ImageType::RegionType lregion(lindex,lsize);

      // Origin.
      typename ImageType::PointType origin;
      origin[2] = 0;
      origin[1] = 0;
      origin[0] = 0;

      // Change regions.
      this->m_changeRegion = typename ChangeRegionType::Pointer();
      this->m_changeRegion = ChangeRegionType::New();
      this->m_changeRegion->SetLargestPossibleRegion(lregion);
      this->m_changeRegion->SetBufferedRegion(bregion);
      this->m_changeRegion->SetOrigin(origin);
      this->m_changeRegion->SetInput( this->m_reader->GetOutput() );
      this->m_changeRegion->Update();
    } else {
        this->m_is_streamed_subregion = false;
    }
}

template <typename ImageType>
typename ImageType::Pointer
CmdReaderFromStdin<ImageType>::GetOutput()
{
    if (this->m_is_streamed_subregion) {
        return this->m_changeRegion->GetOutput();
    } else {
        return this->m_reader->GetOutput();
    }
}

}
