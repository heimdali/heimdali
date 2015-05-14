#include <iostream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkINRImageIOFactory.h>
#include <itkINRImageIO.h>
#include <itkSubtractImageFilter.h>

#include "heimdali/error.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

template<typename ImageType>
void
set_image_to_zero(typename ImageType::Pointer image)
{
    typename ImageType::PixelType pixel;
    pixel.SetSize(image->GetNumberOfComponentsPerPixel());
    pixel.Fill(0.);
    image->FillBuffer(pixel);
}

template<typename ImageType>
void
write_image(typename ImageType::Pointer image, string filename)
{
    typedef Heimdali::CmdWriter<ImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(filename);
    cmdwriter->Write(image);
    cmdwriter->Update();
}

template<typename PixelType>
void
new_image(unsigned int nz, unsigned int ny, unsigned int nx, 
             unsigned int nv, string outputFilename)
{
    // Image type.
    typedef itk::VectorImage<PixelType, 3> ImageType;
    typename ImageType::Pointer image;

    // Number of pixels.
    typename ImageType::SizeType size;
    size[0] = nx;
    size[1] = ny;
    size[2] = nz;

    // Region to allocate.
    typename ImageType::IndexType index;
    index.Fill(0);
    typename ImageType::RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    // Allocate empty image.
    image = ImageType::New();
    image->SetRegions(region);
    image->SetVectorLength(nv);
    image->Allocate();

    set_image_to_zero<ImageType>(image);
    write_image<ImageType>(image,outputFilename);
}

template<typename PixelType>
typename itk::VectorImage<PixelType, 3>::Pointer
read_image(string inputFilename)
{
    typedef itk::VectorImage<PixelType, 3> ImageType;
    typename ImageType::Pointer image;

    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();

    reader->SetFileName(inputFilename);
    reader->Update();
    image = reader->GetOutput();

    return image;
}

template<typename PixelType>
void existing_image(string inplaceFilename)
{
    typedef itk::VectorImage<PixelType, 3> ImageType;
    typename ImageType::Pointer image = read_image<PixelType>(inplaceFilename);

    set_image_to_zero<ImageType>(image);
    write_image<ImageType>(image,inplaceFilename);
}

int main(int argc, char** argv)
{
    try {

    // Command line options and arguments.
    TCLAP::CmdLine cmd("Fill an image with zero values.", ' ', HEIMDALI_VERSION);

    // image.h5
    TCLAP::UnlabeledValueArg<string> imageArg("image", 
        "First image.",false,"","IMAGE",cmd);

    // -z -y -x -v
    TCLAP::ValueArg<int> nzArg("z","nplanes", "Number of planes",false,1,"NZ", cmd);
    TCLAP::ValueArg<int> nyArg("y","nrows", "Number of rows",false,1,"NY", cmd);
    TCLAP::ValueArg<int> nxArg("x","ncolumns", "Number of columns",false,1,"NX", cmd);
    TCLAP::ValueArg<int> nvArg("v","ncomponents", "Number of components",false,1,"NV", cmd);

    // -o
    TCLAP::ValueArg<int> oArg("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",cmd);

    // -r
    TCLAP::SwitchArg floatingSwitch("r","floating", "Convert to floating point.", cmd, false);

    // -f
    TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert to fixed point.", cmd, false);

    cmd.parse(argc,argv);

    // Register INRimage.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Does the image file exists?
    ifstream f(imageArg.getValue().c_str());
    bool image_exists = f.good();
    f.close();

    // Set pixel type
    bool is_floating = Heimdali::is_floating_point_type(floatingSwitch.isSet(),
                                                        fixedSwitch.isSet());
    itk::ImageIOBase::IOComponentType type = 
        Heimdali::map_to_itk_component_type(is_floating, oArg.getValue());

    // Dimension (only used if new image)
    int nz = nzArg.getValue();
    int ny = nyArg.getValue();
    int nx = nxArg.getValue();
    int nv = nvArg.getValue();

    string filename = imageArg.getValue();

    switch (type)
    {
    case itk::ImageIOBase::FLOAT:
        if (image_exists)
            existing_image<float>(filename);
        else
            new_image<float>(nz,ny,nx,nv,filename);
        break;

    case itk::ImageIOBase::DOUBLE:
        if (image_exists)
            existing_image<double>(filename);
        else
            new_image<double>(nz,ny,nx,nv,filename);
        break;

    case itk::ImageIOBase::UCHAR:
    case itk::ImageIOBase::USHORT:
    case itk::ImageIOBase::UINT:
        if (image_exists)
            existing_image<unsigned int>(filename);
        else
            new_image<unsigned int>(nz,ny,nx,nv,filename);
        break;

    default:
        ostringstream error_msg;
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

