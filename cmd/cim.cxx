#include "itkImageFileWriter.h"
#include <itkINRImageIOFactory.h>
#include <itkImageIOBase.h>

#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

void
read_image_size(unsigned int& sz, unsigned int& sy, 
                unsigned int& sx, unsigned int& sv)
{
    // Read image dimension
    cout << "Enter sx, sy, sv, sz: ";
    cin >> sx >> sy >> sv >> sz;
}


void
read_pixel_type(itk::ImageIOBase::IOComponentType& type)
{
    cout << "Enter pixel type (0: fixed point, 1: floating point): ";
    const unsigned int FIXED_POINT=0, FLOATING_POINT=1;
    unsigned int representation_type;
    cin >> representation_type;

    ostringstream error_msg;
    unsigned int byte_size;
    switch (representation_type)
    {
    case (FIXED_POINT):
        cout << "Enter byte size (1: unsigned char, 2: unsigned short, 4 unsigned int)";
        cin >> byte_size;
        switch (byte_size)
        {
        case (1):
            type = itk::ImageIOBase::UCHAR;
            break;
        case (2):
            type = itk::ImageIOBase::USHORT;
            break;
        case (4):
            type = itk::ImageIOBase::UINT;
            break;
        default:
            error_msg << "Expected fixed point byte size to be 1 (uchar) "
                      << "2 (ushort) or 4 (uint) but got: " << byte_size;
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        }
        break;
    case (FLOATING_POINT):
        cout << "Enter byte size (4: float, 8: double): ";
        cin >> byte_size;
        switch (byte_size)
        {
        case (4):
            type = itk::ImageIOBase::FLOAT;
            break;
        case (8):
            type = itk::ImageIOBase::DOUBLE;
            break;
        default:
            error_msg << "Expected floating point byte size to be 4 (float) "
                      << "or 8 (double) but got: " << byte_size;
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        }
        break;
    default:
        error_msg << "Pixel must be 0 (fixed point) or 1 (floating point) "
                  << "but got: " << representation_type;
        throw(Heimdali::ValueError(error_msg.str()));
        break;
    }
}

template<typename PixelType>
void
read_write_image(unsigned int sz, unsigned int sy,
                 unsigned int sx, unsigned int sv,
                 unsigned int fixed_point_multiplier,
                 string outputFilename,
                 bool is_interactive)
{
    // Allocate image.
    const int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;
    typename ImageType::IndexType start;
    start.Fill(0);
    typename ImageType::SizeType size;
    size[2] = sz;
    size[1] = sy;
    size[0] = sx;
    typename ImageType::RegionType region(start,size);
    typename ImageType::Pointer image = ImageType::New();
    image->SetRegions(region);
    image->SetVectorLength(sv);
    image->Allocate();

    float value;
    float cast_to_near_int = 0;
    if (fixed_point_multiplier==1)
        cast_to_near_int = 0;
    else
        cast_to_near_int = 0.5;

    // Set image pixel values.
    typename ImageType::IndexType pixelIndex;
    typedef itk::VariableLengthVector<PixelType> VariableVectorType;
    VariableVectorType variableLengthVector;
    variableLengthVector.SetSize(sv);
    if (is_interactive) cout << "Enter pixel values:" << endl;
    for (unsigned int iz=0 ; iz < sz ; ++iz) { pixelIndex[2] = iz;
    for (unsigned int iy=0 ; iy < sy ; ++iy) { pixelIndex[1] = iy;
    for (unsigned int ix=0 ; ix < sx ; ++ix) { pixelIndex[0] = ix;
        for (unsigned int iv=0 ; iv < sv ; ++iv) {
            cin >> value;
            variableLengthVector[iv] = (PixelType) (value * fixed_point_multiplier + cast_to_near_int);
        }
        image->SetPixel(pixelIndex, variableLengthVector);
    }}}

    // Write image.
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(image);
    writer->Update();

    if (is_interactive) cout << outputFilename << endl;
}

int main(int argc, char** argv)
{ 

try {

// Parse command line.
TCLAP::CmdLine parser("Inputs ASCII value into image",
                      ' ', HEIMDALI_VERSION);

// -z -y -x
TCLAP::ValueArg<unsigned int> zArg("z","nplanes", "Number of planes",false,1,"NZ", parser);
TCLAP::ValueArg<unsigned int> yArg("y","nrows", "Number of rows",false,1,"NY", parser);
TCLAP::ValueArg<unsigned int> xArg("x","ncolumns", "Number of values",false,1,"NX", parser);
TCLAP::ValueArg<unsigned int> vArg("v","ncomponents", "Number of pixel components",false,1,"NV", parser);

// -o
TCLAP::ValueArg<int> oArg("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",parser);

// -r
TCLAP::SwitchArg floatingSwitch("r","floating", "Convert to floating point.", parser, false);

// -f
TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert to fixed point.", parser, false);

TCLAP::UnlabeledValueArg<string> outputFilenameArg("IMAGE-OUT", 
    "Output image file name.",false,"","IMAGE-OUT", parser);
vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
parser.parse(tclap_argv);
string outputFilename = outputFilenameArg.getValue();

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Interactive command or not?
bool is_interactive = (!( zArg.isSet() || yArg.isSet() || xArg.isSet() || vArg.isSet()
  || oArg.isSet() || floatingSwitch.isSet() || fixedSwitch.isSet() ));

ostringstream error_msg;

// Fixed point or floating point
bool is_floating_point_type;
if (fixedSwitch.isSet()) {
    if (floatingSwitch.isSet()) 
        throw(TCLAP::ArgException("-r and -f flags are incompatible"));
    is_floating_point_type = false;
} else {
    is_floating_point_type = true;
}

// Set parameters.
unsigned int sz, sy, sx, sv;
itk::ImageIOBase::IOComponentType type;
if (is_interactive) {
    read_image_size(sz, sy, sx, sv);
    read_pixel_type(type);
} else {
    sz = zArg.getValue();
    sy = yArg.getValue();
    sx = xArg.getValue();
    sv = vArg.getValue();
    type = Heimdali::map_to_itk_component_type(is_floating_point_type,
                                               oArg.getValue());
}

// Write image.
switch (type)
{
case itk::ImageIOBase::FLOAT:
    read_write_image<float>(sz,sy,sx,sv,1,outputFilename,is_interactive);
    break;
case itk::ImageIOBase::DOUBLE:
    read_write_image<double>(sz,sy,sx,sv,1,outputFilename,is_interactive);
    break;
case itk::ImageIOBase::UCHAR:
    read_write_image<unsigned char>(sz,sy,sx,sv,255,outputFilename,is_interactive);
    break;
case itk::ImageIOBase::USHORT:
    read_write_image<unsigned short>(sz,sy,sx,sv,65535,outputFilename,is_interactive);
    break;
case itk::ImageIOBase::UINT:
    read_write_image<unsigned int>(sz,sy,sx,sv,4294967295,outputFilename,is_interactive);
    break;
default:
    error_msg
      << "Expected pixel component type to be"
      << "FLOAT, DOUBLE, UCHAR, USHORT or UINT"
      << "but, got " << itk::ImageIOBase::GetComponentTypeAsString(type);
    throw(Heimdali::ValueError(error_msg.str()));
    break;
}

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "cim: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

catch (Heimdali::IOError &e) {
    cerr << "cim: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::ValueError &e) {
    cerr << "cim: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "cim: ERROR: " << e.getMessage() << endl;
}

return 0;

}
