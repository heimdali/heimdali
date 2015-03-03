#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkAddImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 
TCLAP::CmdLine parser("Add a constant to an image.", ' ', HEIMDALI_VERSION);

// -n
TCLAP::ValueArg<float> number("n","number", "Value of the constant",true,0,"N", parser);

// -streaming N
TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",
    false, 0,"NUMBER_OF_LINES", parser);
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

// Command line tool reader.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(streaming.getValue(),
                                                    inputFilename);

// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

// Set constant.
ImageType::PixelType pixel;
bool pixel_is_ready = false;

// Add constant.
typedef itk::AddImageFilter<ImageType> AddImageType;
AddImageType::Pointer addImage = AddImageType::New();

unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader->next_iteration();
    if (cmdreader->is_complete()) break;

    if (!pixel_is_ready) {
        pixel.SetSize( cmdreader->get_sc() );
        pixel.Fill( number.getValue() );
        pixel_is_ready = true;
    }

    ImageType::IndexType index;
    index[0] = 2;
    index[1] = 2;
    index[2] = 2;

    // Add constant.
    addImage->SetInput(cmdreader->GetOutput());
    addImage->SetConstant2(pixel);
    addImage->UpdateLargestPossibleRegion();

    // Write output.
    cmdwriter->Write( addImage->GetOutput() );
    cmdwriter->Update();
}

return 0;
}
