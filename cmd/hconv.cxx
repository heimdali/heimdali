#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>
#include "itkImageFileWriter.h"

#include "heimdali/version.hxx"
#include "heimdali/error.hxx"
#include "heimdali/itkhelper.hxx"

using namespace std;

typedef unsigned char PixelType;

itk::ImageIOBase::IOComponentType
read_component_type(string inputFilename)
{
    itk::ImageIOBase::Pointer io = 
        itk::ImageIOFactory::CreateImageIO(inputFilename.c_str(),
                                      itk::ImageIOFactory::ReadMode);
    io->SetFileName(inputFilename.c_str());
    io->ReadImageInformation();
    return io->GetComponentType();
}

template<typename PixelType>
void convert(string inputFilename, string outputFilename)
{
    // Image.
    const unsigned int ImageDimension = 3;
    typedef itk::VectorImage<PixelType, ImageDimension> ImageType;
  
    // Reader.
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);

    reader->Update();
  
    // Writer.
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
  
    // Connect reader to writer.
    writer->SetInput(reader->GetOutput());
  
    // Update pipeline.
    writer->Update();
}


int main( int argc, char ** argv )
{
    try {

    TCLAP::CmdLine cmd("Convert image from one format to another", ' ', HEIMDALI_VERSION);
    
    // input.inr
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("IMAGE-IN", 
        "Input image.",true,"","IMAGE-IN",cmd);
    
    // output.hdf5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("IMAGE-OUT", 
        "Output image.",true,"","IMAGE-OUT",cmd);
    
    cmd.parse(argc,argv);
    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();
    
    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    
    // Image.
    const unsigned int ImageDimension = 3;
    typedef itk::VectorImage<PixelType, ImageDimension> ImageType;
    
    // Reader.
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inputFilename  );
    
    itk::ImageIOBase::IOComponentType type = read_component_type(inputFilename);
    ostringstream error_msg;
    switch (type)
    {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Component type is unknown";
            throw(Heimdali::Exception(error_msg.str()));
            break;
        case itk::ImageIOBase::UCHAR:
            convert<unsigned char>(inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::USHORT:
            convert<unsigned short>(inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::UINT:
            convert<unsigned int>(inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::FLOAT:
            convert<float>(inputFilename, outputFilename);
            break;
        case itk::ImageIOBase::DOUBLE:
            convert<double>(inputFilename, outputFilename);
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
