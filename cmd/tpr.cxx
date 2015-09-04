#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkArray.h>
#include <itkINRImageIOFactory.h>
#include <itkStatisticsImageFilter.h>
#include <itkINRImageIO.h>
#include <itkHDF5ImageIO.h>

#include "heimdali/error.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

int main(int argc, char** argv)
{
    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

        
    TCLAP::CmdLine parser("Print the pixel values of a image subregion",' ', HEIMDALI_VERSION);

    // -iz -iy -ix
    TCLAP::ValueArg<unsigned int> izArg("","iz", "First plane",false,0,"IZ",parser);
    TCLAP::ValueArg<unsigned int> iyArg("","iy", "First row",false,0,"IY",parser);
    TCLAP::ValueArg<unsigned int> ivArg("","ix", "First value",false,0,"IX",parser);

    // -z -y -x
    TCLAP::ValueArg<unsigned int> zArg("z","nplanes", "Number of planes",false,0,"NZ", parser);
    TCLAP::ValueArg<unsigned int> yArg("y","nrows", "Number of rows",false,0,"NY", parser);
    TCLAP::ValueArg<unsigned int> vArg("x","ncolumns", "Number of values",false,0,"NX", parser);

    // -c, -cz
    TCLAP::SwitchArg cSwitch("c","", "Compact format, suppress line and plane number printing", parser);
    TCLAP::SwitchArg czSwitch("","cz", "Compact format, but with plane number printing", parser);
    TCLAP::SwitchArg zeroSwitch("","from-zero", "Count indices from 0.", parser);

    // -l 
    TCLAP::ValueArg<unsigned int> lArg("l","nvalues", "Maximal number of value printed per console row (0 for full line)",false,5,"NVALUES", parser);

    // -f
    TCLAP::ValueArg<string> fArg("f","format", "printf format to print values",false,"%g","FORMAT", parser);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",false,"","FILE-IN", parser);

    parser.parse(tclap_argv);
    string inputFilename = inputFilenameArg.getValue();

    if (cSwitch.getValue() && czSwitch.getValue()) {
        ostringstream error_msg;
        error_msg << "Flags -c and -cz are incompatible";
        throw(TCLAP::ArgException(error_msg.str()));
    }

    int offset = zeroSwitch.getValue() ? 0 : 1;

    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Pixel
    typedef double PixelType;

    // Images
    const unsigned int Dimension = 3;
    unsigned int SZ,SY,SX,SC, NZ,NY,NX,NV, IZ,IY,IX,IV;
    typedef itk::VectorImage<PixelType, Dimension> ImageType;
    ImageType::Pointer image;

    // Read image information.
    itk::ImageIOBase::Pointer io = Heimdali::open_from_stdin_or_file(inputFilename);
    bool is_uchar = (io->GetComponentType() == itk::ImageIOBase::UCHAR);

    // Set output format.
    string fmt = is_uchar && ! fArg.isSet() ? "%d" : fArg.getValue();
    bool is_fixed_point = (fmt=="%d" || fmt=="%u");

    // Readers
    typedef Heimdali::CmdReader<ImageType> CmdReaderType;
    CmdReaderType* cmdreader = CmdReaderType::make_cmd_reader(0, inputFilename);
    if (! is_fixed_point)
        cmdreader->convert_fixed_point_to_floating_point_on();
    cmdreader->next_iteration(io);
    cmdreader->Update();

    // Region
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType requestedRegion;
    ImageType::IndexType ImageIndex;
    ImageType::SizeType ImageSize;


    //////////////////////////////////////////////////////////////////////////
    // Read file.
    //////////////////////////////////////////////////////////////////////////

    // Total size.
    SZ = cmdreader->get_sz();
    SY = cmdreader->get_sy();
    SX = cmdreader->get_sx();
    SC = cmdreader->get_sc();

    // First plane to read and print
    if (! zeroSwitch.getValue() && izArg.getValue() == 0) {
        // Problem is that TCLAP default value for izArg assume counting from
        // zero.
        IZ = 0;
    } else {
        IZ = izArg.getValue() - offset;
    }

    // First row to read and print
    if (! zeroSwitch.getValue() && iyArg.getValue() == 0) {
        // Problem is that TCLAP default value for izArg assume counting from
        // zero.
        IY = 0;
    } else {
        IY = iyArg.getValue() - offset;
    }

    // First value in row to print
    if (! zeroSwitch.getValue() && ivArg.getValue() == 0) {
        // Problem is that TCLAP default value for izArg assume counting from
        // zero.
        IV = 0;
    } else {
        IV = ivArg.getValue() - offset;
    }

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

    // Read image.
    image = cmdreader->GetOutput();

    // Print values.
    typedef itk::VariableLengthVector<PixelType> VariableVectorType;
    VariableVectorType value;
    value.SetSize(SC);
    unsigned int nprinted = 0;
    unsigned int nprinted_by_console_line = lArg.getValue();
    if (nprinted_by_console_line == 0)
        nprinted_by_console_line = SX*SC;
    unsigned int iv;
    string fmt_space = fmt + " ";
    string fmt_newline = fmt + "\n";
    for (unsigned int iz=IZ ; iz<IZ+NZ; iz++) { 
        ImageIndex[2] = iz;
        if (czSwitch.getValue())
            cout << "plane " << iz+offset << endl;
        for (unsigned int iy=IY ; iy<IY+NY; iy++) { 
            if (! cSwitch.getValue() && ! czSwitch.getValue())
                cout << "plane " << iz+offset << ", line " << iy+offset << endl;
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
                        if (is_fixed_point)
                            printf(fmt_newline.c_str(), (unsigned int) value[ic]);
                        else
                            printf(fmt_newline.c_str(), value[ic]);
                        nprinted = 0;
                    } else {
                        if (is_fixed_point)
                            printf(fmt_space.c_str(), (unsigned int) value[ic]);
                        else
                            printf(fmt_space.c_str(), value[ic]);
                    }
                }
            }
        }
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
