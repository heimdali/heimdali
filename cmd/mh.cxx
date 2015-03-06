#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkINRImageIOFactory.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkCastImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Set image values to VALUE if values are above THRESHOLD",
                      ' ', HEIMDALI_VERSION);
// -n
TCLAP::ValueArg<float> numberArg("n","number", "Threshold",true,
                                 1,"THRESHOLD", parser);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
string inputFilename;
string outputFilename;
Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef itk::Image<Heimdali::PixelFloat, Heimdali::ImageDimension> ScalarImageType;
typedef itk::VectorImage<Heimdali::PixelFloat, Heimdali::ImageDimension> FloatVectorImageType;
typedef itk::VectorImage<unsigned char, Heimdali::ImageDimension> BinaryVectorImageType;

// Command line tool readers.
typedef Heimdali::CmdReader<FloatVectorImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);
cmdreader->convert_fixed_point_to_floating_point_on();

// indexer
typedef itk::VectorIndexSelectionCastImageFilter<FloatVectorImageType, ScalarImageType> IndexerType;
IndexerType::Pointer indexer = IndexerType::New();

// binary_thresholder
typedef itk::BinaryThresholdImageFilter<ScalarImageType, ScalarImageType> 
    BinaryThresholderType;
typename BinaryThresholderType::Pointer binary_thresholder = BinaryThresholderType::New();

// duplicator
typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
DuplicatorType::Pointer duplicator = DuplicatorType::New();

// composer
typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
ComposerType::Pointer composer = ComposerType::New();

// caster
typedef itk::CastImageFilter<FloatVectorImageType, BinaryVectorImageType > CasterType;
typename CasterType::Pointer caster = CasterType::New();

// Command line tool writer.
typedef Heimdali::CmdWriter<BinaryVectorImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

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

        binary_thresholder->SetInput(indexer->GetOutput());
        binary_thresholder->SetUpperThreshold( numberArg.getValue() );
        binary_thresholder->SetInsideValue(0);
        binary_thresholder->SetOutsideValue(255);
        binary_thresholder->Update();
        binary_thresholder->Modified();

        duplicator->SetInputImage(binary_thresholder->GetOutput());
        duplicator->Update();

        composer->SetInput(componentIndex, duplicator->GetOutput());
    }
    composer->Update();

    caster->SetInput(composer->GetOutput());
    caster->Update();

    // Write output.
    cmdwriter->Write( caster->GetOutput() );
    cmdwriter->Update();
}

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "mb: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "mb: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "mb: ERROR: " << e.getMessage() << endl;
}

return 0;

}
