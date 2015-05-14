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
               unsigned int NZ0, unsigned int NY0, unsigned NX0, unsigned NV0)
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

    // Number of plane/column/row read.
    unsigned int NZ = NZ0==0 ? SZ-IZ : NZ0;
    unsigned int NY = NY0==0 ? SY-IY : NY0;
    unsigned int NX = NX0==0 ? SX-IX : NX0;
    unsigned int NV = NV0==0 ? SV-IV : NV0;

    // Readers
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename, NZ,NY,IZ,IY);
    cmdreader->next_iteration(io);
    cmdreader->Update();

    // Create requested region.
    typename VectorImageType::IndexType index;
    index[ZD] = IZ;
    index[YD] = IY;
    index[XD] = IX;
    typename VectorImageType::SizeType size;
    size[ZD] = NZ;
    size[YD] = NY;
    size[XD] = NX;
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    // Extract region
    typedef itk::ExtractImageFilter< VectorImageType, VectorImageType > ExtractFilterType;
    typename ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( cmdreader->GetOutput() );
    extract->SetExtractionRegion( region );

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    typename IndexerType::Pointer indexer = IndexerType::New();

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    typename ComposerType::Pointer composer = ComposerType::New();

    extract->Update();
    indexer->SetInput(extract->GetOutput());

    for (unsigned int iv = IV ; iv < IV+NV ; ++iv) {
        indexer->SetIndex(iv);
        indexer->Update();

        duplicator->SetInputImage(indexer->GetOutput());
        duplicator->Update();

        composer->SetInput(iv-IV, duplicator->GetOutput());
        composer->Modified();
        composer->Update();
    }

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);
    cmdwriter->Write(composer->GetOutput());
    cmdwriter->Update();

}

int main(int argc, char** argv)
{
    try {

    // Parse comamnd line options and arguments.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
        
    TCLAP::CmdLine parser("Extract image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<unsigned int> ixValue("","iz", "First column",false,0,"IZ",parser);
    TCLAP::ValueArg<unsigned int> iyValue("","iy", "First rows",false,0,"IY",parser);
    TCLAP::ValueArg<unsigned int> izValue("","ix", "First plane",false,0,"IX",parser);
    TCLAP::ValueArg<unsigned int> ivValue("","iv", "First component",false,0,"IV",parser);

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
            templated_main<unsigned char>(inputFilename, outputFilename,
                                  io,IZ,IY,IX,IV,NZ0,NY0,NX0,NV0);
            break;
        case itk::ImageIOBase::USHORT:
            templated_main<unsigned short>(inputFilename, outputFilename,
                                  io,IZ,IY,IX,IV,NZ0,NY0,NX0,NV0);
            break;
        case itk::ImageIOBase::UINT:
            templated_main<unsigned int>(inputFilename, outputFilename,
                                  io,IZ,IY,IX,IV,NZ0,NY0,NX0,NV0);
            break;
        case itk::ImageIOBase::FLOAT:
            templated_main<float>(inputFilename, outputFilename,
                                  io,IZ,IY,IX,IV,NZ0,NY0,NX0,NV0);
            break;
        case itk::ImageIOBase::DOUBLE:
            templated_main<double>(inputFilename, outputFilename,
                                  io,IZ,IY,IX,IV,NZ0,NY0,NX0,NV0);
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
