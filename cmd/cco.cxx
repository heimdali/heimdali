#include <sstream>

#include <tclap/CmdLine.h>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include <itkHDF5ImageIO.h>
#include <itkINRImageIO.h>
#include <itkMultiplyImageFilter.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"

using namespace std;

const unsigned int ImageDimension = 3;
typedef double InputPixelType;
typedef itk::VectorImage<InputPixelType, ImageDimension> InputImageType;
typedef itk::Image<InputPixelType, ImageDimension> ScalarInputImageType;
typedef Heimdali::CmdReader<InputImageType> ReaderType;

bool is_hdf5_or_inrimage(string filename)
{
    const char* s = filename.c_str();

    itk::HDF5ImageIO::Pointer HDF5io = itk::HDF5ImageIO::New();
    if ( HDF5io->CanWriteFile(s) ) return true;

    itk::INRImageIO::Pointer INRio = itk::INRImageIO::New();
    if ( INRio->CanWriteFile(s) ) return true;

    return false;
}

template<typename OutputPixelType>
void
write_output(ReaderType* reader, string outputFilename, int fixed_point_divider)
{
    bool convert_floating_to_fixed_point = is_hdf5_or_inrimage(outputFilename) 
                                        && fixed_point_divider != 0;

    reader->Update();

    // MultiplyImageFilter.
    typedef itk::MultiplyImageFilter<InputImageType, ScalarInputImageType,
                                     InputImageType> MultiplierType;
    MultiplierType::Pointer multiplier = MultiplierType::New();
    if (convert_floating_to_fixed_point) {
        multiplier->SetInput1(reader->GetOutput());
        multiplier->SetConstant2(fixed_point_divider);
    }

    // Cast image from double to output type.
    typedef itk::VectorImage<OutputPixelType, ImageDimension> OutputImageType;
    typedef itk::CastImageFilter<InputImageType, OutputImageType > CastFilterType;
    typename CastFilterType::Pointer caster = CastFilterType::New();
    if (convert_floating_to_fixed_point)
        caster->SetInput(multiplier->GetOutput());
    else
        caster->SetInput(reader->GetOutput());

    // Command line tool writer.
    typedef Heimdali::CmdWriter<OutputImageType> WriterType;
    WriterType* writer = WriterType::make_cmd_writer(outputFilename);
    writer->Write(caster->GetOutput());
    writer->Update();
}

int main(int argc, char *argv[])
{

    try {
    TCLAP::CmdLine cmd("Change pixel type of image",' ', HEIMDALI_VERSION);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"","FILE-IN", cmd);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",false,"","FILE-OUT", cmd);

    // -r
    TCLAP::SwitchArg floatingSwitch("r","floating", "Convert to floating point.", cmd, false);

    // -f
    TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert to fixed point.", cmd, false);

    // -o
    TCLAP::ValueArg<int> oSwitch("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",cmd);

    // Parse command line.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    cmd.parse(tclap_argv);
    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();
    int nbytes = oSwitch.getValue();

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Command line tool readers.
    ReaderType* reader = ReaderType::make_cmd_reader(0, inputFilename);
    reader->convert_fixed_point_to_floating_point_on();
    reader->next_iteration();

    ostringstream error_msg;

    if (floatingSwitch.getValue()) {
        switch (nbytes)
        {
        case(4):
            write_output<float>(reader, outputFilename, 0);
            break;
        case(8):
            write_output<double>(reader, outputFilename, 0);
            break;
        default:
            error_msg << "Pixel component size must to be 4 or 8 "
                      << "for floating point number, but got "
                      << nbytes;
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        }

    } else if (fixedSwitch.getValue()) {
        switch (nbytes)
        {
        case(1):
            write_output<unsigned char>(reader, outputFilename, 255);
            break;
        case(2):
            write_output<unsigned int>(reader, outputFilename, 65535);
            break;
        case(4):
            write_output<unsigned long>(reader, outputFilename, 4294967295);
            break;
        default:
            error_msg << "Pixel component size must to be 1, 2 or 4 "
                      << "for fixed point number, but got "
                      << nbytes;
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        }

    } else {
          ostringstream msg;
          msg << "errror: expected one switch" << endl;
          throw(Heimdali::IOError(msg.str()));
    }

    }

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "cco: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    // Heimdali
    catch (Heimdali::IOError &e) {
        cerr << "cco: ERROR: " << e.getMessage() << endl;
    }
    catch (Heimdali::ValueError &e) {
        cerr << "cco: ERROR: " << e.getMessage() << endl;
    }

  return 0;
}
