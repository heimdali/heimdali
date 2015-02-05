#include "cmdreader.hxx"

using namespace std;

namespace Heimdali {

template <typename ImageType>
CmdReader<ImageType>*
CmdReader<ImageType>::make_cmd_reader(
    size_t nlines_per_loop, string filename)
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
    size_t nlines_per_loop, string filename)
{
    int zd=2, yd=1, xd=0;
    this->m_is_complete = false;
    this->m_nlines_per_loop = nlines_per_loop;
    this->m_filename = filename;
    this->m_reader = CmdReader<ImageType>::ReaderType::New();
    this->m_reader->SetFileName(filename);

    // Read image size only.
    this->m_reader->UpdateOutputInformation();
    this->m_sz = this->m_reader->GetImageIO()->GetDimensions(zd);
    this->m_sy = this->m_reader->GetImageIO()->GetDimensions(yd);
    this->m_sx = this->m_reader->GetImageIO()->GetDimensions(xd);
    this->m_sc = this->m_reader->GetImageIO()->GetNumberOfComponents();
    this->m_region_reader = RegionReader::make_region_reader(
        this->m_sz,this->m_sy,nlines_per_loop);

    // Always read a full line.
    this->m_ix = 0;
    this->m_nx = this->m_sx;

    // Region to read.
    this->m_index[xd] = this->m_ix;
    this->m_size[xd] = this->m_nx;
}

template <typename ImageType>
void
CmdReaderFromFile<ImageType>::next_iteration()
{
    int zd=2, yd=1, xd=0;

    // Compute next region to region.
    this->m_region_reader->next_iteration();
    if (this->m_region_reader->is_complete()) {
        this->m_is_complete = true;
        return;
    }
    this->m_region_reader->values(this->m_iz,this->m_iy,this->m_nz,this->m_ny);

    // Set requested region.
    this->m_index[zd] = this->m_iz;
    this->m_index[yd] = this->m_iy;
    this->m_size[zd] = this->m_nz;
    this->m_size[yd] = this->m_ny;
    this->m_requestedRegion.SetIndex(this->m_index);
    this->m_requestedRegion.SetSize(this->m_size);
    this->m_reader->GetOutput()->SetRequestedRegion(this->m_requestedRegion);
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
    return this->m_reader->GetOutput();
}

template <typename ImageType>
void
CmdReaderFromFile<ImageType>::Update()
{
    this->m_reader->Update();
}


//////////////////////////////////////////////////////////////////////////////
// CmdReaderFromStdin
//////////////////////////////////////////////////////////////////////////////


template <typename ImageType>
CmdReaderFromStdin<ImageType>::CmdReaderFromStdin(
    size_t nlines_per_loop)
{
    this->m_nlines_per_loop = nlines_per_loop;
    this->m_HDF5io = itk::HDF5ImageIO::New();
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
    int zd=2, yd=1, xd=0;

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

    // Get metadata.
    itk::Array<size_t> sz_sy_iz_iy(4);
    dictionary = this->m_reader->GetMetaDataDictionary();
    string key = "sz_sy_iz_iy";
    if (dictionary.HasKey(key)) {
      m_using_pipe = true;
      itk::ExposeMetaData< itk::Array<size_t> >(dictionary,key,sz_sy_iz_iy);
      size_t sz = sz_sy_iz_iy[0];
      size_t sy = sz_sy_iz_iy[1];
      size_t iz = sz_sy_iz_iy[2];
      size_t iy = sz_sy_iz_iy[3];

      // Buffered region.
      typename ImageType::IndexType bindex;
      bindex[zd] = iz;
      bindex[yd] = iy;
      bindex[xd] =  0;
      typename ImageType::SizeType bsize = this->m_reader->GetOutput()->GetBufferedRegion().GetSize();
      typename ImageType::RegionType bregion(bindex,bsize);

      // Largest possible region.
      typename ImageType::IndexType lindex;
      lindex[zd] = 0;
      lindex[yd] = 0;
      lindex[xd] = 0;
      typename ImageType::SizeType lsize;
      lsize[zd] = sz;
      lsize[yd] = sy;
      lsize[xd] = bsize[xd];
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
        m_using_pipe = false;
    }
}

template <typename ImageType>
typename ImageType::Pointer
CmdReaderFromStdin<ImageType>::GetOutput()
{
    if (m_using_pipe) {
        return this->m_changeRegion->GetOutput();
    } else {
        return this->m_reader->GetOutput();
    }
}

}
