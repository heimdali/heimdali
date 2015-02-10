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
#include "itkImageFileWriter.h"

#include "itkExtractImageFilter.h"

#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{
    cout << scientific << showpos << setprecision(7) << uppercase;

    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

        
    TCLAP::CmdLine cmd("Extract image subregion",' ', "0.0");

    // -iz -iy -ix
    TCLAP::ValueArg<int> ixValue("k","icolumn", "First column",false,0,"IZ",cmd);
    TCLAP::ValueArg<int> iyValue("j","irow", "First rows",false,0,"IY",cmd);
    TCLAP::ValueArg<int> izValue("i","iplane", "First plane",false,0,"IX",cmd);

    // -z -y -x
    TCLAP::ValueArg<int> zValue("z","nplanes", "Number of planes",false,0,"NZ", cmd);
    TCLAP::ValueArg<int> yValue("y","nrows", "Number of rows",false,0,"NY", cmd);
    TCLAP::ValueArg<int> xValue("x","ncolumns", "Number of columns",false,0,"NX", cmd);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","FILE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","FILE-OUT");
    cmd.add(outputFilenameArg);

    cmd.parse(argc,argv);


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
    itk::ImageIORegion ioRegion(Dimension);
    ImageType::IndexType ImageIndex;
    ImageType::SizeType ImageSize;


    //////////////////////////////////////////////////////////////////////////
    // Read file.
    //////////////////////////////////////////////////////////////////////////

    // Read image size only.
    reader->SetFileName( inputFilenameArg.getValue() );
    reader->Update();

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
    NZ = zValue.getValue()==0 ? SZ-IZ : zValue.getValue();
    NY = yValue.getValue()==0 ? SY-IY : yValue.getValue();
    NX = xValue.getValue()==0 ? SX-IX : xValue.getValue();
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

    ioRegion.SetIndex(2,IZ);
    ioRegion.SetIndex(1,IY);
    ioRegion.SetIndex(0,IX);
    ioRegion.SetSize(2,NZ);
    ioRegion.SetSize(1,NY);
    ioRegion.SetSize(0,NX);

    typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractFilterType;
    ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( reader->GetOutput() );
    extract->SetExtractionRegion( requestedRegion );


    //////////////////////////////////////////////////////////////////////////
    // Write image.
    //////////////////////////////////////////////////////////////////////////

    // Writer.
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilenameArg.getValue());
    writer->SetInput(extract->GetOutput());
    reader->UpdateOutputInformation();
    writer->UpdateOutputInformation();
    writer->Update();

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
