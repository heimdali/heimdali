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

using namespace std;

int main(int argc, char** argv)
{
    cout << scientific << showpos << setprecision(7) << uppercase;

    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

        
    TCLAP::CmdLine cmd("Print the pixel values of a image subregion",' ', "0.0");

    // -iz -iy -ix
    TCLAP::ValueArg<int> ixValue("k","icolumn", "First column",false,0,"IZ",cmd);
    TCLAP::ValueArg<int> iyValue("j","irow", "First rows",false,0,"IY",cmd);
    TCLAP::ValueArg<int> izValue("i","iplane", "First plane",false,0,"IX",cmd);

    // -z -y -x
    TCLAP::ValueArg<int> zValue("z","nplanes", "Number of planes",false,0,"NZ", cmd);
    TCLAP::ValueArg<int> yValue("y","nrows", "Number of rows",false,0,"NY", cmd);
    TCLAP::ValueArg<int> xValue("x","ncolumns", "Number of columns",false,0,"NX", cmd);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",true,"IMAGE",cmd);

    cmd.parse(argc,argv);

    vector<string> inputFilenames = inputFilenamesArg.getValue();


    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////


    // Pixel
    typedef float PixelType;

    // Images
    const unsigned int Dimension = 3;
    int SZ,SY,SX,SC, NZ,NY,NX,NC, IZ,IY,IX,IC;
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
    SC = reader->GetImageIO()->GetNumberOfComponents();

    // First index to read.
    IZ = izValue.getValue();
    IY = iyValue.getValue();
    IX = ixValue.getValue();
    IC = 0;

    // Number of plane/column/row read.
    NZ = zValue.getValue()==0 ? SZ+1-ImageIndex[2] : zValue.getValue();
    NY = yValue.getValue()==0 ? SY+1-ImageIndex[1] : yValue.getValue();
    NX = xValue.getValue()==0 ? SX+1-ImageIndex[0] : xValue.getValue();
    NC = SC;

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
    for (int iz=IZ ; iz<IZ+NZ; iz++) { ImageIndex[2] = iz;
    for (int iy=IY ; iy<IY+NY; iy++) { ImageIndex[1] = iy;
    for (int ix=IX ; ix<IX+NX; ix++) { ImageIndex[0] = ix;
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
