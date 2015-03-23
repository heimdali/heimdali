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
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

#include "itkExtractImageFilter.h"

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

using namespace std;

unsigned int ZD=2, YD=1, XD=1;

int main(int argc, char** argv)
{
    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
        
    TCLAP::CmdLine cmd("Extract image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<unsigned int> ixValue("","iz", "First column",false,0,"IZ",cmd);
    TCLAP::ValueArg<unsigned int> iyValue("","iy", "First rows",false,0,"IY",cmd);
    TCLAP::ValueArg<unsigned int> izValue("","ix", "First plane",false,0,"IX",cmd);
    TCLAP::ValueArg<unsigned int> ivValue("","iv", "First component",false,0,"IV",cmd);

    // -z -y -x
    TCLAP::ValueArg<unsigned int> zValue("z","nplanes", "Number of planes",false,0,"NZ", cmd);
    TCLAP::ValueArg<unsigned int> yValue("y","nrows", "Number of rows",false,0,"NY", cmd);
    TCLAP::ValueArg<unsigned int> xValue("x","ncolumns", "Number of columns",false,0,"NX", cmd);
    TCLAP::ValueArg<unsigned int> vValue("v","ncomponent", "Number of components",false,0,"NV", cmd);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","FILE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","FILE-OUT");
    cmd.add(outputFilenameArg);

    cmd.parse(tclap_argv);

    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////

    // Image type
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    VectorImageType::Pointer image;

    // Readers
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    typedef itk::ImageFileReader< VectorImageType >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    //////////////////////////////////////////////////////////////////////////
    // Read file.
    //////////////////////////////////////////////////////////////////////////

    // Read image size only.
    reader->SetFileName( inputFilenameArg.getValue() );
    reader->Update();

    // Total size.
    unsigned int SZ = reader->GetImageIO()->GetDimensions(ZD);
    unsigned int SY = reader->GetImageIO()->GetDimensions(YD);
    unsigned int SX = reader->GetImageIO()->GetDimensions(XD);
    unsigned int SV = reader->GetImageIO()->GetNumberOfComponents();

    // First index to read.
    unsigned int IZ = izValue.getValue();
    unsigned int IY = iyValue.getValue();
    unsigned int IX = ixValue.getValue();
    unsigned int IV = ivValue.getValue();

    // Number of plane/column/row read.
    unsigned int NZ = zValue.getValue()==0 ? SZ-IZ : zValue.getValue();
    unsigned int NY = yValue.getValue()==0 ? SY-IY : yValue.getValue();
    unsigned int NX = xValue.getValue()==0 ? SX-IX : xValue.getValue();
    unsigned int NV = vValue.getValue()==0 ? SV-IV : vValue.getValue();

    // Create requested region.
    VectorImageType::IndexType index;
    index[ZD] = IZ;
    index[YD] = IY;
    index[XD] = IX;
    VectorImageType::SizeType size;
    size[ZD] = NZ;
    size[YD] = NY;
    size[XD] = NX;
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    typedef itk::ExtractImageFilter< VectorImageType, VectorImageType > ExtractFilterType;
    ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( reader->GetOutput() );
    extract->SetExtractionRegion( region );

    //////////////////////////////////////////////////////////////////////////
    // Extract components
    //////////////////////////////////////////////////////////////////////////

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    typename ComposerType::Pointer composer = ComposerType::New();

    extract->Update();
    indexer->SetInput(extract->GetOutput());

    for (unsigned int iv = IV ; iv < IV+NV ; ++iv) {
        indexer->SetIndex(iv);
        indexer->Update();

        duplicator->SetInputImage(indexer->GetOutput());
        duplicator->Update();

        composer->SetInput(iv-IV, duplicator->GetOutput());
        composer->Modified();
        composer->Update();
    }

    //////////////////////////////////////////////////////////////////////////
    // Write image.
    //////////////////////////////////////////////////////////////////////////

    // Writer.
    typedef itk::ImageFileWriter<VectorImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilenameArg.getValue());
    writer->SetInput(composer->GetOutput());
    writer->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
