#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>
#include "itkImageFileWriter.h"
#include <itkMetaDataDictionary.h>
#include <itkMetaDataObject.h>
#include "itkChangeRegionImageFilter.h"
#include "itkArray.h"

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

using namespace std;

unsigned int ZD=2, YD=1, XD=0;

void
read_image_size(
    string inputFilename,
    unsigned int& nz, unsigned int& sy,  
    unsigned int& sx, unsigned int& sc)
{
    itk::ImageIOBase::Pointer io = itk::ImageIOFactory::CreateImageIO(
        inputFilename.c_str(), itk::ImageIOFactory::ReadMode);

    io->SetFileName(inputFilename);
    io->ReadImageInformation();

    nz = io->GetDimensions(ZD);
    sy = io->GetDimensions(YD);
    sx = io->GetDimensions(XD);
    sc = io->GetNumberOfComponents();
}

int
main(int argc, char** argv)
{
    try {

    TCLAP::CmdLine parser("Assemble 2D images into one 3D image", ' ', HEIMDALI_VERSION);
    TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames",
            "Input images",false,"[IMAGE-IN, ...] [IMAGE-OUT]",parser);

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    vector<string> filenames = filenamesArg.getValue();

    if (filenames.size() < 3) {
        ostringstream error_msg;
        error_msg << "At leat 2 input file names and the output file name required";
        throw(TCLAP::ArgException(error_msg.str()));
    }

    vector<string> inputFilenames;
    unsigned int nInputFilenames = filenames.size() - 1; // Last one is ouputFileName
    for (unsigned int ifile = 0 ; ifile < nInputFilenames ; ifile++)
        inputFilenames.push_back( filenames[ifile] );
    string outputFilename = filenames[nInputFilenames];
    string inputFilename;

    // Image.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;

    // Reader.
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    ReaderType::Pointer reader;

    // writer.
    typedef itk::ImageFileWriter<ImageType>  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);


    // Read first file size.
    unsigned int NZ,SY,SX,SC;
    unsigned int ifile = 0;
    read_image_size(inputFilenames[ifile],NZ,SY,SX,SC);

    if (NZ != 1) {
        ostringstream error_msg;
        error_msg << "Input file must have one plane, but first file has "
                  << NZ << " planes.";
        throw(Heimdali::ValueError(error_msg.str()));
    }

    // Check that others files all have the same size.
    for (ifile=1 ; ifile < nInputFilenames ; ifile++) {
        unsigned int nz,sy,sx,sc;
        read_image_size(inputFilenames[ifile],nz,sy,sx,sc);
        bool same_size = nz==NZ && sy==SY && sx==SX && sc==SC;
        if (! same_size) {
            ostringstream error_msg;
            error_msg << "First file has dimensions: (" 
                      << NZ << ", " << SY << ", " << SX << ", " << SC << "), "
                      << "but " << ifile << "-st file has dimensions: ("
                      << nz << ", " << sy << ", " << sx << ", " << sc << ").";
            throw(Heimdali::ValueError(error_msg.str()));
        }
    }

    // Output image plane numbers it number of input image.
    unsigned int SZ = nInputFilenames;

    // Regions
    ImageType::RegionType region;
    ImageType::RegionType lregion;
    ImageType::RegionType bregion;
    ImageType::IndexType index;
    ImageType::SizeType size;

    // Change region
    typedef itk::ChangeRegionImageFilter<ImageType> ChangeRegionType;
    ChangeRegionType::Pointer changeRegion;

    // ioregion
    itk::ImageIORegion ioregion(Dimension);

    // Origin.
    typename ImageType::PointType origin;
    origin[ZD] = 0;
    origin[YD] = 0;
    origin[XD] = 0;

    for (ifile=0 ; ifile < nInputFilenames ; ifile++) {
        inputFilename = inputFilenames[ifile];

        // Read image
        reader = ReaderType::New();
        reader->SetFileName(inputFilename);
        reader->Update();

        // Largest possible region (of the 3D image).
        index[ZD] = 0;
        index[YD] = 0;
        index[XD] = 0;
        size[ZD] = SZ;
        size[YD] = SY;
        size[XD] = SX;
        lregion.SetIndex(index);
        lregion.SetSize(size);

        // Buffered region (in the 3D image).
        unsigned int IZ=ifile, IY=0, IX=0;
        unsigned int NY=SY, NX=SX;
        index[ZD] = IZ;
        index[YD] = IY;
        index[XD] = IX;
        size[ZD] = NZ;
        size[YD] = NY;
        size[XD] = NX;
        bregion.SetIndex(index);
        bregion.SetSize(size);

        // Change region.
        changeRegion = ChangeRegionType::New();
        changeRegion->SetLargestPossibleRegion(lregion);
        changeRegion->SetBufferedRegion(bregion);
        changeRegion->SetOrigin(origin);
        changeRegion->SetInput( reader->GetOutput() );
        changeRegion->Update();

        // Set ioregion.
        ioregion.SetIndex(ZD,IZ);
        ioregion.SetIndex(YD,IY);
        ioregion.SetIndex(XD,IX);
        ioregion.SetSize(ZD,NZ);
        ioregion.SetSize(YD,NY);
        ioregion.SetSize(XD,SX);

        // Write to file.
        writer->SetInput(changeRegion->GetOutput());
        writer->SetIORegion(ioregion);
        writer->Update();

        // Force objects to close.
        reader = typename ReaderType::Pointer();
        changeRegion = ChangeRegionType::Pointer();
    }

    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "asm: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::NotImplementedError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }
}
