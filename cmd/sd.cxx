#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkDivideImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{

TCLAP::CmdLine parser("Divide each pixel by NUMBER.", ' ', HEIMDALI_VERSION);

// -n
TCLAP::ValueArg<float> number("n","number", "Value of the number",true,
    1,"NUNMBER", parser);

// --streaming
TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",false,
    0,"NUMBER_OF_LINES", parser);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
string inputFilename;
string outputFilename;
Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::VectorImage<PixelType, Dimension> ImageType;
typedef itk::Image<PixelType, Dimension>  ScalarImageType;

// DivideImageFilter.
typedef itk::DivideImageFilter<ImageType, ScalarImageType, ImageType> DivideImageFilterType;
DivideImageFilterType::Pointer divideImageFilter = DivideImageFilterType::New();

// Command line tool reader.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(streaming.getValue(),
                                                    inputFilename);

// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader->next_iteration();
    if (cmdreader->is_complete()) break;

    // Divide by constant.
    divideImageFilter->SetInput1( cmdreader->GetOutput() );
    divideImageFilter->SetConstant2(number.getValue());

    // Write output.
    cmdwriter->Write( divideImageFilter->GetOutput() );
    cmdwriter->Update();
}

return 0;

}
