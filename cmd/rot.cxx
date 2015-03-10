#include <tclap/CmdLine.h>

#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include <itkAffineTransform.h>
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include <itkVectorIndexSelectionCastImageFilter.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

int
main(int argc, char** argv)
{
    try {

    TCLAP::CmdLine parser("z-axis rotation and translation", ' ', HEIMDALI_VERSION);
    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Image type.
    const unsigned int ImageDimension = 3;
    typedef unsigned char PixelType;
    typedef itk::Image< PixelType, ImageDimension > ScalarImageType;
    typedef itk::VectorImage< PixelType, ImageDimension > VectorImageType;

    // reader
    typedef itk::ImageFileReader< VectorImageType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    reader->Update();

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();
    unsigned int componentIndex = 0;
    indexer->SetIndex(componentIndex);
    indexer->SetInput(reader->GetOutput());
    indexer->Update();
    ScalarImageType::Pointer inputImage = indexer->GetOutput();

    // interpolator
    typedef itk::LinearInterpolateImageFunction< ScalarImageType, double > InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    // transform
    typedef itk::AffineTransform< double, ImageDimension > TransformType;
    TransformType::Pointer transform = TransformType::New();
    double angleInDegrees = 45;
    double degreesToRadians = std::atan(1.0) / 45.0;
    double angle = angleInDegrees * degreesToRadians;
    transform->Rotate2D(-angle, false);

    // resampler
    typedef itk::ResampleImageFilter< ScalarImageType, ScalarImageType > ResamplerType;
    ResamplerType::Pointer resampler = ResamplerType::New();
    resampler->SetInterpolator(interpolator);
    resampler->SetDefaultPixelValue(100);
    resampler->SetOutputOrigin(inputImage->GetOrigin());
    resampler->SetOutputSpacing(inputImage->GetSpacing());
    resampler->SetOutputDirection(inputImage->GetDirection());
    resampler->SetSize(inputImage->GetLargestPossibleRegion().GetSize());
    resampler->SetInput(inputImage);
    resampler->SetTransform(transform);

    // writer
    typedef itk::ImageFileWriter< ScalarImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(resampler->GetOutput());
    writer->Update();

    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "rot: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::NotImplementedError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }
}
