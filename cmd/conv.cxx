#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include "itkConvolutionImageFilter.h"
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

using namespace std;

int main(int argc, char** argv)
{ 

    try {

    TCLAP::CmdLine parser("Convolution of image with kernel of any size", ' ', HEIMDALI_VERSION);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);

    string inputFilename;
    string kernelFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_in_image_out(filenamesArg, inputFilename, kernelFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;

    // Readers
    typedef itk::ImageFileReader<VectorImageType> ReaderType;
    ReaderType::Pointer reader_input = ReaderType::New();
    ReaderType::Pointer reader_kernel = ReaderType::New();
    reader_input->SetFileName(inputFilename);
    reader_kernel->SetFileName(kernelFilename);
    reader_input->Update();
    reader_kernel->Update();

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer_input = IndexerType::New();
    IndexerType::Pointer indexer_kernel = IndexerType::New();

    // convolutioner
    typedef itk::ConvolutionImageFilter<ScalarImageType> ConvolutionerType;
    ConvolutionerType::Pointer convolutioner = ConvolutionerType::New();

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    ComposerType::Pointer composer = ComposerType::New();

    indexer_input->SetInput(reader_input->GetOutput());
    indexer_kernel->SetInput(reader_kernel->GetOutput());

    for (unsigned int componentIndex = 0 ;
                      componentIndex < reader_input->GetImageIO()->GetNumberOfComponents() ;
                      componentIndex++)
    {
        indexer_input->SetIndex(componentIndex);
        //indexer_kernel->SetIndex(componentIndex);
        indexer_kernel->SetIndex(0);

        convolutioner->SetInput(indexer_input->GetOutput());
        convolutioner->SetKernelImage(indexer_kernel->GetOutput());
        convolutioner->Update();
        convolutioner->Modified();

        duplicator->SetInputImage(convolutioner->GetOutput());
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
