#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMeanImageFilter.h>
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

using namespace std;

unsigned int XD=0, YD=1;

int main(int argc, char** argv)
{ 

    try {

    TCLAP::CmdLine parser("Convolution of image by mean filter", ' ', HEIMDALI_VERSION);

    TCLAP::ValueArg<unsigned int> yArg("y","row-size", "Window size",false,3,"Y", parser);
    TCLAP::ValueArg<unsigned int> xArg("x","column-size", "Window size",false,3,"X", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;

    // Readers
    typedef itk::ImageFileReader<VectorImageType> ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);
    reader->Update();

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // convolutioner
    typedef itk::MeanImageFilter<ScalarImageType,ScalarImageType> MeanType;
    MeanType::Pointer mean = MeanType::New();
    MeanType::RadiusType radius;
    radius[XD] = xArg.getValue();
    radius[YD] = yArg.getValue();
    mean->SetRadius(radius);

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    ComposerType::Pointer composer = ComposerType::New();

    indexer->SetInput(reader->GetOutput());

    for (unsigned int componentIndex = 0 ;
                      componentIndex < reader->GetImageIO()->GetNumberOfComponents() ;
                      componentIndex++)
    {
        indexer->SetIndex(componentIndex);

        mean->SetInput(indexer->GetOutput());
        mean->Update();
        mean->Modified();

        duplicator->SetInputImage(mean->GetOutput());
        duplicator->Update();

        composer->SetInput(componentIndex, duplicator->GetOutput());
    }
    composer->Update();

    // Writer
    typedef itk::ImageFileWriter<VectorImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(composer->GetOutput());
    writer->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
