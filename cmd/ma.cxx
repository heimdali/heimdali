#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include <itkMagnitudeAndPhaseToComplexImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

TCLAP::CmdLine parser("Convert complex image from (magnitude, phase) to (real, imaginary)", ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
string inputFilename_mod;
string inputFilename_phs;
string outputFilename_re;
string outputFilename_im;
Heimdali::parse_tclap_image_in_image_in_image_out_image_out(filenamesArg,
                                                   inputFilename_mod, inputFilename_phs,
                                                   outputFilename_re, outputFilename_im);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<PixelType, Dimension> VectorImageType;
typedef itk::Image<std::complex<float>, Dimension> ComplexVectorImageType;

// Readers
typedef itk::ImageFileReader<VectorImageType> ReaderType;
ReaderType::Pointer reader_mod = ReaderType::New();
ReaderType::Pointer reader_phs = ReaderType::New();
reader_mod->SetFileName(inputFilename_mod);
reader_phs->SetFileName(inputFilename_phs);
reader_mod->Update();
reader_phs->Update();

// complexer
typedef itk::MagnitudeAndPhaseToComplexImageFilter<VectorImageType, VectorImageType, ComplexVectorImageType> ComplexerType;
ComplexerType::Pointer complexer = ComplexerType::New();
complexer->SetInput1(reader_mod->GetOutput());
complexer->SetInput2(reader_phs->GetOutput());

// Extract the real part
typedef itk::ComplexToRealImageFilter<ComplexVectorImageType, VectorImageType> RealFilterType;
RealFilterType::Pointer refilter = RealFilterType::New();
refilter->SetInput(complexer->GetOutput());
refilter->Update();

// Extract the imaginary part
typedef itk::ComplexToImaginaryImageFilter<ComplexVectorImageType, VectorImageType> ImaginaryFilterType;
ImaginaryFilterType::Pointer imfilter = ImaginaryFilterType::New();
imfilter->SetInput(complexer->GetOutput());
imfilter->Update();

// Writers
typedef itk::ImageFileWriter<VectorImageType> WriterType;
WriterType::Pointer writer_re = WriterType::New();
WriterType::Pointer writer_im = WriterType::New();
writer_re->SetFileName(outputFilename_re);
writer_im->SetFileName(outputFilename_im);
writer_re->SetInput(refilter->GetOutput());
writer_im->SetInput(imfilter->GetOutput());
writer_re->Update();
writer_im->Update();

return 0;
}

