#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"

// http://www.itk.org/Wiki/ITK/Examples/IO/ReadUnknownImageType
 
int main(int argc, char *argv[])
{

    try {
        
    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////
    //
    TCLAP::CmdLine cmd("Change pixel type of image",' ', "0.0");

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","FILE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","FILE-OUT");
    cmd.add(outputFilenameArg);

    TCLAP::SwitchArg realSwitch("","r", "Convert to flotting point.", parser);
    TCLAP::SwitchArg intergralSwitch("","f", "Convert to integral.", parser);

    cmd.parse(argc,argv);
    } 

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "cco: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }


typedef itk::ImageIOBase::IOComponentType ScalarPixelType;
 
  itk::ImageIOBase::Pointer imageIO =
        itk::ImageIOFactory::CreateImageIO(
            inputFilename.c_str(), itk::ImageIOFactory::ReadMode);
 
  imageIO->SetFileName(inputFilename);
  imageIO->ReadImageInformation();
  const ScalarPixelType pixelType = imageIO->GetComponentType();
  std::cout << "Pixel Type is " << imageIO->GetComponentTypeAsString(pixelType) // 'double'
            << std::endl;
  const size_t numDimensions =  imageIO->GetNumberOfDimensions();
  std::cout << "numDimensions: " << numDimensions << std::endl; // '2'
 
  std::cout << "component size: " << imageIO->GetComponentSize() << std::endl; // '8'
  std::cout << "pixel type (string): " << imageIO->GetPixelTypeAsString(imageIO->GetPixelType()) << std::endl; // 'vector'
  std::cout << "pixel type: " << imageIO->GetPixelType() << std::endl; // '5'

/*
  switch (pixelType)
  {
    case itk::ImageIOBase::COVARIANTVECTOR:
      typedef itk::Image<unsigned char, 2> ImageType;
      ImageType::Pointer image = ImageType::New();
      ReadFile<ImageType>(inputFilename, image);
      break;
 
      typedef itk::Image<unsigned char, 2> ImageType;
      ImageType::Pointer image = ImageType::New();
      ReadFile<ImageType>(inputFilename, image);
      break;
 
    default:
      std::cerr << "Pixel Type ("
                << imageIO->GetComponentTypeAsString(pixelType)
                << ") not supported. Exiting." << std::endl;
      return -1;
  }
  */

template<typename TImageType>
void ReadFile(std::string filename, typename TImageType::Pointer image)
{
  typedef itk::ImageFileReader<TImageType> ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
 
  reader->SetFileName(filename);
  reader->Update();
 
  image->Graft(reader->GetOutput());
}

    
    /*
    const unsigned int Dimension = 3;

    typedef itk::VectorImage<float, Dimension>  FloatImageType;

    // float to uchar
    typedef itk::VectorImage<unsigned char, Dimension>  UnsignedCharImageType;
    typedef itk::CastImageFilter< FloatImageType, UnsignedCharImageType > Float2UIntCastFilterType;

    // uchar to float
    typedef itk::VectorImage<unsigned char, Dimension>  UnsignedCharImageType;
    typedef itk::CastImageFilter< FloatImageType, UnsignedCharImageType > Float2UIntCastFilterType;

    // reader
    typedef itk::ImageFileReader<FloatImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);

    // writer
    typedef itk::ImageFileWriter<UnsignedCharImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);

    if (choice==0) {
        Float2UIntCastFilterType::Pointer castFilter = Float2UIntCastFilterType::New();
        castFilter->SetInput(reader->GetOutput());
        writer->SetInput(castFilter->GetOutput());
    } else {
    }

    writer->Update();
    */

  return 0;
}
