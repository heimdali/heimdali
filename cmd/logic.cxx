#include <sstream>

#include <tclap/CmdLine.h>

#include "itkVectorImage.h"
#include <itkINRImageIOFactory.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include "itkAndImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkXorImageFilter.h"
#include "itkNotImageFilter.h"
#include "itkAddImageFilter.h"

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

#define GET_SWITCH(name) \
    if (name ## Switch.getValue()) {\
        nflags++; \
        operation = name; \
    }

#define GET_ARG(name) \
    if (name ## Arg.getValue() != 0) {\
        nflags++; \
        operation = name; \
    }

using namespace std;

// Image type.
typedef unsigned char PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<PixelType, Dimension> ScalarImageType;
typedef itk::VectorImage<PixelType, Dimension> VectorImageType;

typedef Heimdali::CmdReader<VectorImageType> ReaderType;
typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
typedef Heimdali::CmdWriter<VectorImageType> WriterType;

typedef itk::AndImageFilter<ScalarImageType> AndFilterType;
typedef itk::OrImageFilter<ScalarImageType> OrFilterType;
typedef itk::XorImageFilter<ScalarImageType> XorFilterType;
typedef itk::NotImageFilter<ScalarImageType,ScalarImageType> NotFilterType;

template <typename FilterType>
void
compute_and_or_xor(string inputFilename0, string inputFilename1, string outputFilename)
{
    ReaderType* reader0 = ReaderType::make_cmd_reader(0, inputFilename0);
    ReaderType* reader1 = ReaderType::make_cmd_reader(0, inputFilename1);
    IndexerType::Pointer indexer0 = IndexerType::New();
    IndexerType::Pointer indexer1 = IndexerType::New();
    DuplicatorType::Pointer duplicator = DuplicatorType::New();
    ComposerType::Pointer composer = ComposerType::New();
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    typename FilterType::Pointer filter = FilterType::New();

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
            filter->SetInput(0, indexer0->GetOutput());
            filter->SetInput(1, indexer1->GetOutput());
            filter->Update();

            duplicator->SetInputImage(filter->GetOutput());
            duplicator->Update();

            composer->SetInput(componentIndex, duplicator->GetOutput());
        }
        composer->Update();

        // Write output.
        cmdwriter->Write( composer->GetOutput() );
        cmdwriter->Update();
    }
}

void
compute_inv(string inputFilename, string outputFilename)
{
    
    ReaderType* reader = ReaderType::make_cmd_reader(0, inputFilename);
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    unsigned int iregionmax = 1E+06;
    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        reader->next_iteration();
        if (reader->is_complete()) break;

        // todo

        // Write output.
        cmdwriter->Write( reader->GetOutput() );
        cmdwriter->Update();
    }
}


int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Boolean operation on image",
                      ' ', HEIMDALI_VERSION);

TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"IMAGES",parser);

TCLAP::SwitchArg ANDSwitch("","et", "Boolean and", parser, false);
TCLAP::SwitchArg ORSwitch("","ou", "Boolean or", parser, false);
TCLAP::SwitchArg XORSwitch("","xou", "Boolean xor", parser, false);
TCLAP::ValueArg<int> MAArg("","ma","And with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> FOArg("","fo","Or with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> COArg("","co","Xor with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::SwitchArg INVSwitch("","inv", "inv", parser, false);

vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
parser.parse(tclap_argv);

// Set nflags and operation.
enum Operation {AND, OR, XOR, MA, FO, CO, INV} operation;
int nflags = 0;
GET_SWITCH(AND);
GET_SWITCH(OR);
GET_SWITCH(XOR);
GET_ARG(MA);
GET_ARG(FO);
GET_ARG(CO);
GET_SWITCH(INV);

// One and only one flag is allowed.
ostringstream error_msg;
if (nflags != 1) {
    error_msg
        << "Expected one and only one flag -et, -ou, "
        << "-xou, -ma, -fo, -co or -inv, but got: "
        << nflags;
    throw(TCLAP::ArgException(error_msg.str()));
}

// Get filenames
string inputFilename0;
string inputFilename1;
string outputFilename;
vector<string> filenames = filenamesArg.getValue();
if (operation==AND || operation==OR || operation==XOR) {
    Heimdali::parse_tclap_image_in_image_in_image_out(
        filenamesArg, inputFilename0, inputFilename1, outputFilename);

} else if (operation==MA || operation==FO || operation==CO || operation==INV) {
    Heimdali::parse_tclap_image_in_image_out(
        filenamesArg, inputFilename0, outputFilename);

} else {
    error_msg
        << "Expected AND, OR, XOR, MA, FO, CO, INV, but got " << operation;
    throw(TCLAP::ArgException(error_msg.str()));
}

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

switch (operation)
{
case (AND):
    compute_and_or_xor<AndFilterType>(inputFilename0, inputFilename1, outputFilename);
    break;

case (OR):
    compute_and_or_xor<OrFilterType>(inputFilename0, inputFilename1, outputFilename);
    break;

case (XOR):
    compute_and_or_xor<XorFilterType>(inputFilename0, inputFilename1, outputFilename);
    break;

case (INV):
    throw(Heimdali::NotImplementedError("INV not yet implemented"));
    break;

case (MA):
    throw(Heimdali::NotImplementedError("MA not yet implemented"));
    break;

case (FO):
    throw(Heimdali::NotImplementedError("FO not yet implemented"));
    break;

case (CO):
    throw(Heimdali::NotImplementedError("CO not yet implemented"));
    break;
}

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "logic: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "logic: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "logic: ERROR: " << e.getMessage() << endl;
}

return 0;

}
