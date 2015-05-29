#include <vector>
#include <string>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkINRImageIOFactory.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include "itkSobelEdgeDetectionImageFilter.h"

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

using namespace std;

typedef float PixelType;

int main(int argc, char** argv)
{ 

    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    TCLAP::CmdLine parser("Detect image contour",
                          ' ', HEIMDALI_VERSION);

    TCLAP::SwitchArg sobSwitch("","sob", "Sobel", parser);
    TCLAP::SwitchArg kirSwitch("","kir", "Kirsh", parser);
    TCLAP::SwitchArg lapSwitch("","lap", "Laplace", parser);
    TCLAP::SwitchArg hueSwitch("","hue", "Hueckel", parser);
    TCLAP::SwitchArg walSwitch("","wal", "Wallis", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    if (kirSwitch.isSet() || lapSwitch.isSet() || hueSwitch.isSet() ||
        walSwitch.isSet()) {
        ostringstream error_msg;
        error_msg << "Only Sobel implemented";
        throw(Heimdali::Exception(error_msg.str()));
    }

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<PixelType, Heimdali::ImageDimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Heimdali::ImageDimension> VectorImageType;

    // Command line tool readers.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);
    cmdreader->convert_fixed_point_to_floating_point_on();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // Sobel filter
    typedef itk::SobelEdgeDetectionImageFilter <ScalarImageType, ScalarImageType> SobelFilterType;
    SobelFilterType::Pointer sobelFilter = SobelFilterType::New();

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

            sobelFilter->SetInput(indexer->GetOutput());
            sobelFilter->Modified();
            sobelFilter->Update();

            duplicator->SetInputImage(sobelFilter->GetOutput());
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
