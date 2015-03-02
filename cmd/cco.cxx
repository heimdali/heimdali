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
#include "itkNthElementImageAdaptor.h"

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"

// http://insight-developers.itk.narkive.com/pprO3gTs/vectorimagetoimageadaptor-dysfunctional
// http://www.itk.org/Wiki/ITK/Examples/ImageProcessing/ProcessingNthImageElement

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
write_output(ReaderType* reader, string outputFilename, double fixed_point_divider)
{
    bool convert_floating_to_fixed_point = is_hdf5_or_inrimage(outputFilename) 
                                        && fixed_point_divider != 0;

    // MultiplyImageFilter.
    typedef itk::MultiplyImageFilter<VectorInputImageType, ScalarInputImageType,
                                     VectorInputImageType> MultiplierType;
    MultiplierType::Pointer multiplier = MultiplierType::New();

    // Cast image from double to output type.
    typedef itk::VectorImage<OutputPixelType, ImageDimension> VectorOutputImageType;
    typedef itk::Image<OutputPixelType, ImageDimension> ScalarOutputImageType;
    typedef itk::CastImageFilter<VectorInputImageType, VectorOutputImageType > CastFilterType;
    typename CastFilterType::Pointer caster = CastFilterType::New();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorOutputImageType> WriterType;
    WriterType* writer = WriterType::make_cmd_writer(outputFilename);

    // ProcessingNthImageElement
    typedef itk::NthElementImageAdaptor<VectorInputImageType, InputPixelType> ToImageType;
    typename ToImageType::Pointer toImageFilter = ToImageType::New();

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

    // Thresholder
    typedef itk::ThresholdImageFilter<ScalarInputImageType> ThresholderType;
    typename ThresholderType::Pointer thresholder = ThresholderType::New();

    // Duplicator.
    typedef itk::ImageDuplicator<ScalarInputImageType> DuplicatorType;
    typename DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // Image to VectorImage
    typedef itk::ComposeImageFilter<ScalarInputImageType> ToVectorImageType;
    typename ToVectorImageType::Pointer toVectorImage = ToVectorImageType::New();

    typename VectorInputImageType::Pointer vectorImage;

    if (convert_floating_to_fixed_point) {
        multiplier->SetInput1(reader->GetOutput());
        multiplier->SetConstant2(fixed_point_divider);

        toImageFilter->SetImage(multiplier->GetOutput());
        for (unsigned int ic = 0 ; ic < reader->get_sc() ; ++ic)
        {
            // VectorImage to Image
            toImageFilter->SelectNthElement(ic);
            toImageFilter->Modified();
            toImageFilter->Update();

            for (unsigned int iz=0 ; iz < reader->get_sz() ; iz++) {
                index[2] = iz;
                for (unsigned int iy=0 ; iy < reader->get_sy() ; iy++) {
                    index[1] = iy;
                    for (unsigned int ix=0 ; ix < reader->get_sx() ; ix++) {
                        index[0] = ix;
                        image->SetPixel(index,
                                        toImageFilter->GetPixel(index));
                    }
                }
            }

            // Threshold image
            thresholder->SetInput(image);
            thresholder->ThresholdAbove(fixed_point_divider);
            thresholder->SetOutsideValue(fixed_point_divider);
            thresholder->Update();
            thresholder->Modified();

            // Image to VectorImage
            duplicator->SetInputImage(thresholder->GetOutput());
            duplicator->Update();

            toVectorImage->SetInput(ic, duplicator->GetOutput());
            toVectorImage->Modified();
            toVectorImage->Update();
        }

        vectorImage = toVectorImage->GetOutput();

        caster->SetInput(vectorImage);
        caster->Update();

        writer->Write(caster->GetOutput());
        vectorImage->Initialize();

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
    if ( (! fixed_point) && (! floating_point) )
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
