#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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
    //cout << scientific << showpos << setprecision(7) << uppercase;

    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

        
    TCLAP::CmdLine cmd("Print the pixel values of a image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<unsigned int> izArg("","iz", "First plane",false,0,"IZ",cmd);
    TCLAP::ValueArg<unsigned int> iyArg("","iy", "First row",false,0,"IY",cmd);
    TCLAP::ValueArg<unsigned int> ivArg("","ix", "First value",false,0,"IX",cmd);

    // -z -y -x
    TCLAP::ValueArg<unsigned int> zArg("z","nplanes", "Number of planes",false,0,"NZ", cmd);
    TCLAP::ValueArg<unsigned int> yArg("y","nrows", "Number of rows",false,0,"NY", cmd);
    TCLAP::ValueArg<unsigned int> vArg("x","ncolumns", "Number of values",false,0,"NX", cmd);

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
    unsigned int SZ,SY,SX,SC, NZ,NY,NX,NV, IZ,IY,IX,IV;
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

    // First plane to read and print
    IZ = izArg.getValue();

    // First row to read and print
    IY = iyArg.getValue();

    // First value in row to print
    IV = ivArg.getValue();

    // The row will be read.
    IX = 0;
    NX = SX;

    // Number of planes to print.
    if (zArg.getValue() == 0) {
        NZ = SZ-IZ;
    } else {
        NZ = std::min(zArg.getValue(), SZ-IZ);
    }

    // Number of rows to print.
    if (yArg.getValue() == 0) {
        NY = SY-IY;
    } else {
        NY = std::min(yArg.getValue(), SY-IY);
    }

    // Number of values to print per row.
    if (vArg.getValue() == 0) {
        NV = SX*SC - IV;
    } else {
        NV = std::min(vArg.getValue(), SX*SC - IV);
    }

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

    // Print values.
    typedef itk::VariableLengthVector<PixelType> VariableVectorType;
    VariableVectorType value;
    value.SetSize(SC);
    unsigned int nprinted = 0;
    unsigned int nprinted_by_console_line = 5;
    unsigned int iv;
    for (unsigned int iz=IZ ; iz<IZ+NZ; iz++) { 
        ImageIndex[2] = iz;
        for (unsigned int iy=IY ; iy<IY+NY; iy++) { 
            cout << "plane " << iz << ", line " << iy << endl;
            ImageIndex[1] = iy;
            iv = -1;
            for (unsigned int ix=IX ; ix<IX+NX; ix++) {
                ImageIndex[0] = ix;
                value = image->GetPixel(ImageIndex);
                for (unsigned int ic=0 ; ic<SC ; ic++) {
                    iv++;
                    if (! (IV<=iv && iv<IV+NV) )
                        continue;
                    nprinted++;
                    if (nprinted == nprinted_by_console_line || 
                            (iv==IV+NV-1) ) {
                        cout << value[ic] << endl;
                        nprinted = 0;
                    } else {
                        cout << value[ic] << " ";
                    }
                }
            }
        }
    }


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
