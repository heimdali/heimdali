#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include <itkComplexToComplexFFTImageFilter.h>
#include <itkFFTWInverseFFTImageFilter.h>
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include <itkVectorIndexSelectionCastImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 
    try {

    TCLAP::CmdLine parser("Inverse FFT on complex image", ' ', HEIMDALI_VERSION);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);
    string inputFilename_re;
    string inputFilename_im;
    string outputFilename_re;
    string outputFilename_im;
    Heimdali::parse_tclap_image_in_image_in_image_out_image_out(
            filenamesArg,
            inputFilename_re, inputFilename_im,
            outputFilename_re, outputFilename_im);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image<PixelType, Dimension> ImageType;
    typedef itk::Image<std::complex<PixelType>, Dimension> ComplexImageType;

    // Readers
    typedef itk::ImageFileReader<ImageType> ReaderType;
    ReaderType::Pointer reader_re = ReaderType::New();
    ReaderType::Pointer reader_im = ReaderType::New();
    reader_re->SetFileName(inputFilename_re);
    reader_im->SetFileName(inputFilename_im);
    reader_re->Update();
    reader_im->Update();

    // complexer
    typedef itk::ComposeImageFilter<ImageType,ComplexImageType> ComplexerType;
    ComplexerType::Pointer complexer = ComplexerType::New();
    complexer->SetInput1(reader_re->GetOutput());
    complexer->SetInput2(reader_im->GetOutput());
    complexer->Update();

    // inverse FFT
    typedef itk::FFTWInverseFFTImageFilter<ComplexImageType, ImageType> invFFTType;
    invFFTType::Pointer invFFTFilter = invFFTType::New();
    invFFTFilter->SetInput(complexer->GetOutput());

    // Writers
    typedef itk::ImageFileWriter<ImageType> WriterType;
    WriterType::Pointer writer_re = WriterType::New();
    writer_re->SetFileName(outputFilename_re);
    writer_re->SetInput(invFFTFilter->GetOutput());
    writer_re->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
