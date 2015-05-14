#include <iostream>
#include <fstream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkArray.h>
#include <itkINRImageIOFactory.h>
#include <itkStatisticsImageFilter.h>
#include <itkINRImageIO.h>
#include <itkHDF5ImageIO.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

#include "itkExtractImageFilter.h"

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

unsigned int ZD=2, YD=1, XD=0;

template <typename PixelType>
void
templated_main(string inputFilename, string outputFilename,
               itk::ImageIOBase::Pointer io,
               unsigned int IZ, unsigned int IY, unsigned IX, unsigned IV,
               unsigned int NZ0, unsigned int NY0, unsigned NX0, unsigned NV0,
               unsigned int DZ, unsigned int DY, unsigned DX, unsigned DV)
{
    // Image type
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    typename VectorImageType::Pointer image;

    // Read image size
    unsigned int SZ = io->GetDimensions(ZD);
    unsigned int SY = io->GetDimensions(YD);
    unsigned int SX = io->GetDimensions(XD);
    unsigned int SV = io->GetNumberOfComponents();

    // Number of plane/column/row.
    unsigned int NZ = NZ0==0 ? SZ-IZ : NZ0;
    unsigned int NY = NY0==0 ? SY-IY : NY0;
    unsigned int NX = NX0==0 ? SX-IX : NX0;
    unsigned int NV = NV0==0 ? SV-IV : NV0;

    // Buffer to read.
    unsigned int BZ = NZ + (NZ-1)*(DZ-1);
    unsigned int BY = NY + (NY-1)*(DY-1);

    // Readers
    // The full raw is read and pixel components are read, and extracted
    // later.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename, BZ,BY,IZ,IY);
    cmdreader->next_iteration(io);
    cmdreader->Update();

    PixelType* p = cmdreader->GetOutput()->GetBufferPointer();

    // Allocate output image.
    typename VectorImageType::SizeType size;
    size[ZD] = NZ;
    size[YD] = NY;
    size[XD] = NX;
    typename VectorImageType::IndexType index;
    index.Fill(0);
    typename VectorImageType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);
    typename VectorImageType::Pointer image_out;
    image_out = VectorImageType::New();
    image_out->SetRegions(region);
    image_out->SetVectorLength(NV);
    image_out->Allocate();
    PixelType* p_out = image_out->GetBufferPointer();

    // Set output image origin.
    typename VectorImageType::PointType origin;
    origin[ZD] = IZ;
    origin[YD] = IY;
    origin[XD] = IX;
    image_out->SetOrigin(origin);

    PixelType* p_in = cmdreader->GetOutput()->GetBufferPointer();

    // Fill output image
    for (unsigned int iz = 0 ; iz < NZ ; iz++) {
        unsigned int ibz = iz*DZ;
        for (unsigned int iy = 0 ; iy < NY ; iy++) {
            unsigned int iby = iy*DY;
            for (unsigned int ix = 0 ; ix < NX ; ix++) {
                unsigned int ibx = IX + ix*DX;
                for (unsigned int iv = 0 ; iv < NV ; iv++) {
                    unsigned int ibv = IV + iv*DV;
                    *(p_out++) = p_in[ibz*BY*SX*SV + iby*SX*SV + ibx*SV + ibv];
                }
            }
        }
    }


    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);
    cmdwriter->Write(image_out);
    cmdwriter->Update();
}

int main(int argc, char** argv)
{
    try {

    // Parse comamnd line options and arguments.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
        
    TCLAP::CmdLine parser("Extract image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix -iv
    TCLAP::ValueArg<unsigned int> izValue("","iz", "First column",false,0,"IZ",parser);
    TCLAP::ValueArg<unsigned int> iyValue("","iy", "First rows",false,0,"IY",parser);
    TCLAP::ValueArg<unsigned int> ixValue("","ix", "First plane",false,0,"IX",parser);
    TCLAP::ValueArg<unsigned int> ivValue("","iv", "First component",false,0,"IV",parser);

    // -zpas -ypas -xpas -vpas
    TCLAP::ValueArg<unsigned int> zpasValue("","zpas", "Column sample step",false,1,"ZPAS",parser);
    TCLAP::ValueArg<unsigned int> ypasValue("","ypas", "Rows sample step",false,1,"YPAS",parser);
    TCLAP::ValueArg<unsigned int> xpasValue("","xpas", "Plane sample step",false,1,"XPAS",parser);
    TCLAP::ValueArg<unsigned int> vpasValue("","vpas", "Component sample step",false,1,"VPAS",parser);

    // -z -y -x
    TCLAP::ValueArg<unsigned int> zValue("z","nplanes", "Number of planes",false,0,"NZ", parser);
    TCLAP::ValueArg<unsigned int> yValue("y","nrows", "Number of rows",false,0,"NY", parser);
    TCLAP::ValueArg<unsigned int> xValue("x","ncolumns", "Number of columns",false,0,"NX", parser);
    TCLAP::ValueArg<unsigned int> vValue("v","ncomponent", "Number of components",false,0,"NV", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // First index to read.
    unsigned int IZ = izValue.getValue();
    unsigned int IY = iyValue.getValue();
    unsigned int IX = ixValue.getValue();
    unsigned int IV = ivValue.getValue();

    // Number of plane/column/row read (0 is the dimension size).
    unsigned int NZ0 = zValue.getValue();
    unsigned int NY0 = yValue.getValue();
    unsigned int NX0 = xValue.getValue();
    unsigned int NV0 = vValue.getValue();

    // Sampling step in each direction
    unsigned int DZ = zpasValue.getValue();
    unsigned int DY = ypasValue.getValue();
    unsigned int DX = xpasValue.getValue();
    unsigned int DV = vpasValue.getValue();

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Read type
    itk::ImageIOBase::Pointer io = Heimdali::open_from_stdin_or_file(inputFilename);
    itk::ImageIOBase::IOComponentType type = io->GetComponentType();

    ostringstream error_msg;
    switch (type)
    {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Component type is unknown";
            throw(Heimdali::Exception(error_msg.str()));
            break;
        case itk::ImageIOBase::UCHAR:
            templated_main<unsigned char>(inputFilename, outputFilename, io,
                                          IZ,IY,IX,IV, NZ0,NY0,NX0,NV0,
                                          DZ,DY,DX,DV);
            break;
        case itk::ImageIOBase::USHORT:
            templated_main<unsigned short>(inputFilename, outputFilename, io,
                                           IZ,IY,IX,IV, NZ0,NY0,NX0,NV0,
                                           DZ,DY,DX,DV);
            break;
        case itk::ImageIOBase::UINT:
            templated_main<unsigned int>(inputFilename, outputFilename, io,
                                         IZ,IY,IX,IV, NZ0,NY0,NX0,NV0,
                                           DZ,DY,DX,DV);
            break;
        case itk::ImageIOBase::FLOAT:
            templated_main<float>(inputFilename, outputFilename, io,
                                  IZ,IY,IX,IV, NZ0,NY0,NX0,NV0,
                                  DZ,DY,DX,DV);
            break;
        case itk::ImageIOBase::DOUBLE:
            templated_main<double>(inputFilename, outputFilename, io,
                                  IZ,IY,IX,IV, NZ0,NY0,NX0,NV0,
                                  DZ,DY,DX,DV);
            break;
        default:
             error_msg 
             << "Expected pixel component type to be "
             << "FLOAT, DOUBLE, UCHAR, USHORT or UINT "
             << "but, got "
             << itk::ImageIOBase::GetComponentTypeAsString(type);
            throw(Heimdali::Exception(error_msg.str()));
            break;
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
