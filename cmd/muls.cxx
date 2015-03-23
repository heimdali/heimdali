#include <vector>
#include <string>
#include <sstream>

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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

typedef unsigned char PixelType;

int main(int argc, char** argv)
{ 

    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    TCLAP::CmdLine parser("Threshold image with multiple thresholds",
                          ' ', HEIMDALI_VERSION);
    // -n
    TCLAP::ValueArg<string> vsArg("","vs", "Commad separated threshold values list",true,
                                     "","THRESHOLD ...", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    /*
    For example, if -sc=0.2,0.4,0.6,0.8 :
    threshold_below threshold_above threshold_value
    0.0             0.2             0*255 / 4
    0.2             0.4             1*255 / 4
    0.4             0.6             2*255 / 4
    0.6             0.8             3*255 / 4
    0.8             1.0             4*255 / 4
    */
    std::vector<std::string> threshold_string = split(vsArg.getValue(), ',');
    std::vector<float> threshold_below;
    std::vector<float> threshold_above;
    std::vector<PixelType> threshold_value;
    threshold_below.push_back(0.0);
    for (unsigned int i = 0 ; i < threshold_string.size() ; i++) {
        float th = atof(threshold_string[i].c_str());
        threshold_below.push_back(th);
        threshold_above.push_back(th);
        threshold_value.push_back( i*255 / threshold_string.size() );
    }
    threshold_above.push_back(1.);
    threshold_value.push_back(255);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<PixelType, Heimdali::ImageDimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Heimdali::ImageDimension> VectorImageType;

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

            for (unsigned int interval = 0 ; interval < threshold_value.size() ; interval++) {
                thresholder->ThresholdBelow(threshold_below[interval]);
                thresholder->SetOutsideValue(threshold_value[interval]);
                thresholder->Modified();
                thresholder->Update();
            }

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
