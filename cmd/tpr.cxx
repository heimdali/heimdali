#include <iostream>
#include <fstream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkArray.h>
#include <itkImageFileReader.h>
#include <itkINRImageIOFactory.h>
#include <itkStatisticsImageFilter.h>
#include <itkINRImageIO.h>
#include <itkHDF5ImageIO.h>

#include "heimdali/error.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{
    cout << scientific << showpos << setprecision(7) << uppercase;

    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

        
    TCLAP::CmdLine cmd("Print the pixel values of a image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<int> ixValue("","iz", "First column",false,0,"IZ",cmd);
    TCLAP::ValueArg<int> iyValue("","iy", "First rows",false,0,"IY",cmd);
    TCLAP::ValueArg<int> izValue("","ix", "First plane",false,0,"IX",cmd);

    // -z -y -x
    TCLAP::ValueArg<int> zValue("z","nplanes", "Number of planes",false,0,"NZ", cmd);
    TCLAP::ValueArg<int> yValue("y","nrows", "Number of rows",false,0,"NY", cmd);
    TCLAP::ValueArg<int> xValue("x","ncolumns", "Number of columns",false,0,"NX", cmd);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",true,"IMAGE",cmd);

    cmd.parse(tclap_argv);

    vector<string> inputFilenames = inputFilenamesArg.getValue();


    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////


    // Pixel
    typedef float PixelType;

    // Images
    const unsigned int Dimension = 3;
    unsigned int SZ,SY,SX, NZ,NY,NX, IZ,IY,IX;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;
    ImageType::Pointer image;

    // Readers
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    // Region
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType requestedRegion;
    ImageType::IndexType ImageIndex;
    ImageType::SizeType ImageSize;


    //////////////////////////////////////////////////////////////////////////
    // Read file.
    //////////////////////////////////////////////////////////////////////////

    // Read image size only.
    reader->SetFileName( inputFilenames[0] );
    reader->UpdateOutputInformation();

    // Total size.
    SZ = reader->GetImageIO()->GetDimensions(2);
    SY = reader->GetImageIO()->GetDimensions(1);
    SX = reader->GetImageIO()->GetDimensions(0);

    // First index to read.
    IZ = izValue.getValue();
    IY = iyValue.getValue();
    IX = ixValue.getValue();

    // Number of plane/column/row read.
    NZ = zValue.getValue()==0 ? SZ-IZ : zValue.getValue();
    NY = yValue.getValue()==0 ? SY-IY : yValue.getValue();
    NX = xValue.getValue()==0 ? SX-IX : xValue.getValue();

    // Create region to read
    ImageIndex[2] = IZ;
    ImageIndex[1] = IY;
    ImageIndex[0] = IX;
    ImageSize[2] = NZ;
    ImageSize[1] = NY;
    ImageSize[0] = NX;
    requestedRegion.SetIndex(ImageIndex);
    requestedRegion.SetSize(ImageSize);

    // Read requested region.
    reader->GetOutput()->SetRequestedRegion(requestedRegion);
    reader->UpdateOutputInformation();

    // Read image.
    reader->Update();
    image = reader->GetOutput();

    // print values
    for (unsigned int iz=IZ ; iz<IZ+NZ; iz++) { ImageIndex[2] = iz;
    for (unsigned int iy=IY ; iy<IY+NY; iy++) { ImageIndex[1] = iy;
    for (unsigned int ix=IX ; ix<IX+NX; ix++) { ImageIndex[0] = ix;
        cout << image->GetPixel(ImageIndex) << endl;
    }}}


    //////////////////////////////////////////////////////////////////////////
    // End of 'try' block.
    //////////////////////////////////////////////////////////////////////////

    } 

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "ical: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }


    // Input/output.
    catch (Heimdali::IOError &e) {
        cerr << "ical: ERROR: " << e.getMessage() << endl;
    }

    return 0;
}
