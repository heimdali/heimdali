#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkAddImageFilter.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 
    try {

    TCLAP::CmdLine parser("Add two images", ' ', HEIMDALI_VERSION);

    // -streaming N
    TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",
        false, 0,"NUMBER_OF_LINES", parser);
    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);
    string inputFilename0;
    string inputFilename1;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_in_image_out(filenamesArg, inputFilename0,
                                                      inputFilename1, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;

    // Command line tool readers.
    typedef Heimdali::CmdReader<ImageType> ReaderType;
    ReaderType* reader1 = ReaderType::make_cmd_reader( streaming.getValue(), inputFilename0);
    ReaderType* reader2 = ReaderType::make_cmd_reader(streaming.getValue(),inputFilename1);
    reader1->convert_fixed_point_to_floating_point_on();
    reader2->convert_fixed_point_to_floating_point_on();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<ImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    // Add filter.
    typedef itk::AddImageFilter <ImageType,ImageType> AddImageFilterType;
    AddImageFilterType::Pointer adder = AddImageFilterType::New ();

    unsigned int iregionmax = 1E+06;
    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        reader1->next_iteration();
        reader2->next_iteration();
        if (reader1->is_complete()) break;

        // Add images.
        adder->SetInput1( reader1->GetOutput() );
        adder->SetInput2( reader2->GetOutput() );

        // Write output.
        cmdwriter->Write( adder->GetOutput() );
        cmdwriter->Update();
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
