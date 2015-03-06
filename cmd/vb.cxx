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

TCLAP::CmdLine parser("Set image values to VALUE if values are below THRESHOLD",
                      ' ', HEIMDALI_VERSION);
// -n
TCLAP::ValueArg<float> numberArg("n","number", "Threshold",true,
                                 1,"THRESHOLD", parser);
// -v
TCLAP::ValueArg<float> valueArg("v","value", "Value to replace with",true,
                                 1,"VALUE", parser);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
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
typename ThresholderType::Pointer thresholder = ThresholderType::New();
thresholder->ThresholdBelow(numberArg.getValue());
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


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "vb: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}


// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "vb: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "vb: ERROR: " << e.getMessage() << endl;
}

return 0;

}
