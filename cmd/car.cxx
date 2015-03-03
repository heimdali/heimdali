#include <string>
#include <vector>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkSquareImageFilter.h>
#include <itkVectorImageToImageAdaptor.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Compute square of each pixel", ' ', HEIMDALI_VERSION);
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
VectorImageType::Pointer vectorImage;

// Command line tool readers.
typedef Heimdali::CmdReader<VectorImageType> ReaderType;
ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);

// Command line tool writer.
typedef Heimdali::CmdWriter<VectorImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

// Duplicator.
typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
DuplicatorType::Pointer duplicator = DuplicatorType::New();

// VectorImage to Image
typedef itk::VectorImageToImageAdaptor
    <Heimdali::PixelFloat, Heimdali::ImageDimension> ToImageType;
ToImageType::Pointer toImage = ToImageType::New();

// Square filter.
typedef itk::SquareImageFilter <ToImageType,ScalarImageType> SquarerType;
SquarerType::Pointer squarefilter = SquarerType::New();

// Image to VectorImage
typedef itk::ComposeImageFilter<ScalarImageType> ToVectorImageType;
ToVectorImageType::Pointer toVectorImage = ToVectorImageType::New();

unsigned int iregionmax = 1E+06;
for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader->next_iteration();
    if (cmdreader->is_complete()) break;

    cmdreader->reader()->Update();

    vectorImage = cmdreader->GetOutput();

    toImage->SetImage(vectorImage);

    for (unsigned int ic = 0 ; ic < cmdreader->get_sc() ; ++ic)
    {
        // VectorImage to Image
        toImage->SetExtractComponentIndex(ic);
        toImage->Modified();

        // Square images.
        squarefilter->SetInput(toImage);
        squarefilter->Update();
        squarefilter->Modified();

        // Image to VectorImage
        duplicator->SetInputImage(squarefilter->GetOutput());
        duplicator->Update();

        // Image to VectorImage
        toVectorImage->SetInput(ic, duplicator->GetOutput());
        toVectorImage->Modified();
        toVectorImage->Update();
    }

    // Write output.
    cmdwriter->Write( toVectorImage->GetOutput() );
    cmdwriter->Update();

    vectorImage->Initialize();
}

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "car: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}


// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "car: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "car: ERROR: " << e.getMessage() << endl;
}

return 0;

}
