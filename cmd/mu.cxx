#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkMultiplyImageFilter.h>
#include <itkVectorImageToImageAdaptor.h>
#include <itkComposeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkINRImageIOFactory.h>

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"

using namespace std;

int main(int argc, char** argv)
{ 
    try {

    TCLAP::CmdLine parser("Multiply two images", ' ', HEIMDALI_VERSION);

    // -streaming N
    TCLAP::ValueArg<int> streaming("s","streaming", "Number of lines to stream",
        false, 0,"NUMBER_OF_LINES", parser);
    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(argc,argv);
    string inputFilename0;
    string inputFilename1;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_in_image_out(filenamesArg, inputFilename0,
                                                      inputFilename1, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<Heimdali::PixelFloat, Heimdali::ImageDimension> ScalarImageType;
    typedef itk::VectorImage<Heimdali::PixelFloat, Heimdali::ImageDimension> VectorImageType;
    VectorImageType::Pointer vectorImage1;
    VectorImageType::Pointer vectorImage2;

    // Command line tool readers.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader1 = ReaderType::make_cmd_reader( streaming.getValue(),
                                                          inputFilename0);

    ReaderType* cmdreader2 = ReaderType::make_cmd_reader(streaming.getValue(),
                                                         inputFilename1);

    cmdreader1->convert_fixed_point_to_floating_point_on();
    cmdreader2->convert_fixed_point_to_floating_point_on();

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    // VectorImage to Image
    typedef itk::VectorImageToImageAdaptor
        <Heimdali::PixelFloat, Heimdali::ImageDimension> ToImageType;
    ToImageType::Pointer toImage1Filter = ToImageType::New();
    ToImageType::Pointer toImage2Filter = ToImageType::New();

    // Multiply filter.
    typedef itk::MultiplyImageFilter <ToImageType,ToImageType,ScalarImageType> MultiplyImageFilterType;
    MultiplyImageFilterType::Pointer multiplier = MultiplyImageFilterType::New();

    // Duplicator.
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // Image to VectorImage
    typedef itk::ComposeImageFilter<ScalarImageType> ToVectorImageType;
    ToVectorImageType::Pointer toVectorImage = ToVectorImageType::New();

    unsigned int iregionmax = 1E+06;
    for (unsigned int iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        cmdreader1->next_iteration();
        cmdreader2->next_iteration();
        if (cmdreader1->is_complete()) break;

        cmdreader1->reader()->Update();
        cmdreader2->reader()->Update();

        vectorImage1 = cmdreader1->GetOutput();
        vectorImage2 = cmdreader2->GetOutput();

        toImage1Filter->SetImage(vectorImage1);
        toImage2Filter->SetImage(vectorImage2);

        for (unsigned int ic = 0 ; ic < cmdreader1->get_sc() ; ++ic)
        {
            // VectorImage to Image
            toImage1Filter->SetExtractComponentIndex(ic);
            toImage2Filter->SetExtractComponentIndex(ic);
            toImage1Filter->Modified();
            toImage2Filter->Modified();

            // Multiply images.
            multiplier->SetInput1(toImage1Filter);
            multiplier->SetInput2(toImage2Filter);
            multiplier->Update();
            multiplier->Modified();

            // Image to VectorImage
            duplicator->SetInputImage(multiplier->GetOutput());
            duplicator->Update();

            toVectorImage->SetInput(ic, duplicator->GetOutput());
            toVectorImage->Modified();
            toVectorImage->Update();
        }

        // Write output.
        cmdwriter->Write( toVectorImage->GetOutput() );
        cmdwriter->Update();

        vectorImage1->Initialize();
        vectorImage2->Initialize();
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
