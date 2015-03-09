#include <tclap/CmdLine.h>

#include "itkVectorImage.h"
#include "itkAndImageFilter.h"
#include <itkINRImageIOFactory.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkVectorIndexSelectionCastImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Boolean operation on image",
                      ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

TCLAP::SwitchArg andSwitch("","et", "Boolean and", parser, false);
TCLAP::SwitchArg orSwitch("","ou", "Boolean or", parser, false);
TCLAP::SwitchArg xorSwitch("","xou", "Boolean xor", parser, false);
TCLAP::ValueArg<int> maArg("","ma","And with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> foArg("","fo","Or with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> coArg("","co","Xor with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::SwitchArg invSwitch("","inv", "", parser, false);

vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
parser.parse(tclap_argv);
string inputFilename0;
string inputFilename1;
string outputFilename;
Heimdali::parse_tclap_image_in_image_in_image_out(filenamesArg, inputFilename0,
                                                  inputFilename1, outputFilename);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef unsigned char PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<PixelType, Dimension> ScalarImageType;
typedef itk::VectorImage<PixelType, Dimension> VectorImageType;

// Command line tool readers.
typedef Heimdali::CmdReader<VectorImageType> ReaderType;
ReaderType* reader0 = ReaderType::make_cmd_reader(0, inputFilename0);
ReaderType* reader1 = ReaderType::make_cmd_reader(0, inputFilename1);

// indexer
typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
IndexerType::Pointer indexer0 = IndexerType::New();
IndexerType::Pointer indexer1 = IndexerType::New();

// duplicator
typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
DuplicatorType::Pointer duplicator = DuplicatorType::New();

// composer
typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
ComposerType::Pointer composer = ComposerType::New();

// AndFillter
typedef itk::AndImageFilter<ScalarImageType> AndFilterType;
AndFilterType::Pointer andFilter = AndFilterType::New();

// Command line tool writer.
typedef Heimdali::CmdWriter<VectorImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    reader0->next_iteration();
    reader1->next_iteration();
    if (reader0->is_complete()) break;

    indexer0->SetInput(reader0->GetOutput());
    indexer1->SetInput(reader1->GetOutput());

    for (unsigned int componentIndex = 0 ;
                      componentIndex < reader0->get_sc() ;
                      componentIndex++)
    {
        indexer0->SetIndex(componentIndex);
        indexer1->SetIndex(componentIndex);

        // Binary AND images.
        andFilter->SetInput(0, indexer0->GetOutput());
        andFilter->SetInput(1, indexer1->GetOutput());
        andFilter->Update();

        duplicator->SetInputImage(andFilter->GetOutput());
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
