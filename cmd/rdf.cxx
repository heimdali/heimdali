#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include "itkFFTWForwardFFTImageFilter.h"
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

    TCLAP::CmdLine parser("Direct FFT on real image", ' ', HEIMDALI_VERSION);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);
    string inputFilename;
    string outputFilename_re;
    string outputFilename_im;
    Heimdali::parse_tclap_image_in_image_out_image_out(filenamesArg, inputFilename,
                                                       outputFilename_re, outputFilename_im);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef float PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;

    // Command line tool readers.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);
    cmdreader->convert_fixed_point_to_floating_point_on();

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<VectorImageType, ScalarImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // FFT
    typedef itk::FFTWForwardFFTImageFilter<ScalarImageType> FFTType;
    FFTType::Pointer fftFilter = FFTType::New();

    typedef FFTType::OutputImageType FFTOutputImageType;

    // Extract the real part
    typedef itk::ComplexToRealImageFilter< FFTOutputImageType, ScalarImageType> RealFilterType;
    RealFilterType::Pointer refilter = RealFilterType::New();

    // Extract the imaginary part
    typedef itk::ComplexToImaginaryImageFilter< FFTOutputImageType, ScalarImageType> ImaginaryFilterType;
    ImaginaryFilterType::Pointer imfilter = ImaginaryFilterType::New();

    // duplicator
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator_re = DuplicatorType::New();
    DuplicatorType::Pointer duplicator_im = DuplicatorType::New();

    // composer
    typedef itk::ComposeImageFilter<ScalarImageType> ComposerType;
    ComposerType::Pointer composer_re = ComposerType::New();
    ComposerType::Pointer composer_im = ComposerType::New();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* writer_re = WriterType::make_cmd_writer(outputFilename_re);
    WriterType* writer_im = WriterType::make_cmd_writer(outputFilename_im);


    unsigned int iregionmax = 1E+06;
    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        cmdreader->next_iteration();
        if (cmdreader->is_complete()) break;

        indexer->SetInput(cmdreader->GetOutput());

        for (unsigned int componentIndex = 0 ;
                          componentIndex < cmdreader->get_sc();
                          componentIndex++)
        {
            indexer->SetIndex(componentIndex);

            // FFT
            fftFilter->SetInput( indexer->GetOutput() );

            // Extract real and imaginary part.
            refilter->SetInput(fftFilter->GetOutput());
            imfilter->SetInput(fftFilter->GetOutput());
            refilter->Update();
            imfilter->Update();
            refilter->Modified();
            imfilter->Modified();

            duplicator_re->SetInputImage(refilter->GetOutput());
            duplicator_im->SetInputImage(imfilter->GetOutput());
            duplicator_re->Update();
            duplicator_im->Update();

            composer_re->SetInput(componentIndex, duplicator_re->GetOutput());
            composer_im->SetInput(componentIndex, duplicator_im->GetOutput());
            composer_re->Modified();
            composer_im->Modified();
            composer_re->Update();
            composer_im->Update();
        }

        // Write output.
        writer_re->Write( composer_re->GetOutput() );
        writer_im->Write( composer_im->GetOutput() );
        writer_re->Update();
        writer_im->Update();
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
