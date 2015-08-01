#include "itkImageFileWriter.h"
#include <itkINRImageIOFactory.h>
#include <itkImageIOBase.h>

#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/cmdhelper.hxx"
#include "heimdali/cmdwriter.hxx"

using namespace std;

template<typename PixelType, typename valueType>
void
read_write_image(unsigned int sz, unsigned int sy,
                 unsigned int sx, unsigned int sv,
                 unsigned int fixed_point_multiplier,
                 string inputFilename,
                 string outputFilename,
                 string format)
{
    FILE* inputfile;
    if (inputFilename == "")
        inputfile = NULL;
    else
        inputfile = fopen(inputFilename.c_str(), "r");

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

    valueType value;
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
    int nvalues;
    for (unsigned int iz=0 ; iz < sz ; ++iz) { pixelIndex[2] = iz;
    for (unsigned int iy=0 ; iy < sy ; ++iy) { pixelIndex[1] = iy;
    for (unsigned int ix=0 ; ix < sx ; ++ix) { pixelIndex[0] = ix;
        for (unsigned int iv=0 ; iv < sv ; ++iv) {
            if (inputfile == NULL)
                nvalues = scanf(format.c_str(), &value);
            else
                nvalues = fscanf(inputfile,format.c_str(), &value);
            if (nvalues == 1) {
                variableLengthVector[iv] = (PixelType) (value * fixed_point_multiplier + cast_to_near_int);
            } else {
                throw(Heimdali::Exception("Failed to read input value"));
            }
        }
        image->SetPixel(pixelIndex, variableLengthVector);
    }}}

    // Write image.
    typedef Heimdali::CmdWriter<ImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);
    cmdwriter->Write(image);
    cmdwriter->Update();

    if (inputfile != NULL)
        fclose(inputfile);
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
    TCLAP::ValueArg<int> nbytesArg("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",parser);

    // -r
    TCLAP::SwitchArg floatingSwitch("r","floating", "Convert to floating point.", parser, false);

    // -f
    TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert to fixed point.", parser, false);

    // --format
    TCLAP::ValueArg<string> formatArg("","format", "scan format to read values",false,"%g","FORMAT", parser);

    // 
    TCLAP::ValueArg<string> rdArg("","rd", "Input file",false,"","FILENAME", parser);

    TCLAP::UnlabeledValueArg<string> outputFilenameArg("IMAGE-OUT", 
        "Output image file name.",false,"","IMAGE-OUT", parser);
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);
    string outputFilename = outputFilenameArg.getValue();

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    ostringstream error_msg;

    Heimdali::PixelTypeArgParser typeParser;
    typeParser.parse(&floatingSwitch, &fixedSwitch, &nbytesArg);

    // Set parameters.
    unsigned int sz, sy, sx, sv;
    sz = zArg.getValue();
    sy = yArg.getValue();
    sx = xArg.getValue();
    sv = vArg.getValue();
    itk::ImageIOBase::IOComponentType type = 
        Heimdali::map_to_itk_component_type(
            typeParser.is_floating_point,
            typeParser.nbytes);

    string format;
    if (formatArg.isSet()) {
        format = formatArg.getValue();
    } else {
        format = (type == itk::ImageIOBase::UCHAR) ? "%d" : "%g";
    }

    string inputFilename = rdArg.getValue();

    // Write image.
    switch (type)
    {
    case itk::ImageIOBase::FLOAT:
        read_write_image<float,float>(sz,sy,sx,sv,1,inputFilename,outputFilename,format);
        break;
    case itk::ImageIOBase::DOUBLE:
        read_write_image<double,double>(sz,sy,sx,sv,1,inputFilename,outputFilename,format);
        break;
    case itk::ImageIOBase::UCHAR:
        read_write_image<unsigned char,int>(sz,sy,sx,sv,1,inputFilename,outputFilename,format);
        break;
    case itk::ImageIOBase::USHORT:
        read_write_image<unsigned short,float>(sz,sy,sx,sv,65534,inputFilename,outputFilename,format);
        break;
    case itk::ImageIOBase::UINT:
        read_write_image<unsigned int,float>(sz,sy,sx,sv,4294967294,inputFilename,outputFilename,format);
        break;
    default:
        error_msg
          << "Expected pixel component type to be"
          << "FLOAT, DOUBLE, UCHAR, USHORT or UINT"
          << "but, got " << itk::ImageIOBase::GetComponentTypeAsString(type);
        throw(Heimdali::Exception(error_msg.str()));
        break;
    }

    } // End of 'try' block.
    
    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
