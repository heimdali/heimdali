#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

TCLAP::CmdLine parser("Direct FFT on real image", ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
string inputFilename;
string outputFilename0;
string outputFilename1;
Heimdali::parse_tclap_image_in_image_out_image_out(filenamesArg, inputFilename,
                                                   outputFilename0, outputFilename1);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::VectorImage<PixelType, Dimension> ImageType;

// Command line tool readers.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* reader = ReaderType::make_cmd_reader(0, inputFilename);

// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* writer0 = WriterType::make_cmd_writer(outputFilename0);
WriterType* writer1 = WriterType::make_cmd_writer(outputFilename1);

/*
unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    reader->next_iteration();
    if (reader->is_complete()) break;

    // Subtract images.
    subtracter->SetInput1( reader->GetOutput() );

    // Write output.
    writer0->Write( subtracter->GetOutput() );
    writer1->Write( subtracter->GetOutput() );
    writer0->Update();
    writer1->Update();
}
*/

return 0;

}
