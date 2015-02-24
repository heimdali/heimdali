#include "itkVersion.h"
#include "itkINRImageIO.h"
#include "itkArray.h"
#include "itksys/SystemTools.hxx"

/*
Image planes, rows, columns and pixels
--------------------------------------

Suppose is image is made of:
  - 4 planes numbered (0,1,2,3), so SZ=4.
  - 5 rows numbered (0,1,2,3,4), so SY=5.
  - 6 columns of pixels, numbered (0,1,2,3,4,5), so SX=6
  - each pixel is a vector of 3 values numbered (0,1,2), so SV=3.

 c  c  c  c  c  c
 o  o  o  o  o  o
 l  l  l  l  l  l
 u  u  u  u  u  u
 m  m  m  m  m  m
                 
 0  1  2  3  4  5

.+..+..+..+..+..+.   row 4
.+..+..+..+..+..+.   row 3
.+..+..+..+..+..+.   row 2     A pixel (.+.) has 3 values, the middle one
.+..+..+..+..+..+.   row 1     being represented by a '+' for readibility.
.+..+..+..+..+..+.   row 0
  
     plane 0          
 
Streaming
---------

Suppose we want to read only :
  - the planes (1,2), so IZ=1 and NZ=2.
  - the rows (2,3), so IY=2 and NY=2.
  - the columns (1,2,3), so IX=1 and NX=3.
  - all the pixel values.

I* is the first index, while N* the the number of elements.

The value read are represented with 'r' and 'R', and are stored in 'buffer'.

.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+.rRrrRrrRr.+..+.  .+.rRrrRrrRr.+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+.rRrrRrrRr.+..+.  .+.rRrrRrrRr.+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
      plane 0            plane 1             plane 2             plane 3      


The INRimage library provide the functionnaly to not read the whole image, but
only  a  particular  range of  rows.  So,  we  will  read  only  lines  we are
interessed in in  row_buffer,  extract the columns (1,2,3) and  copy them into
'buffer'.  The value read are represented with 'r' and 'R'

.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  rRrrRrrRrrRrrRrrRr  rRrrRrrRrrRrrRrrRr  .+..+..+..+..+..+.
.+..+..+..+..+..+.  rRrrRrrRrrRrrRrrRr  rRrrRrrRrrRrrRrrRr  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
.+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.  .+..+..+..+..+..+.
      plane 0            plane 1             plane 2             plane 3      

row_buffer has the size:
  rRrrRrrRrrRrrRrrRr

that is, the number of all the value contained in a whole row, that is the
number of columns times the number of values in a pixel.
 */

extern struct error error_;
#define inr_erreur(ier) { error_.retadr = (int(*)())-1; ier = setjmp(error_.retenv); }

