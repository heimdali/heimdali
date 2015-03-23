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
#include "heimdali/version.hxx"

using namespace std;

int main(int argc, char** argv)
{
    cout << scientific << showpos << setprecision(7) << uppercase;

    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

        
    TCLAP::CmdLine cmd("Extract image subregion",' ', HEIMDALI_VERSION);

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
    int SZ,SY,SX, NZ,NY,NX, IZ,IY,IX;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;
    ImageType::Pointer image;

    // Readers
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

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

    // First index to read.
    IZ = izValue.getValue();
    IY = iyValue.getValue();
    IX = ixValue.getValue();

    // Number of plane/column/row read.
    NZ = zValue.getValue()==0 ? SZ-IZ : zValue.getValue();
    NY = yValue.getValue()==0 ? SY-IY : yValue.getValue();
    NX = xValue.getValue()==0 ? SX-IX : xValue.getValue();

    // Create requested region.
    ImageType::IndexType index;
    index[2] = IZ;
    index[1] = IY;
    index[0] = IX;
    ImageType::SizeType size;
    size[2] = NZ;
    size[1] = NY;
    size[0] = NX;
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    typedef itk::ExtractImageFilter< ImageType, ImageType > ExtractFilterType;
    ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( reader->GetOutput() );
    extract->SetExtractionRegion( region );


    //////////////////////////////////////////////////////////////////////////
    // Write image.
    //////////////////////////////////////////////////////////////////////////

    // Writer.
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilenameArg.getValue());
    writer->SetInput(extract->GetOutput());
    writer->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
