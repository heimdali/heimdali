#include <tclap/CmdLine.h>

#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkHDF5ImageIO.h"
#include "itkChangeRegionImageFilter.h"
#include <itkHDF5ImageIO.h>
#include "itkExtractImageFilter.h"

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdhelper.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/version.hxx"

#define MELG(pixeltype)  melg<pixeltype>(inputFilename, outputFilename, io, IZ,IY,IX, OZ,OY,OX, NZ,NY,NX)

using namespace std;

unsigned int ZD=2, YD=1, XD=0;

template <typename PixelType>
typename itk::VectorImage<PixelType, Heimdali::ImageDimension>::SizeType
read_image_out_size(string outputFilename)
{
    typename itk::VectorImage<PixelType, Heimdali::ImageDimension>::SizeType size;

    // Create reader.
    typedef itk::VectorImage<PixelType, Heimdali::ImageDimension> ImageType;
    typedef itk::ImageFileReader< ImageType>  ReaderType;
    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(outputFilename);

    // Read image size only.
    reader->UpdateOutputInformation();

    size[ZD] = reader->GetImageIO()->GetDimensions(ZD);
    size[YD] = reader->GetImageIO()->GetDimensions(YD);
    size[XD] = reader->GetImageIO()->GetDimensions(XD);

    return size;
}

template <typename PixelType>
void
melg(string inputFilename, string outputFilename, itk::ImageIOBase::Pointer io,
     unsigned int IZ, unsigned int IY, unsigned int IX,
     unsigned int OZ, unsigned int OY, unsigned int OX,
     unsigned int NZ, unsigned int NY, unsigned int NX)
{
    typedef itk::VectorImage<PixelType, Heimdali::ImageDimension> ImageType;

    // Set requested region.
    typename ImageType::IndexType index;
    typename ImageType::SizeType size;
    index[ZD] = IZ;
    index[YD] = IY;
    index[XD] = IX;
    size[ZD] = NZ;
    size[YD] = NY;
    size[XD] = NX;
    typedef itk::ImageRegion<Heimdali::ImageDimension> RegionType;
    RegionType region;
    region.SetIndex(index);
    region.SetSize(size);

    // Create reader
    typedef Heimdali::CmdReader<ImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename, NZ,NY,IZ,IY);
    cmdreader->next_iteration(io);
    cmdreader->Update();

    // Extract region
    typedef itk::ExtractImageFilter<ImageType,ImageType > ExtractFilterType;
    typename ExtractFilterType::Pointer extract = ExtractFilterType::New();
    extract->SetInput( cmdreader->GetOutput() );
    extract->SetExtractionRegion( region );
    extract->Update();

    // IMAGE-OUT largest possible region.
    index[ZD] = 0;
    index[YD] = 0;
    index[XD] = 0;
    typename ImageType::RegionType lregion;
    lregion.SetIndex(index);
    lregion.SetSize(read_image_out_size<PixelType>(outputFilename));

    // IMAGE-OUT buffered region.
    typename ImageType::RegionType bregion;
    index[ZD] = OZ;
    index[YD] = OY;
    index[XD] = OX;
    size[ZD] = NZ;
    size[YD] = NY;
    size[XD] = NX;
    bregion.SetIndex(index);
    bregion.SetSize(size);

    // IMAGE-OUT Origin.
    typename ImageType::PointType origin;
    origin[ZD] = 0;
    origin[YD] = 0;
    origin[XD] = 0;

    // Change region filter
    typedef itk::ChangeRegionImageFilter<ImageType> ChangeRegionType;
    typename ChangeRegionType::Pointer changeRegion;
    changeRegion = ChangeRegionType::New();
    changeRegion->SetLargestPossibleRegion(lregion);
    changeRegion->SetBufferedRegion(bregion);
    changeRegion->SetOrigin(origin);
    changeRegion->SetInput( extract->GetOutput() );

    // writer.
    itk::HDF5ImageIO::Pointer HDF5io = itk::HDF5ImageIO::New();
    HDF5io->PastingToExistingFile();
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetImageIO(HDF5io);
    writer->SetFileName(outputFilename);

    // ioregion
    itk::ImageIORegion ioregion(Heimdali::ImageDimension);
    ioregion.SetIndex(ZD,OZ);
    ioregion.SetIndex(YD,OY);
    ioregion.SetIndex(XD,OX);
    ioregion.SetSize(ZD,NZ);
    ioregion.SetSize(YD,NY);
    ioregion.SetSize(XD,NX);
    writer->SetIORegion(ioregion);

    writer->SetInput(changeRegion->GetOutput());
    writer->Update();
}

int main(int argc, char** argv)
{
    try {

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

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

    unsigned int NZ = idzValue.getValue();
    unsigned int NY = idyValue.getValue();
    unsigned int NX = idxValue.getValue();

    unsigned int IZ = iziValue.getValue();
    unsigned int IY = iyiValue.getValue();
    unsigned int IX = ixiValue.getValue();

    unsigned int OZ = izoValue.getValue();
    unsigned int OY = iyoValue.getValue();
    unsigned int OX = ixoValue.getValue();

    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();

    //////////////////////////////////////////////////////////////////////////
    // Read input image informations
    //////////////////////////////////////////////////////////////////////////

    itk::ImageIOBase::Pointer io = Heimdali::open_from_stdin_or_file(inputFilename);

    if (NZ==0) {
         Heimdali::read_information(io, "nplanes", NZ);
    }

    if (NY==0) {
         Heimdali::read_information(io, "nrows", NY);
    }

    if (NX==0) {
         Heimdali::read_information(io, "ncolumns", NX);
    }

    //////////////////////////////////////////////////////////////////////////
    // Call templated melg function
    //////////////////////////////////////////////////////////////////////////

    ostringstream error_msg;
    switch (io->GetComponentType()) {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Pixel component type is unknown";
            throw(Heimdali::Exception(error_msg.str()));
            break;
        case itk::ImageIOBase::FLOAT:
            MELG(float);
            break;
        case itk::ImageIOBase::DOUBLE:
            MELG(double);
            break;
        case itk::ImageIOBase::UCHAR:
            MELG(unsigned char);
            break;
        case itk::ImageIOBase::USHORT:
            MELG(unsigned short);
            break;
        case itk::ImageIOBase::UINT:
            MELG(unsigned int);
            break;
        default:
            error_msg
                << "Expected pixel component type to be "
                << "FLOAT, DOUBLE, UCHAR, USHORT or UINT"
                << "but, got " << io->GetComponentTypeAsString(io->GetComponentType());
            throw(Heimdali::Exception(error_msg.str()));
            break;
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}

