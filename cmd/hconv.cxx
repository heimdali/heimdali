#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/error.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/cmdhelper.hxx"
#include "heimdali/cli.hxx"

using namespace std;

typedef unsigned char PixelType;

template<typename PixelType>
void convert(itk::ImageIOBase::Pointer io, string inputFilename, string outputFilename)
{
    // Image.
    const unsigned int ImageDimension = 3;
    typedef itk::VectorImage<PixelType, ImageDimension> ImageType;
  
    // Reader.
    typedef Heimdali::CmdReader<ImageType> CmdReaderType;
    CmdReaderType* cmdreader = CmdReaderType::make_cmd_reader(0, inputFilename);
    cmdreader->next_iteration(io);
    cmdreader->Update();
 
    // Write output
    typedef Heimdali::CmdWriter<ImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);
    cmdwriter->Write( cmdreader->GetOutput() );
    cmdwriter->Update();
}


int main( int argc, char ** argv )
{
    try {

    TCLAP::CmdLine parser("Convert image from one format to another", ' ', HEIMDALI_VERSION);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)
    
    parser.parse(argc,argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);
    
    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    
    // Image.
    const unsigned int ImageDimension = 3;
    typedef itk::VectorImage<PixelType, ImageDimension> ImageType;

    // Read image information.
    itk::ImageIOBase::Pointer io = Heimdali::open_from_stdin_or_file(inputFilename);
    
    ostringstream error_msg;
    switch (io->GetComponentType())
    {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Component type is unknown";
            throw(Heimdali::Exception(error_msg.str()));
            break;
        case itk::ImageIOBase::UCHAR:
            convert<unsigned char>(io, inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::USHORT:
            convert<unsigned short>(io, inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::UINT:
            convert<unsigned int>(io, inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::FLOAT:
            convert<float>(io, inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::DOUBLE:
            convert<double>(io, inputFilename, outputFilename);
            break;
        default:
             error_msg 
             << "Expected pixel component type to be "
             << "FLOAT, DOUBLE, UCHAR, USHORT or UINT "
             << "but, got "
             << itk::ImageIOBase::GetComponentTypeAsString(io->GetComponentType());
            throw(Heimdali::Exception(error_msg.str()));
            break;
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
