#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkMultiplyImageFilter.h>
#include <itkVectorImageToImageAdaptor.h>
#include <itkComposeImageFilter.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine cmd("Multiply two images", ' ', "0.0.0");

// -o output.h5
TCLAP::ValueArg<string> output("o","output", 
    "Output image, instead of standard output",false,"","FILENAME", cmd);

// -streaming N
TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",
    false, 0,"NUMBER_OF_LINES", cmd);

// --force
TCLAP::SwitchArg forceSwitch("v","verbose",
    "ITK HDF5 IO operation are verboses.", cmd);

// image0.h5
TCLAP::UnlabeledValueArg<string> input0("image0", 
    "First image.",true,"","IMAGE1",cmd);

// image1.h5
TCLAP::UnlabeledValueArg<string> input1("image1", 
    "Second image.",true,"","IMAGE2",cmd);

cmd.parse(argc,argv);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef itk::Image<Heimdali::PixelFloat, Heimdali::ImageDimension> ScalarImageType;
typedef itk::VectorImage<Heimdali::PixelFloat, Heimdali::ImageDimension> VectorImageType;
VectorImageType::Pointer vectorImage1;
VectorImageType::Pointer vectorImage2;

// Command line tool readers.
typedef Heimdali::CmdReader<VectorImageType> ReaderType;
ReaderType* cmdreader1 = ReaderType::make_cmd_reader( streaming.getValue(),
                                                      input0.getValue());

ReaderType* cmdreader2 = ReaderType::make_cmd_reader(streaming.getValue(),
                                                     input1.getValue());
// Command line tool writer.
typedef Heimdali::CmdWriter<VectorImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(output.getValue());

// VectorImage to Image
typedef itk::VectorImageToImageAdaptor
    <Heimdali::PixelFloat, Heimdali::ImageDimension> ToImageType;
ToImageType::Pointer toImage1 = ToImageType::New();
ToImageType::Pointer toImage2 = ToImageType::New();

// Multiply filter.
typedef itk::MultiplyImageFilter <ToImageType,ToImageType> MultiplyImageFilterType;
MultiplyImageFilterType::Pointer multiplier = MultiplyImageFilterType::New ();

// Image to VectorImage
typedef itk::ComposeImageFilter<ScalarImageType> ToVectorImageType;
ToVectorImageType::Pointer toVectorImage = ToVectorImageType::New();

size_t iregionmax = 1E+06;
for (size_t iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    cmdreader1->next_iteration();
    cmdreader2->next_iteration();
    if (cmdreader1->is_complete()) break;

    cmdreader1->reader()->Update();
    cmdreader2->reader()->Update();

    vectorImage1 = cmdreader1->GetOutput();
    vectorImage2 = cmdreader2->GetOutput();

    toImage1->SetImage(vectorImage1);
    toImage2->SetImage(vectorImage2);

    for (int ic = 0 ; ic < cmdreader1->get_sc() ; ++ic)
    {
        // VectorImage to Image
        toImage1->SetExtractComponentIndex(ic);
        toImage2->SetExtractComponentIndex(ic);

        // Multiply images.
        multiplier->SetInput1(toImage1);
        multiplier->SetInput2(toImage2);
        multiplier->Update();

        // Image to VectorImage
        toVectorImage->SetInput(ic, multiplier->GetOutput());
        toVectorImage->Update();
    }

    // Write output.
    cmdwriter->Write( toVectorImage->GetOutput() );
    cmdwriter->Update();

    vectorImage1->Initialize();
    vectorImage2->Initialize();
}

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "par: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}


// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "par: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "par: ERROR: " << e.getMessage() << endl;
}

return 0;

}
