#include <vector>
#include <string>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkINRImageIOFactory.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include "itkRescaleIntensityImageFilter.h"

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

typedef unsigned char PixelType;

int main(int argc, char** argv)
{ 

try {

vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

TCLAP::CmdLine parser("Rescale image value between 0 and 1",
                      ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

parser.parse(tclap_argv);
string inputFilename;
string outputFilename;
Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

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
typedef itk::RescaleIntensityImageFilter<ScalarImageType,ScalarImageType > RescalerType;
RescalerType::Pointer rescaler = RescalerType::New();
rescaler->SetOutputMinimum(0);
rescaler->SetOutputMaximum(255);

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

        rescaler->SetInput(indexer->GetOutput());

        duplicator->SetInputImage(rescaler->GetOutput());
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
    cerr << "sha: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}


// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "sha: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "sha: ERROR: " << e.getMessage() << endl;
}

return 0;

}
