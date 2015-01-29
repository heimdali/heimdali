/*
#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkSubtractImageFilter.h>

//#include "cmdreader.hxx"
//#include "cmdwriter.hxx"
#include "H5Cpp.h"

using namespace std;
*/

int main(int argc, char** argv)
{ 

/*
TCLAP::CmdLine cmd("Image soustraction.", ' ', "0.0.0");

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

// image0.h5
TCLAP::UnlabeledValueArg<string> input1("image1", 
    "Second image.",true,"","IMAGE2",cmd);

cmd.parse(argc,argv);

cout << "Je passe par là" << endl;

H5::H5File file("nosuch.h5", H5F_ACC_RDONLY);
*/


/*
// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::VectorImage<PixelType, Dimension> ImageType;


// Command line tool readers.
typedef Heimdali::CmdReader<ImageType> ReaderType;
ReaderType* reader1 = ReaderType::make_cmd_reader( streaming.getValue(),
                                                   input0.getValue());

ReaderType* reader2 = ReaderType::make_cmd_reader(streaming.getValue(),
                                                  input1.getValue());
// Command line tool writer.
typedef Heimdali::CmdWriter<ImageType> WriterType;
WriterType* cmdwriter = WriterType::make_cmd_writer(output.getValue());

// Substract filter.
typedef itk::SubtractImageFilter <ImageType,ImageType> SubtractImageFilterType;
SubtractImageFilterType::Pointer subtractFilter = SubtractImageFilterType::New ();

size_t iregionmax = 1E+06;
for (size_t iregion=0 ; iregion<iregionmax ; iregion++) {
    // Read input.
    reader1->next_iteration();
    reader2->next_iteration();
    if (reader1->is_complete()) break;

    // Substract images.
    subtractFilter->SetInput1( reader1->GetOutput() );
    subtractFilter->SetInput2( reader2->GetOutput() );

    // Write output.
    cmdwriter->Write( subtractFilter->GetOutput() );
    cmdwriter->Update();
}
*/

}