namespace itk
{

INRImageIO::INRImageIO():
  m_InrArgc(0),
  m_InrArgv(NULL),
  m_InrProgVers((char*)"1.0-beta"),
  m_InrUcmd((char*)"inr_init"),
  m_InrUdetail((char*)"inr_init"),
  m_InrMode((char*)"e"),
  m_InrVerif((char*)""),
  m_InrIsInitialized(false)
{
    m_InrArgc = 1;
    m_InrArgv = new char*[m_InrArgc]();

    m_InrArgv[0] = new char[ strlen("INRImageIO") + 1];
    strcpy(m_InrArgv[0], "INRImageIO");

    this->AddSupportedWriteExtension(".inr");
    this->AddSupportedWriteExtension(".vel");

    this->AddSupportedReadExtension(".inr");
    this->AddSupportedReadExtension(".vel");
}

INRImageIO::~INRImageIO()
{
    FreeInrArgcArgv();
}

void INRImageIO ::PrintSelf(std::ostream & os, Indent indent) const {
  Superclass::PrintSelf(os, indent);
}

bool INRImageIO ::CanWriteFile(const char *name) {
  InrInit();

   std::string filename = name; 
  
  if (  filename == "" )
    {
    return false;
    }

  std::string::size_type mhaPos = filename.rfind(".inr");
  if ( ( mhaPos != std::string::npos ) 
       && ( mhaPos == filename.length() - 4 ) )
    {
    return true; 
    }

  std::string::size_type mhdPos = filename.rfind(".vel");
  if ( ( mhdPos != std::string::npos ) 
       && ( mhdPos == filename.length() - 4 ) )
    {
    return true;
    }

  return false;
 
}

/*
ImageIOBase::SizeType INRImageIO::GetHeaderSize(void) const
{
    return 256;
}
*/




ImageIORegion INRImageIO ::GenerateStreamableReadRegionFromRequestedRegion(const ImageIORegion & requestedRegion) const
{
  // Set streamable region to be the requestedRegion
  const unsigned int nDims = this->GetNumberOfDimensions();
  ImageIORegion streamableRegion(nDims);
  for ( unsigned int i = 0; i < nDims; i++ )
    {
    streamableRegion.SetIndex(i, requestedRegion.GetIndex(i));
    streamableRegion.SetSize(i, requestedRegion.GetSize(i));
    }

  return streamableRegion;
}


void INRImageIO::Read(void *buffer)
{
  // Get largest possible region (image size).
  const unsigned int nDims = this->GetNumberOfDimensions();
  ImageIORegion largestRegion(nDims);
  for ( unsigned int i = 0; i < nDims; i++ )
    {
    largestRegion.SetIndex(i, 0);
    largestRegion.SetSize( i, this->GetDimensions(i) );
    }

  // Get region to read.
  ImageIORegion regionToRead = this->GetIORegion();

  // Open image.
  m_InrImage = c_image( (char*) m_FileName.c_str(), (char*) m_InrMode.c_str(), 
      (char*) m_InrVerif.c_str(), m_InrFmt);

  // Do we want to read just a part of the image?
  if ( largestRegion != m_IORegion ) {

      // Use more readable names.
      unsigned int SY = m_InrFmt[I_NDIMY];
      unsigned int SX = m_InrFmt[I_NDIMX];
      unsigned int SV = m_InrFmt[I_NDIMV];

      unsigned int IZ = nDims>2 ? regionToRead.GetIndex(2) : 0;
      unsigned int IY = regionToRead.GetIndex(1);
      unsigned int IX = regionToRead.GetIndex(0);

      unsigned int NZ = nDims>2 ? regionToRead.GetSize(2) : 1;
      unsigned int NY = regionToRead.GetSize(1);
      unsigned int NX = regionToRead.GetSize(0);

      // Size (in bytes) of one pixel.
      unsigned int pixel_size = SV * m_InrFmt[I_BSIZE];

      // full row
      unsigned int full_row_size = SX * pixel_size;
      char* full_row_buffer = (char*) malloc(full_row_size);

      // requested row
      unsigned int requested_row_size = NX * pixel_size;
      // buffer has been allocated by ITK to NZ*NY*requested_row_size
      char* buffer_p = (char*) buffer;

      // skipped pixel row.
      unsigned int skipped_size = IX * pixel_size;

      // Iterate on rows we want to read.
      for (unsigned int iz = IZ ; iz < IZ+NZ ; iz++) {
      for (unsigned int iy = IY ; iy < IY+NY ; iy++) {
          // Set cursor to row we want to read.
          unsigned int row_cursor = 
            iz*SY // Number of lines in all previous planes.
            + iy  // Line index in current plane.
            + 1;  // INRimage counts from 1.
          c_lptset(m_InrImage, row_cursor);

          // Read 1 full row.
          c_lect(m_InrImage, 1, full_row_buffer);

          // Extract columns.
          memcpy(buffer_p, full_row_buffer + skipped_size, requested_row_size);

          // Update buffer_p
          buffer_p += requested_row_size;
      }}

  // Or the whole image?
  } else {
      c_lect(m_InrImage,m_InrFmt[I_DIMY],buffer);
  }

  // Close image.
  fermnf_( &m_InrImage);
}

void INRImageIO::WriteImageInformation(void)
{
}

void INRImageIO ::Write(const void *buffer)
{
  InrInit();
  // Set INRimaage format.
  switch( this->GetNumberOfDimensions() )
  {
  case 2:
    m_InrFmt[I_NDIMX] = this->m_Dimensions[0];
    m_InrFmt[I_NDIMY] = this->m_Dimensions[1];
    m_InrFmt[I_NDIMZ] = 1;
    break;
  case 3:
    m_InrFmt[I_NDIMX] = this->m_Dimensions[0];
    m_InrFmt[I_NDIMY] = this->m_Dimensions[1];
    m_InrFmt[I_NDIMZ] = this->m_Dimensions[2];
    break;
  default:
    itkExceptionMacro(<< "unexpected number of dimension: " << GetNumberOfDimensions() );
    break;
  }
  m_InrFmt[I_NDIMV] = this->m_NumberOfComponents;
  m_InrFmt[I_DIMX] = m_InrFmt[I_NDIMX]*m_InrFmt[I_NDIMV];
  m_InrFmt[I_DIMY] = m_InrFmt[I_NDIMY]*m_InrFmt[I_NDIMZ];

  const int inr_integer = 0;
  const int inr_real = 1;

  switch (this->GetComponentType()) {
      case ImageIOBase::UNKNOWNCOMPONENTTYPE:
          itkExceptionMacro(<< "Component type is unknown");
          break;
      case ImageIOBase::FLOAT:
          m_InrFmt[I_TYPE] = inr_real;
          m_InrFmt[I_BSIZE] = 4;
          break;
      case ImageIOBase::DOUBLE:
          m_InrFmt[I_TYPE] = inr_real;
          m_InrFmt[I_BSIZE] = 8;
          break;
      case ImageIOBase::UCHAR:
          m_InrFmt[I_TYPE] = inr_real;
          m_InrFmt[I_BSIZE] = 1;
          break;
      case ImageIOBase::UINT:
          m_InrFmt[I_TYPE] = inr_integer;
          m_InrFmt[I_BSIZE] = 2;
          break;
      case ImageIOBase::ULONG:
          m_InrFmt[I_TYPE] = inr_integer;
          m_InrFmt[I_BSIZE] = 4;
          break;
      default:
        itkExceptionMacro(
            << "Expected pixel component type to be"
            << "FLOAT, DOUBLE, UCHAR, UINT or ULONG"
            << "but, got " << this->GetComponentTypeAsString(this->GetComponentType()));
          break;
  }

  // Open image.
  m_InrImage = c_image( (char*) m_FileName.c_str(), (char*)"c", (char*)"", m_InrFmt);

  // Set image format.
  unsigned int lfmt_size = 9;
  unsigned int offsets_size = 3;
  struct nf_fmt gfmt;
  for (unsigned int i=0 ; i < lfmt_size ; i++) 
      gfmt.lfmt[i] = m_InrFmt[i];
  for (unsigned int i=0 ; i < this->GetNumberOfDimensions() ; i++)  {
      gfmt.offsets[i] = this->m_Origin[i];
  }
  for (unsigned int i=this->GetNumberOfDimensions() ; i<offsets_size ; i++) {
      gfmt.offsets[i] = 0;
  }
  gfmt.maille = 0;
  gfmt.bias = 0.;
  gfmt.scale = 1.;
  gfmset_(&m_InrImage,&gfmt);

  // Write image.
  c_ecrflt(m_InrImage,m_InrFmt[I_DIMY],(float *)buffer);

  // Close image.
  fermnf_(&m_InrImage);
}


void INRImageIO::InrInit()
{
    if ( ! m_InrIsInitialized )
    {
        inr_init(m_InrArgc,m_InrArgv,
            (char*) m_InrProgVers.c_str(), (char*) m_InrUcmd.c_str(), (char*) m_InrUdetail.c_str());
        m_InrIsInitialized = true;
    }
}

void INRImageIO::FreeInrArgcArgv() 
{
    if (m_InrArgv != NULL)
    {
        for (unsigned int iarg = 0 ; iarg < m_InrArgc ; ++iarg)
        {
            if (m_InrArgv[iarg] != NULL)
                delete[] m_InrArgv[iarg];
        }
        delete[] m_InrArgv;
        m_InrArgv = NULL;
        m_InrArgc = 0;
    }
}

/*
void INRImageIO::SetInrArgcArgv(const int InrArgc, ...)
{
    // Copy InrArgc
    m_InrArgc = InrArgc;

    // Free and reallocate m_InrArgv
    FreeInrArgcArgv();
    m_InrArgv = new char*[InrArgc]();

    // One command line argument, for example: "--a-option".
    const char* arg;

    // Number of character in arg, in above example: 10.
    unsigned int lenarg; 

    // Start iteration over command line arguments, passed to the function
    // using '...'.
    va_list InrArgvInput;
    va_start(InrArgvInput, InrArgc);

    // Iteration over '...'.
    for(int iarg = 0 ; iarg < InrArgc ; iarg++)
    {
        // Get the argument.
        arg = va_arg(InrArgvInput, char*);

        // Allocate space for the argument. (+1 for the ending NULL character)
        lenarg = strlen(arg);
        m_InrArgv[iarg] = new char[lenarg+1]();
        
        // Copy the argument 'arg' into 'm_InrArgv[iarg]'.
        strcpy(m_InrArgv[iarg], arg);
    }

    // End iteration over '...'.
    va_end(InrArgvInput);
}

void INRImageIO::SetInrArgcArgv(const int InrArgc, const char** InrArgv)
{
    // Copy InrArgc
    m_InrArgc = InrArgc;

    // Free and reallocate m_InrArgv
    FreeInrArgcArgv();
    m_InrArgv = new char*[InrArgc]();

    // One command line argument, for example: "--a-option".
    const char* arg;

    // Number of character in arg, in above example: 10.
    unsigned int lenarg; 

    // Iteration over InrArgv.
    for(int iarg = 0 ; iarg < InrArgc ; iarg++)
    {
        // Get the argument.
        arg = InrArgv[iarg];

        // Allocate space for the argument. (+1 for the ending NULL character)
        lenarg = strlen(arg);
        m_InrArgv[iarg] = new char[lenarg+1]();
        
        // Copy the argument 'arg' into 'm_InrArgv[iarg]'.
        strcpy(m_InrArgv[iarg], arg);
    }
}
*/

void INRImageIO::SetInrProgVers(const char* InrProgVers)
{
    m_InrProgVers = InrProgVers;
}

void INRImageIO::SetInrUcmd(const char* InrUcmd)
{
    m_InrUcmd = InrUcmd;
}

void INRImageIO::SetInrUdetail(const char* InrUdetail)
{
    InrUdetail = InrUdetail;
}

void INRImageIO::SetInrMode(const char* InrMode)
{
    m_InrMode = InrMode;
}

void INRImageIO::SetInrVerif(const char* InrVerif)
{
    m_InrVerif = InrVerif;
}

} // end namespace itk



