#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkMaximumImageFilter.h>
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

    TCLAP::CmdLine parser("Maximum of two images", ' ', HEIMDALI_VERSION);

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
    ToImageType::Pointer toImage1 = ToImageType::New();
    ToImageType::Pointer toImage2 = ToImageType::New();

    // Maximum filter.
    typedef itk::MaximumImageFilter <ToImageType,ToImageType,ScalarImageType> MaximumImageFilterType;
    MaximumImageFilterType::Pointer maximizer = MaximumImageFilterType::New ();

    // Duplicator.
    typedef itk::ImageDuplicator<ScalarImageType> DuplicatorType;
    DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // Image to VectorImage
    typedef itk::ComposeImageFilter<ScalarImageType> ToVectorImageType;
    ToVectorImageType::Pointer toVectorImage = ToVectorImageType::New();

    ScalarImageType::IndexType index;
    index[0] = 1;
    index[1] = 0;
    index[2] = 0;

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

        toImage1->SetImage(vectorImage1);
        toImage2->SetImage(vectorImage2);

        for (unsigned int ic = 0 ; ic < cmdreader1->get_sc() ; ++ic)
        {
            // VectorImage to Image
            toImage1->SetExtractComponentIndex(ic);
            toImage2->SetExtractComponentIndex(ic);
            toImage1->Modified();
            toImage2->Modified();

            // Maximum images.
            maximizer->SetInput1(toImage1);
            maximizer->SetInput2(toImage2);
            maximizer->Update();
            maximizer->Modified();

            // Image to VectorImage
            duplicator->SetInputImage(maximizer->GetOutput());
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
