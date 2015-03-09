#include "itkImageFileWriter.h"
#include <itkINRImageIOFactory.h>

#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

using namespace std;

typedef float PixelType;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Inputs ASCII value into image",
                      ' ', HEIMDALI_VERSION);

// IMAGE-OUT
TCLAP::UnlabeledValueArg<string> inputFilenameArg("IMAGE-OUT", 
    "Output image file name.",false,"","IMAGE-OUT", parser);

// Parse command line.
vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
parser.parse(tclap_argv);
string inputFilename = inputFilenameArg.getValue();

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Read image dimension
cout << "Enter sx, sy, sv, sz: ";
unsigned int sx, sy, sv, sz;
cin >> sx >> sy >> sv >> sz;

cout << "Enter pixel type (0: fixed point, 1: floating point): ";
unsigned int pixeltype;
cin >> pixeltype;
    
unsigned int byte_size;
cout << "Enter byte size (0: float, 1: double): ";
cin >> byte_size;

// Allocate image.
const int Dimension = 3;
typedef itk::VectorImage<PixelType, Dimension> ImageType;
ImageType::IndexType start;
start.Fill(0);
ImageType::SizeType size;
size[2] = sz;
size[1] = sy;
size[0] = sx;
ImageType::RegionType region(start,size);
ImageType::Pointer image = ImageType::New();
image->SetRegions(region);
image->SetVectorLength(sv);
image->Allocate();

// Set image pixel values.
ImageType::IndexType pixelIndex;
typedef itk::VariableLengthVector<double> VariableVectorType;
VariableVectorType variableLengthVector;
variableLengthVector.SetSize(sv);
cout << "Enter pixel values:" << endl;
for (unsigned int iz=0 ; iz < sz ; ++iz) { pixelIndex[2] = iz;
for (unsigned int iy=0 ; iy < sy ; ++iy) { pixelIndex[1] = iy;
for (unsigned int ix=0 ; ix < sx ; ++ix) { pixelIndex[0] = ix;
    for (unsigned int iv=0 ; iv < sv ; ++iv) {
        cin >> variableLengthVector[iv];
    }
    image->SetPixel(pixelIndex, variableLengthVector);
}}}

// Write image.
typedef itk::ImageFileWriter<ImageType>  WriterType;
WriterType::Pointer writer = WriterType::New();
writer->SetFileName(inputFilename);
writer->SetInput(image);
writer->Update();

cout << inputFilename << endl;

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "cim: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "cim: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "cim: ERROR: " << e.getMessage() << endl;
}

return 0;

}
