#include <tclap/CmdLine.h>

#include <itkImage.h>
#include "itkPasteImageFilter.h"

#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/itkhelper.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

using namespace std;

unsigned int ZD=2, YD=1, XD=0;

typedef float PixelType;

int main(int argc, char** argv)
{ 

    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    TCLAP::CmdLine parser("Swap image quadrant regions",
                          ' ', HEIMDALI_VERSION);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    parser.parse(tclap_argv);
    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    typedef itk::Image<PixelType, Heimdali::ImageDimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Heimdali::ImageDimension> VectorImageType;

    // Command line tool readers.
    typedef Heimdali::CmdReader<VectorImageType> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilename);

    // Command line tool writer.
    typedef Heimdali::CmdWriter<VectorImageType> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilename);

    VectorImageType::Pointer image;

    // Read input.
    cmdreader->next_iteration();
    cmdreader->next_iteration();
    if (! cmdreader->is_complete()) {
        throw(Heimdali::Exception("Streaming not supported"));
    }
    cmdreader->reader()->Update();
    VectorImageType::Pointer source = cmdreader->reader()->GetOutput();

    // Allocate image.
    VectorImageType::IndexType start;
    start.Fill(0);
    VectorImageType::SizeType size = source->GetLargestPossibleRegion().GetSize();
    unsigned int sx = size[XD];
    if ( sx%2 != 0)
        throw(Heimdali::Exception("Number of columns must be even"));
    unsigned int sy = size[YD];
    if ( sy%2 != 0)
        throw(Heimdali::Exception("Number of rows must be even"));
    VectorImageType::RegionType region(start,size);
    VectorImageType::Pointer dest = VectorImageType::New();
    dest->SetRegions(region);
    dest->SetVectorLength(source->GetNumberOfComponentsPerPixel());
    dest->Allocate();

    //paster
    typedef itk::PasteImageFilter <VectorImageType, VectorImageType> PasterType;

    /*
       For example, sx=4 and sy=6 .

       0,0  0,1 | 0,2  0,3
                |
       1,0  1,1 | 1,2  1,3
                |
       2,0  2,1 | 2,2  2,3
       ---------+----------
       3,0  3,1 | 3,2  3,4
                |
       4,0  4,1 | 4,2  4,3
                |
       5,0  5,1 | 5,2  5,3

       The four image regions are swapped like this:

       0 | 1       3 | 2
       -----  -->  -----
       2 | 3       1 | 0

       All of the 4 regions have size: (nx/2, ny/2).

       Index of the source regions are:

           source region | index        |
           0             | (0   , 0   ) |
           1             | (sx/2, 0   ) |
           2             | (0   , sy/2) |
           3             | (sx/2, sy/2) |
       
     */

    size[XD] = sx/2;
    size[YD] = sy/2;
    region.SetSize(size);
    VectorImageType::IndexType destIndex;
    destIndex[ZD] = 0;

    // 0 to 3
    PasterType::Pointer paster0 = PasterType::New();
    paster0->SetSourceImage(source);
    paster0->SetDestinationImage(dest);
    start[XD] = 0;
    start[YD] = 0;
    region.SetIndex(start);
    paster0->SetSourceRegion(region);
    destIndex[XD] = sx/2;
    destIndex[YD] = sy/2;
    paster0->SetDestinationIndex(destIndex);
    paster0->Update();

    // 3 to 0
    PasterType::Pointer paster3 = PasterType::New();
    paster3->SetSourceImage(source);
    paster3->SetDestinationImage(paster0->GetOutput());
    start[XD] = sx/2;
    start[YD] = sy/2;
    region.SetIndex(start);
    paster3->SetSourceRegion(region);
    destIndex[XD] = 0;
    destIndex[YD] = 0;
    paster3->SetDestinationIndex(destIndex);
    paster3->Modified();
    paster3->Update();

    // 1 to 2
    PasterType::Pointer paster1 = PasterType::New();
    paster1->SetSourceImage(source);
    paster1->SetDestinationImage(paster3->GetOutput());
    start[XD] = sx/2;
    start[YD] = 0;
    region.SetIndex(start);
    paster1->SetSourceRegion(region);
    destIndex[XD] = 0;
    destIndex[YD] = sy/2;
    paster1->SetDestinationIndex(destIndex);
    paster1->Modified();
    paster1->Update();

    // 2 to 1
    PasterType::Pointer paster2 = PasterType::New();
    paster2->SetSourceImage(source);
    paster2->SetDestinationImage(paster1->GetOutput());
    start[XD] = 0;
    start[YD] = sy/2;
    region.SetIndex(start);
    paster2->SetSourceRegion(region);
    destIndex[XD] = sx/2;
    destIndex[YD] = 0;
    paster2->SetDestinationIndex(destIndex);
    paster2->Modified();
    paster2->Update();

    // Write output.
    cmdwriter->Write( paster2->GetOutput() );
    cmdwriter->Update();

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}
