#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>
#include "itkImageFileReader.h"
#include "itkMedianImageFilter.h"
#include <itkImageDuplicator.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include "itkImageFileWriter.h"
#include <itkComposeImageFilter.h>
//#include "itkPermuteAxesImageFilter.h"

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

typedef float PixelType;
const unsigned int ImageDimension = 3;

const unsigned int ZD=2, YD=1, XD=0;

int main(int argc, char** argv)
{ 
    try {

    TCLAP::CmdLine parser("Median filter", ' ', HEIMDALI_VERSION);

    TCLAP::ValueArg<unsigned int> yArg("y","row-size", "Window size",false,3,"Y", parser);
    TCLAP::ValueArg<unsigned int> xArg("x","column-size", "Window size",false,3,"X", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<PixelType, ImageDimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, ImageDimension> VectorImageType;

    // Reader
    typedef itk::ImageFileReader<VectorImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // Median filter
    typedef itk::MedianImageFilter<ScalarImageType, ScalarImageType> MedianerType;
    MedianerType::Pointer medianer = MedianerType::New();
    MedianerType::InputSizeType radius;
    radius[XD] = xArg.getValue();
    radius[YD] = yArg.getValue();
    radius[ZD] = 1;
    medianer->SetRadius(radius);

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    ComposerType::Pointer composer = ComposerType::New();

    // Writer
    typedef itk::ImageFileWriter<VectorImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);

    reader->UpdateOutputInformation();

    indexer->SetInput(reader->GetOutput());

    for (unsigned int componentIndex = 0 ;
                      componentIndex < reader->GetImageIO()->GetNumberOfComponents() ;
                      componentIndex++)
    {
        indexer->SetIndex(componentIndex);

        medianer->SetInput( indexer->GetOutput() );
        medianer->Update();

        duplicator->SetInputImage(medianer->GetOutput());
        duplicator->Update();

        composer->SetInput(componentIndex, duplicator->GetOutput());
    }
    composer->Update();

    // Write output.
    writer->SetInput( composer->GetOutput() );
    writer->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
