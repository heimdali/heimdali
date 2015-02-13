#include <tclap/CmdLine.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkHDF5ImageIO.h"
#include "itkChangeRegionImageFilter.h"

#include "heimdali/itkhelper.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/version.hxx"

using namespace std;

Heimdali::ImageFloat::SizeType
read_image_out_size(string outputFilename)
{
    int ZD=2, YD=1, XD=0;

    // Create reader.
    typedef itk::ImageFileReader< Heimdali::ImageFloat>  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(outputFilename);

    // Read image size only.
    reader->UpdateOutputInformation();

    Heimdali::ImageFloat::SizeType size;
    size[ZD] = reader->GetImageIO()->GetDimensions(ZD);
    size[YD] = reader->GetImageIO()->GetDimensions(YD);
    size[XD] = reader->GetImageIO()->GetDimensions(XD);

    //int sc = reader->GetImageIO()->GetNumberOfComponents();

    return size;
}

int main(int argc, char** argv)
{
    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
        
    TCLAP::CmdLine cmd("Mix two images, testing on pixel values.",' ', HEIMDALI_VERSION);

    // -ixi -iyi -izi -ivi
    TCLAP::ValueArg<int> iziValue("","izi", "First plane",false,0,"IZI",cmd);
    TCLAP::ValueArg<int> iyiValue("","iyi", "First row",false,0,"IYI",cmd);
    TCLAP::ValueArg<int> ixiValue("","ixi", "First column",false,0,"IXI",cmd);
    TCLAP::ValueArg<int> iviValue("","ivi", "First component",false,0,"IXI",cmd);

    // -ixo -iyo -izo -ivo
    TCLAP::ValueArg<int> izoValue("","izo", "First plane",false,0,"IZO",cmd);
    TCLAP::ValueArg<int> iyoValue("","iyo", "First row",false,0,"IYO",cmd);
    TCLAP::ValueArg<int> ixoValue("","ixo", "First column",false,0,"IXO",cmd);
    TCLAP::ValueArg<int> ivoValue("","ivo", "First component",false,0,"IXO",cmd);

    // -idx -idy -idz -idv
    TCLAP::ValueArg<int> idzValue("","idz", "First plane",false,0,"IDZ",cmd);
    TCLAP::ValueArg<int> idyValue("","idy", "First row",false,0,"IDY",cmd);
    TCLAP::ValueArg<int> idxValue("","idx", "First column",false,0,"IDX",cmd);
    TCLAP::ValueArg<int> idvValue("","idv", "First component",false,0,"IDV",cmd);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","IMAGE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","IMAGE-OUT");
    cmd.add(outputFilenameArg);

    cmd.parse(tclap_argv);

    int ZD=2, YD=1, XD=0;

    // Create reader
    typedef itk::ImageFileReader<Heimdali::ImageFloat>  ReaderType;
    ReaderType::Pointer reader;
    reader = ReaderType::New();
    reader->SetFileName(inputFilenameArg.getValue());

    // Set requested region.
    Heimdali::ImageFloat::IndexType index;
    Heimdali::ImageFloat::SizeType size;
    index[ZD] = iziValue.getValue();
    index[YD] = iyiValue.getValue();
    index[XD] = ixiValue.getValue();
    size[ZD] = idzValue.getValue();
    size[YD] = idyValue.getValue();
    size[XD] = idxValue.getValue();
    typedef itk::ImageRegion<Heimdali::ImageDimension> RegionType;
    RegionType requestedRegion;
    requestedRegion.SetIndex(index);
    requestedRegion.SetSize(size);
    reader->GetOutput()->SetRequestedRegion(requestedRegion);

    reader->Update();
    index[0] = 1;
    index[1] = 1;
    index[2] = 1;

    // IMAGE-OUT largest possible region.
    index[ZD] = 0;
    index[YD] = 0;
    index[XD] = 0;
    Heimdali::ImageFloat::RegionType lregion;
    lregion.SetIndex(index);
    lregion.SetSize(read_image_out_size(outputFilenameArg.getValue()));

    // IMAGE-OUT buffered region.
    Heimdali::ImageFloat::RegionType bregion;
    index[ZD] = izoValue.getValue();
    index[YD] = iyoValue.getValue();
    index[XD] = ixoValue.getValue();
    size[ZD] = idzValue.getValue();
    size[YD] = idyValue.getValue();
    size[XD] = idxValue.getValue();
    bregion.SetIndex(index);
    bregion.SetSize(size);

    // IMAGE-OUT Origin.
    typename Heimdali::ImageFloat::PointType origin;
    origin[ZD] = 0;
    origin[YD] = 0;
    origin[XD] = 0;

    // Change region filter
    typedef itk::ChangeRegionImageFilter<Heimdali::ImageFloat> ChangeRegionType;
    ChangeRegionType::Pointer changeRegion;
    changeRegion = ChangeRegionType::New();
    changeRegion->SetLargestPossibleRegion(lregion);
    changeRegion->SetBufferedRegion(bregion);
    changeRegion->SetOrigin(origin);
    changeRegion->SetInput( reader->GetOutput() );

    // writer.
    typedef itk::ImageFileWriter<Heimdali::ImageFloat>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilenameArg.getValue());

    // ioregion
    itk::ImageIORegion ioregion(Heimdali::ImageDimension);
    ioregion.SetIndex(ZD,izoValue.getValue());
    ioregion.SetIndex(YD,iyoValue.getValue());
    ioregion.SetIndex(XD,ixoValue.getValue());
    ioregion.SetSize(ZD,idzValue.getValue());
    ioregion.SetSize(YD,idyValue.getValue());
    ioregion.SetSize(XD,idxValue.getValue());
    writer->SetIORegion(ioregion);

    writer->SetInput(changeRegion->GetOutput());
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

