#include <sstream>

#include <tclap/CmdLine.h>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>
#include <itkHDF5ImageIO.h>
#include <itkINRImageIO.h>
#include <itkMultiplyImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkVectorIndexSelectionCastImageFilter.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"

using namespace std;

const unsigned int ImageDimension = 3;
typedef double InputPixelType;
typedef itk::      Image<InputPixelType, ImageDimension> ScalarInputImageType;
typedef itk::VectorImage<InputPixelType, ImageDimension> VectorInputImageType;
typedef Heimdali::CmdReader<VectorInputImageType> ReaderType;

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
write_output(ReaderType* reader, string outputFilename, double fixed_point_divider, bool binary=false)
{
    bool convert_floating_to_fixed_point = is_hdf5_or_inrimage(outputFilename) 
                                        && fixed_point_divider != 0;

    // MultiplyImageFilter.
    typedef itk::MultiplyImageFilter<VectorInputImageType, ScalarInputImageType,
                                     VectorInputImageType> MultiplierType;
    MultiplierType::Pointer multiplier = MultiplierType::New();

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorInputImageType, ScalarInputImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // thresholder
    typedef itk::ThresholdImageFilter<ScalarInputImageType> ThresholderType;
    typename ThresholderType::Pointer thresholder = ThresholderType::New();

    // binary_thresholder
    typedef itk::BinaryThresholdImageFilter<ScalarInputImageType, ScalarInputImageType> 
        BinaryThresholderType;
    typename BinaryThresholderType::Pointer binary_thresholder = BinaryThresholderType::New();

    // duplicator
    typedef itk::ImageDuplicator<ScalarInputImageType> DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarInputImageType> ComposerType;
    typename ComposerType::Pointer composer = ComposerType::New();

    // caster
    typedef itk::VectorImage<OutputPixelType, ImageDimension> VectorOutputImageType;
    typedef itk::CastImageFilter<VectorInputImageType, VectorOutputImageType > CastFilterType;
    typename CastFilterType::Pointer caster = CastFilterType::New();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorOutputImageType> WriterType;
    WriterType* writer = WriterType::make_cmd_writer(outputFilename);

    // Store output of ProcessingNthImageElement
    typedef itk::Image<InputPixelType, ImageDimension> ScalarInputImageType;
    typename ScalarInputImageType::Pointer image = ScalarInputImageType::New();
    typename ScalarInputImageType::IndexType index;
    typename ScalarInputImageType::SizeType size;
    typename ScalarInputImageType::RegionType region;
    index.Fill(0);
    size[0] = reader->get_sx();
    size[1] = reader->get_sy();
    size[2] = reader->get_sz();
    region.SetIndex(index);
    region.SetSize(size);
    image->SetRegions(region);
    image->Allocate();

    if (convert_floating_to_fixed_point) {
        multiplier->SetInput1(reader->GetOutput());
        multiplier->SetConstant2(fixed_point_divider);

        indexer->SetInput(multiplier->GetOutput());

        for (unsigned int ic = 0 ; ic < reader->get_sc() ; ++ic)
        {
            indexer->SetIndex(ic);

            // Threshold image
            if (binary) {
                binary_thresholder->SetInput(indexer->GetOutput());
                binary_thresholder->SetUpperThreshold(127);
                binary_thresholder->SetInsideValue(0);
                binary_thresholder->SetOutsideValue(255);
                binary_thresholder->Update();
                binary_thresholder->Modified();
                duplicator->SetInputImage(binary_thresholder->GetOutput());
            } else {
                thresholder->SetInput(indexer->GetOutput());
                thresholder->ThresholdAbove(fixed_point_divider);
                thresholder->SetOutsideValue(fixed_point_divider);
                thresholder->Update();
                thresholder->Modified();
                duplicator->SetInputImage(thresholder->GetOutput());
            }

            // Image to VectorImage
            duplicator->Update();

            composer->SetInput(ic, duplicator->GetOutput());
            composer->Modified();
            composer->Update();
        }

        caster->SetInput(composer->GetOutput());
        caster->Update();

        writer->Write(caster->GetOutput());

    } else {
        caster->SetInput(reader->GetOutput());
        writer->Write(caster->GetOutput());
    }
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

    // -b
    TCLAP::ValueArg<int> binarySwitch("b","binary","Convert to binary",false,0,"NBITS",cmd);

    // -o
    TCLAP::ValueArg<int> oSwitch("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",cmd);

    // Parse command line.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    cmd.parse(tclap_argv);
    string inputFilename = inputFilenameArg.getValue();
    string outputFilename = outputFilenameArg.getValue();
    int nbytes = oSwitch.getValue();
    bool fixed_point = fixedSwitch.getValue();
    bool floating_point = floatingSwitch.getValue();
    bool binary = binarySwitch.getValue();
    if ( (! fixed_point) && (! floating_point) && (binary == 0))
        fixed_point = true;

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Command line tool readers.
    ReaderType* reader = ReaderType::make_cmd_reader(0, inputFilename);
    reader->convert_fixed_point_to_floating_point_on();
    reader->next_iteration();

    ostringstream error_msg;

    if (fixed_point) {
        switch (nbytes)
        {
        case(1):
            write_output<unsigned char>(reader, outputFilename, 255);
            break;
        case(2):
            write_output<unsigned short>(reader, outputFilename, 65535);
            break;
        case(4):
            write_output<unsigned int>(reader, outputFilename, 4294967295);
            break;
        default:
            error_msg << "Pixel component size must to be 1, 2 or 4 "
                      << "for fixed point number, but got "
                      << nbytes;
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        }

    } else if (floating_point) {
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
    } else if (binary) {
        write_output<unsigned char>(reader, outputFilename, 255, true);
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
