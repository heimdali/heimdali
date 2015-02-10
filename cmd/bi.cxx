#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkAddImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"

using namespace std;

int main(int argc, char** argv)
{ 
TCLAP::CmdLine cmd("Add a constant to an image.", ' ', "0.0.0");

// -n
TCLAP::ValueArg<float> number("n","number", "Value of the constant",true,0,"N", cmd);

// -streaming N
TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",
    false, 0,"NUMBER_OF_LINES", cmd);

// input.h5
TCLAP::ValueArg<string> input("i","input", 
    "Input image, instead of standard input",false,"","FILENAME", cmd);

// output.h5
TCLAP::ValueArg<string> output("o","output", 
    "Output image, instead of standard output",false,"","FILENAME", cmd);

cmd.parse(argc,argv);

// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::VectorImage<PixelType, Dimension> ImageType;

// Command line tool reader.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(streaming.getValue(),
                                                    input.getValue());

// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(output.getValue());

// Set constant.
ImageType::PixelType pixel;
int sc = cmdreader->get_sc();
pixel.SetSize(sc);
pixel.Fill( number.getValue() );

// Add constant.
typedef itk::AddImageFilter<ImageType> AddImageType;
AddImageType::Pointer addImage = AddImageType::New();

size_t iregionmax = 1E+06;
for (size_t iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader->next_iteration();
    if (cmdreader->is_complete()) break;

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
