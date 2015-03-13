#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include <itkComplexToPhaseImageFilter.h>
#include <itkComplexToModulusImageFilter.h>
#include <itkComposeImageFilter.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

TCLAP::CmdLine parser("Convert complex image from (real, imaginary) to (module, phase)", ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArg,parser)

parser.parse(argc,argv);
string inputFilename_re;
string inputFilename_im;
string outputFilename_mod;
string outputFilename_phs;
Heimdali::parse_tclap_image_in_image_in_image_out_image_out(filenamesArg,
                                                   inputFilename_re, inputFilename_im,
                                                   outputFilename_mod, outputFilename_phs);

// Put our INRimage reader in the list of readers ITK knows.
itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

// Image type.
typedef float PixelType;
const unsigned int Dimension = 3;
typedef itk::Image<PixelType, Dimension> VectorImageType;
typedef itk::Image<std::complex<float>, Dimension> ComplexVectorImageType;

// Readers
typedef itk::ImageFileReader<VectorImageType> ReaderType;
ReaderType::Pointer reader_re = ReaderType::New();
ReaderType::Pointer reader_im = ReaderType::New();
reader_re->SetFileName(inputFilename_re);
reader_im->SetFileName(inputFilename_im);

// complexer
typedef itk::ComposeImageFilter<VectorImageType,ComplexVectorImageType> ComplexerType;
ComplexerType::Pointer complexer = ComplexerType::New();
complexer->SetInput1(reader_re->GetOutput());
complexer->SetInput2(reader_im->GetOutput());

// moduler
typedef itk::ComplexToModulusImageFilter<ComplexVectorImageType, VectorImageType> ModulerType;
ModulerType::Pointer moduler = ModulerType::New();
moduler->SetInput(complexer->GetOutput());

// phaser
typedef itk::ComplexToPhaseImageFilter<ComplexVectorImageType, VectorImageType> PhaserType;
PhaserType::Pointer phaser = PhaserType::New();
phaser->SetInput(complexer->GetOutput());

// Writers
typedef itk::ImageFileWriter<VectorImageType> WriterType;
WriterType::Pointer writer_mod = WriterType::New();
WriterType::Pointer writer_phs = WriterType::New();
writer_mod->SetFileName(outputFilename_mod);
writer_phs->SetFileName(outputFilename_phs);
writer_mod->SetInput(moduler->GetOutput());
writer_phs->SetInput(phaser->GetOutput());
writer_mod->Update();
writer_phs->Update();

}
