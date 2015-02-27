#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkMultiplyImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"

using namespace std;

/* Note: There is a bug in MultiplyImageFilter with VectorImage. See:
   https://issues.itk.org/jira/browse/SIMPLEITK-498?
       page=com.atlassian.jira.plugin.system.issuetabpanels:all-tabpanel
   http://comments.gmane.org/gmane.comp.lib.itk.devel/2260 */

int main(int argc, char** argv)
{

TCLAP::CmdLine cmd("Multiply each pixel by NUMBER.", ' ', HEIMDALI_VERSION);

// -n
TCLAP::ValueArg<float> number("n","number", "Value of the number",true,
    1,"NUNMBER", cmd);

// --streaming
TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",false,
    0,"NUMBER_OF_LINES", cmd);

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
typedef itk::Image<PixelType, Dimension>  ScalarImageType;

// MultiplyImageFilter.
typedef itk::MultiplyImageFilter<ImageType, ScalarImageType, ImageType>
    MultiplierType;
MultiplierType::Pointer multiplier = MultiplierType::New();

// Command line tool reader.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(streaming.getValue(),
                                                    input.getValue());

// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(
    output.getValue());

unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader->next_iteration();
    if (cmdreader->is_complete()) break;

    // Mult by constant.
    multiplier->SetInput1( cmdreader->GetOutput() );
    multiplier->SetConstant2(number.getValue());

    // Write output.
    cmdwriter->Write( multiplier->GetOutput() );
    cmdwriter->Update();
}

return 0;

}
