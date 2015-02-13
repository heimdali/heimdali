#include <sstream>

#include <tclap/CmdLine.h>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

#include "heimdali/error.hxx"
#include "heimdali/version.hxx"

using namespace std;

// http://www.itk.org/Wiki/ITK/Examples/IO/ReadUnknownImageType
 
int main(int argc, char *argv[])
{

    try {
        
    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////
    //
    TCLAP::CmdLine cmd("Change pixel type of image",' ', HEIMDALI_VERSION);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","FILE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","FILE-OUT");
    cmd.add(outputFilenameArg);

    //TCLAP::SwitchArg realSwitch("","r", "Convert to flotting point.", cmd);
    //TCLAP::SwitchArg intergralSwitch("","f", "Convert to integral.", cmd);

    cmd.parse(argc,argv);

    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();

    const unsigned int Dimension = 3;
    typedef itk::VectorImage<float, Dimension> FloatImageType;
    typedef itk::VectorImage<int, Dimension> IntImageType;

    typedef itk::ImageIOBase::IOComponentType ScalarPixelType;
 
    itk::ImageIOBase::Pointer imageIO =
          itk::ImageIOFactory::CreateImageIO(
              inputFilename.c_str(), itk::ImageIOFactory::ReadMode);
    
    imageIO->SetFileName(inputFilename);
    imageIO->ReadImageInformation();
    const ScalarPixelType pixelType = imageIO->GetComponentType();
    string pixelTypeString = imageIO->GetComponentTypeAsString(pixelType);

    if (pixelTypeString == "float") {
        // reader
        typedef itk::ImageFileReader<FloatImageType> ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputFilename);

        // writer
        typedef itk::ImageFileWriter<IntImageType> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(outputFilename);

        // cast fileter
        typedef itk::CastImageFilter<FloatImageType, IntImageType > Float2IntCastFilterType;
        Float2IntCastFilterType::Pointer castFilter = Float2IntCastFilterType::New();

        // pipeline
        castFilter->SetInput(reader->GetOutput());
        writer->SetInput(castFilter->GetOutput());
        writer->Update();
    } else if (pixelTypeString == "int") {
        // reader
        typedef itk::ImageFileReader<IntImageType> ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputFilename);

        // writer
        typedef itk::ImageFileWriter<FloatImageType> WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(outputFilename);

        // cast fileter
        typedef itk::CastImageFilter<IntImageType, FloatImageType > Int2FloatCastFilterType;
        Int2FloatCastFilterType::Pointer castFilter = Int2FloatCastFilterType::New();

        // pipeline
        castFilter->SetInput(reader->GetOutput());
        writer->SetInput(castFilter->GetOutput());
        writer->Update();
    } else {
          ostringstream msg;
          msg << "Unsupported pixel type: " << pixelTypeString << endl;
          throw(Heimdali::IOError(msg.str()));
    }

    }

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "cco: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    // Input/output.
    catch (Heimdali::IOError &e) {
        cerr << "ical: ERROR: " << e.getMessage() << endl;
    }

  return 0;
}
