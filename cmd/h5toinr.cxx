#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>
#include "itkImageFileWriter.h"

// author: <david.froger@inria.fr>
// usage: Please see 'help' function bellow.

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Function to print help message.
///////////////////////////////////////////////////////////////////////////

int main( int argc, char ** argv )
{


  try {


    ///////////////////////////////////////////////////////////////////////////
    // Parse command line arguments.
    ///////////////////////////////////////////////////////////////////////////


    TCLAP::CmdLine cmd("Convert HDF5 image to INRimage image.", ' ', "0.0");

    // input.inr
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "HDF5 image.",true,"","outputFilename",cmd);

    // output.hdf5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "INRimage image.",true,"","inputFilename",cmd);

    cmd.parse(argc,argv);

    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();



   ///////////////////////////////////////////////////////////////////////////
   // Read and write image
   ///////////////////////////////////////////////////////////////////////////


   // Put our INRimage reader in the list of readers ITK knows.
   itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

   // Image.
   typedef float PixelType;
   const unsigned int Dimension = 3;
   typedef itk::VectorImage<PixelType, Dimension> ImageType;

   // Reader.
   typedef itk::ImageFileReader< ImageType >  ReaderType;
   ReaderType::Pointer reader = ReaderType::New();
   reader->SetFileName( inputFilename  );

   reader->Update();

   // Writer.
   typedef itk::ImageFileWriter<ImageType>  WriterType;
   WriterType::Pointer writer = WriterType::New();
   writer->SetFileName(outputFilename);

   // Connect reader to writer.
   writer->SetInput( reader->GetOutput() );

   // Update pipeline.
   writer->Update();

  }

  // Command line parser.
  catch (TCLAP::ArgException &e) { 
      cerr << "hdf52inr: ERROR: " << e.error() << " for arg " << e.argId() << endl;
  }

  return 0;
}
