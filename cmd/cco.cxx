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

bool write_hdf5_to_stdout(string filename)
{
    return filename == "-" || filename == "";
}

template<typename OutputPixelType>
void
cco(ReaderType* reader, string outputFilename, double fixed_point_divider, bool binary=false)
{
    bool convert_floating_to_fixed_point = 
        (is_hdf5_or_inrimage(outputFilename) || write_hdf5_to_stdout(outputFilename)) &&
        fixed_point_divider != 0;

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
    typedef itk::CastImageFilter<VectorInputImageType, VectorOutputImageType > CasterType;
    typename CasterType::Pointer caster = CasterType::New();

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
    TCLAP::CmdLine parser("Change pixel type of image",' ', HEIMDALI_VERSION);

    // -r
    TCLAP::SwitchArg floatingSwitch("r","floating", "Convert to floating point.", parser, false);

    // -f
    TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert to fixed point.", parser, false);

    // -o
    TCLAP::ValueArg<int> nbytesArg("o","bytes","Number of bytes per pixel component.",false,1,"NBYTES",parser);

    // -b
    TCLAP::ValueArg<int> nbitsArg("b","nbits","Number of bits per pixel component",false,8,"NBITS",parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    // Parse command line.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    Heimdali::PixelTypeArgParser type;
    type.parse(&floatingSwitch, &fixedSwitch, &nbytesArg, &nbitsArg);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Command line tool readers.
    ReaderType* reader = ReaderType::make_cmd_reader(0, inputFilename);
    reader->convert_fixed_point_to_floating_point_on();
    reader->next_iteration();

    ostringstream error_msg;

    if (type.is_fixed_point) {
        switch (type.nbytes)
        {
        case(1):
            cco<unsigned char>(reader, outputFilename, 255, type.is_binary);
            break;
        case(2):
            cco<unsigned short>(reader, outputFilename, 65535);
            break;
        case(4):
            cco<unsigned int>(reader, outputFilename, 4294967295);
            break;
        default:
            error_msg << "Pixel component size must to be 1, 2 or 4 "
                      << "for fixed point number, but got "
                      << type.nbytes;
            throw(Heimdali::Exception(error_msg.str()));
            break;
        }

    } else {
        switch (type.nbytes)
        {
        case(4):
            cco<float>(reader, outputFilename, 0);
            break;
        case(8):
            cco<double>(reader, outputFilename, 0);
            break;
        default:
            error_msg << "Pixel component size must to be 4 or 8 "
                      << "for floating point number, but got "
                      << type.nbytes;
            throw(Heimdali::Exception(error_msg.str()));
            break;
        }
    } 

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
