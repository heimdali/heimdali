#include <iostream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include "itkImageFileWriter.h"
#include <itkINRImageIOFactory.h>
#include <itkINRImageIO.h>
#include <itkSubtractImageFilter.h>

#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{
    try {

    TCLAP::CmdLine cmd("Fill an image with zero values.", ' ', "0.0.0");

    // image.h5
    TCLAP::UnlabeledValueArg<string> imageArg("image", 
        "First image.",true,"","IMAGE",cmd);

    // -z -y -x -v
    TCLAP::ValueArg<int> nzArg("z","nplanes", "Number of planes",false,1,"NZ", cmd);
    TCLAP::ValueArg<int> nyArg("y","nrows", "Number of rows",false,1,"NY", cmd);
    TCLAP::ValueArg<int> nxArg("x","ncolumns", "Number of columns",false,1,"NX", cmd);
    TCLAP::ValueArg<int> nvArg("v","ncomponents", "Number of components",false,1,"NV", cmd);
    // used for now, but required for old INRimage raz compatibility.
    TCLAP::SwitchArg floatArg("r","float", "Float format.", cmd);

    cmd.parse(argc,argv);


    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 


    //////////////////////////////////////////////////////////////////////////
    // Does the image file exists?
    //////////////////////////////////////////////////////////////////////////


    ifstream f(imageArg.getValue().c_str());
    bool image_exists = f.good();
    f.close();


    //////////////////////////////////////////////////////////////////////////
    // Create image.
    //////////////////////////////////////////////////////////////////////////


    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;
    ImageType::Pointer image;

    if (image_exists) {
        // Reader type.
        typedef itk::ImageFileReader< ImageType >  ReaderType;
        ReaderType::Pointer reader = ReaderType::New();

        // Read images
        reader->SetFileName( imageArg.getValue() );
        reader->Update();
        image = reader->GetOutput();
    } else {
        // Use dimensions from command line.
        int nz = nzArg.getValue();
        int ny = nyArg.getValue();
        int nx = nxArg.getValue();
        int nv = nvArg.getValue();

        // Number of pixels.
        ImageType::SizeType size;
        size[0] = nx;
        size[1] = ny;
        size[2] = nz;

        // Region to allocate.
        ImageType::IndexType index;
        index.Fill(0);
        ImageType::RegionType region;
        region.SetIndex(index);
        region.SetSize(size);

        // Allocate empty image.
        image = ImageType::New();
        image->SetRegions(region);
        image->SetNumberOfComponentsPerPixel(nv);
        image->Allocate();
    }


    //////////////////////////////////////////////////////////////////////////
    // Set image to zero.
    //////////////////////////////////////////////////////////////////////////


    ImageType::PixelType pixel;
    pixel.SetSize(image->GetNumberOfComponentsPerPixel());
    pixel.Fill(0.);
    image->FillBuffer(pixel);


    //////////////////////////////////////////////////////////////////////////
    // Write output.
    //////////////////////////////////////////////////////////////////////////


    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(imageArg.getValue());
    writer->SetInput(image);
    writer->Update();

    }

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "raz: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    // Input/output.
    catch (Heimdali::IOError &e) {
        cerr << "raz: ERROR: " << e.getMessage() << endl;
    }

    // Value.
    catch (Heimdali::ValueError &e) {
        cerr << "raz: ERROR: " << e.getMessage() << endl;
    }
}

