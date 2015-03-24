#include <iostream>
#include <fstream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkArray.h>
#include <itkINRImageIOFactory.h>
#include <itkStatisticsImageFilter.h>
#include <itkINRImageIO.h>
#include <itkHDF5ImageIO.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

#include "itkExtractImageFilter.h"

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

unsigned int ZD=2, YD=1, XD=0;

int main(int argc, char** argv)
{
    try {

    // Parse comamnd line options and arguments.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
        
    TCLAP::CmdLine parser("Extract image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<unsigned int> ixValue("","iz", "First column",false,0,"IZ",parser);
    TCLAP::ValueArg<unsigned int> iyValue("","iy", "First rows",false,0,"IY",parser);
    TCLAP::ValueArg<unsigned int> izValue("","ix", "First plane",false,0,"IX",parser);
    TCLAP::ValueArg<unsigned int> ivValue("","iv", "First component",false,0,"IV",parser);

    // -z -y -x
    TCLAP::ValueArg<unsigned int> zValue("z","nplanes", "Number of planes",false,0,"NZ", parser);
    TCLAP::ValueArg<unsigned int> yValue("y","nrows", "Number of rows",false,0,"NY", parser);
    TCLAP::ValueArg<unsigned int> xValue("x","ncolumns", "Number of columns",false,0,"NX", parser);
    TCLAP::ValueArg<unsigned int> vValue("v","ncomponent", "Number of components",false,0,"NV", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    VectorImageType::Pointer image;

    // Read image size
    itk::ImageIOBase::Pointer io = Heimdali::open_from_stdin_or_file(inputFilename);
    unsigned int SZ = io->GetDimensions(ZD);
    unsigned int SY = io->GetDimensions(YD);
    unsigned int SX = io->GetDimensions(XD);
    unsigned int SV = io->GetNumberOfComponents();

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

    // Readers
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename, IZ,IY,NZ,NY);
    cmdreader->next_iteration(io);
    cmdreader->Update();

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

    // Extract region
    typedef itk::ExtractImageFilter< VectorImageType, VectorImageType > ExtractFilterType;
    ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( cmdreader->GetOutput() );
    extract->SetExtractionRegion( region );

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

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);
    cmdwriter->Write(composer->GetOutput());
    cmdwriter->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
