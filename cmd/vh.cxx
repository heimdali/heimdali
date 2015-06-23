#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkThresholdImageFilter.h>
#include <itkINRImageIOFactory.h>
#include <itkVectorIndexSelectionCastImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

    try {
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    vector<string> two_floats = Heimdali::consume_option_values("-n", tclap_argv, 2);
    switch (two_floats.size())
    {
    case 0:
        break;
    case 1:
        throw(Heimdali::Exception("Usage: -n THRESHOLD VALUE"));
        break;
    case 2:
        tclap_argv.push_back("-t");
        tclap_argv.push_back(two_floats[0]);
        tclap_argv.push_back("-v");
        tclap_argv.push_back(two_floats[1]);
        break;
    }

    TCLAP::CmdLine parser("Set image values to VALUE if values are above THRESHOLD",
                          ' ', HEIMDALI_VERSION);
    // -t
    TCLAP::ValueArg<float> thresholdArg("t","threshold",
        "Threshold ('-n THRESHOLD VALUE' is also supported "
        "for INRimage backward compatibility)",
        true, 1,"THRESHOLD", parser);
    // -v
    TCLAP::ValueArg<float> valueArg("v","value",
        "Value to replace with ('-n THRESHOLD VALUE' "
        "is also supported for INRimage backward compatibility)",
        true, 1,"VALUE", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<Heimdali::PixelFloat, Heimdali::ImageDimension> ScalarImageType;
    typedef itk::VectorImage<Heimdali::PixelFloat, Heimdali::ImageDimension> VectorImageType;

    // Command line tool readers.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // thresholder
    typedef itk::ThresholdImageFilter<ScalarImageType> ThresholderType;
    ThresholderType::Pointer thresholder = ThresholderType::New();
    thresholder->ThresholdAbove(thresholdArg.getValue());
    thresholder->SetOutsideValue(valueArg.getValue());

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    ComposerType::Pointer composer = ComposerType::New();

    unsigned int iregionmax = 1E+06;
    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        cmdreader->next_iteration();
        if (cmdreader->is_complete()) break;
        cmdreader->reader()->Update();

        indexer->SetInput(cmdreader->GetOutput());

        for (unsigned int componentIndex = 0 ;
                          componentIndex < cmdreader->get_sc() ;
                          componentIndex++)
        {
            indexer->SetIndex(componentIndex);

            thresholder->SetInput(indexer->GetOutput());
            thresholder->Update();

            duplicator->SetInputImage(thresholder->GetOutput());
            duplicator->Update();

            composer->SetInput(componentIndex, duplicator->GetOutput());
        }
        composer->Update();

        // Write output.
        cmdwriter->Write( composer->GetOutput() );
        cmdwriter->Update();
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