bool itk::INRImageIO::CanReadFile(const char *FileNameToRead)
{
  // First check the extension
  if ( (string) FileNameToRead == "" )
    {
    itkDebugMacro(<< "No filename specified.");
    return false;
    }


  // Now check the file header
  InrInit();

  int ier = 0; 
  inr_erreur(ier);
  bool canread;

  if (ier == 0)
  {
      m_InrImage = c_image( (char*) FileNameToRead, (char*) m_InrMode.c_str(), 
          (char*) m_InrVerif.c_str(), m_InrFmt);
      canread = true;
  }
  else
  {
      canread = false;
  }

  fermnf_( &m_InrImage);
  return canread;
}

bool itk::INRImageIO::CanStreamRead()
{
    return true;
}


bool itk::INRImageIO::CanStreamWrite()
{
    return false;
}

void itk::INRImageIO::ReadImageInformation()
{
    InrInit();

    int ier = 0; 
    inr_erreur(ier);
    if (ier == 0)
      {
      m_InrImage = c_image( (char*) m_FileName.c_str(), (char*) m_InrMode.c_str(), 
            (char*) m_InrVerif.c_str(), m_InrFmt);
      }
    else
      {
      fermnf_( &m_InrImage);
      throw std::invalid_argument("File can not be read.");
      }

    if (m_InrFmt[I_NDIMZ]==1) 
      {
        this->SetNumberOfDimensions(2);
        this->m_Dimensions[0] = m_InrFmt[I_NDIMX];
        this->m_Dimensions[1] = m_InrFmt[I_NDIMY];
        this->m_Dimensions[2] = 1;
      } else {
        this->SetNumberOfDimensions(3);
        this->m_Dimensions[0] = m_InrFmt[I_NDIMX];
        this->m_Dimensions[1] = m_InrFmt[I_NDIMY];
        this->m_Dimensions[2] = m_InrFmt[I_NDIMZ];
      }

    this->m_NumberOfComponents = m_InrFmt[I_NDIMV];

    struct nf_fmt gfmt;
    int return_full_format = 1;
    c_getgfm(m_InrImage, &gfmt, return_full_format);

    for (unsigned int i=0 ; i < this->GetNumberOfDimensions() ; i++) 
      {
        this->m_Spacing[i] = 1;
        this->m_Origin[i] = gfmt.offsets[i];
      }
    for (unsigned int i=this->GetNumberOfDimensions(); i < 3 ; i++) 
      {
        this->m_Spacing[i] = 1;
        this->m_Origin[i] = 0;
      }

    const int inr_integer = 0;
    const int inr_real = 1;

    switch (m_InrFmt[I_TYPE]) {

       case inr_real:
           switch (m_InrFmt[I_BSIZE]) {
             case 4:
               this->m_ComponentType = FLOAT;
               break;
             case 8:
               this->m_ComponentType = DOUBLE;
               break;
             default:
                itkExceptionMacro(<< "Expected float pixel component byte size to be 4 or 8"
                                  << "but, got " << m_InrFmt[I_BSIZE]);
           }
           break;

       case inr_integer:
           switch (m_InrFmt[I_BSIZE]) {
             case 1:
               this->m_ComponentType = UCHAR;
               break;
             case 2:
               this->m_ComponentType = UINT;
               break;
             case 4:
               this->m_ComponentType = ULONG;
               break;
             default:
                itkExceptionMacro(<< "Expected integer pixel component byte size to be 4 or 8"
                                  << "but, got " << m_InrFmt[I_BSIZE]);
           }
           break;
    }
}
