#include <tclap/CmdLine.h>

#include <itkImageFileReader.h>
#include <itkStatisticsImageFilter.h>
#include <itkINRImageIOFactory.h>
#include "itksys/SystemTools.hxx"

#include "heimdali/cmdreader.hxx"
#include "heimdali/error.hxx"
#include "heimdali/redirect_stdout.hxx"
#include "heimdali/version.hxx"

int main(int argc, char** argv)
{
    try {

    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

        
    TCLAP::CmdLine parser("Compute min, max and mean of images",
        ' ', HEIMDALI_VERSION);

    // -w output.txt
    TCLAP::ValueArg<string> outputFilenameArg("o","output",
        "Write image file name and results to file.",
        false,"","output.txt",parser);

    // --force
    TCLAP::SwitchArg forceSwitch("f","force",
        "Force output file override.", parser);

    // --per-component
    TCLAP::SwitchArg perComponentSwitch("p","per-component",
        "Compute stat on each pixel component, instead of globally.", parser);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",false,"FILENAMES",parser);

    parser.parse(argc,argv);

    vector<string> inputFilenames = inputFilenamesArg.getValue();
    if (inputFilenames.size() == 0)
        inputFilenames.push_back("-");

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 


    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////


    // Pixel
    typedef float PixelType;

    // Images
    const unsigned int Dimension = 3;
    unsigned int nz,ny,nx,nc;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    VectorImageType::Pointer vectorImage;
    ScalarImageType::Pointer scalarImage = ScalarImageType::New();

    // Region
    typedef itk::ImageRegion<Dimension> RegionType;
    RegionType region;
    ScalarImageType::IndexType scalarImageIndex;
    ScalarImageType::SizeType scalarImageSize;
    ScalarImageType::RegionType scalarRegion;

    VectorImageType::SizeType vectorImageSize;

    // Statistic filter
    typedef itk::StatisticsImageFilter<ScalarImageType> StatisticsImageFilterType;
    StatisticsImageFilterType::Pointer statisticsImageFilter = StatisticsImageFilterType::New();

    // To copy Pixel from vectorImage to scalarImage
    itk::Index<Dimension> index;

    // Redirect stdout to stderr or to file.
    Heimdali::RedirectStdout redirection(outputFilenameArg.getValue(), forceSwitch.getValue());

    //////////////////////////////////////////////////////////////////////////
    // Loop on input filenames.
    //////////////////////////////////////////////////////////////////////////

    // min, max, mean per component.
    float min, mean, max;

    // All componenent min, max, mean.
    float min_all, mean_all, max_all;

    typedef Heimdali::CmdReader<VectorImageType> CmdReaderType;

    for (unsigned int ifile=0 ; ifile<inputFilenames.size() ; ifile++) {
        string inputFilename = inputFilenames[ifile];

        if ( inputFilename != "" && inputFilename != "-" && 
             ! itksys::SystemTools::FileExists(inputFilename) )
            continue;

        CmdReaderType* cmdreader = CmdReaderType::make_cmd_reader(0, inputFilename);
        cmdreader->convert_fixed_point_to_floating_point_on();
        cmdreader->next_iteration();
        cmdreader->Update();
        vectorImage = cmdreader->GetOutput();

        // Get vectorImage dimension
        region = vectorImage->GetLargestPossibleRegion();
        vectorImageSize = region.GetSize();
        nz = vectorImageSize[2];
        ny = vectorImageSize[1];
        nx = vectorImageSize[0];
        nc = vectorImage->GetNumberOfComponentsPerPixel();

        // Create scalarImage that will contains 1 component of the VectorImage
        scalarImageIndex.Fill(0);
        scalarImageSize[0] = nx;
        scalarImageSize[1] = ny;
        scalarImageSize[2] = nz;
        scalarRegion.SetIndex(scalarImageIndex);
        scalarRegion.SetSize(scalarImageSize);
        scalarImage->SetRegions(scalarRegion);
        scalarImage->Allocate();

        // Compute stats.
        // TODO: implement itk::StatisticsImageFilter for VectorImage, so we can
        // compute stats effecently, without reorganizing memory into nc Images.

        for (unsigned int ic=0 ; ic<nc ; ic++)
        {
             for (unsigned int iz=0 ; iz<nz ; iz++) { index[2] = iz;
             for (unsigned int iy=0 ; iy<ny ; iy++) { index[1] = iy;
             for (unsigned int ix=0 ; ix<nx ; ix++) { index[0] = ix;
                 scalarImage->SetPixel(index, vectorImage->GetPixel(index)[ic] );
             }}}
             scalarImage->Modified(); // SetPixel doesn't update the modified time.

             statisticsImageFilter->SetInput(scalarImage);
             statisticsImageFilter->Update();

             min = statisticsImageFilter->GetMinimum();
             mean = statisticsImageFilter->GetMean();
             max = statisticsImageFilter->GetMaximum();

             if (perComponentSwitch.getValue()) {
                printf("%d %14g%14g%14g\n", ic, min ,mean, max);
             } else {
                 if (ic==0) {
                     min_all = min;
                     mean_all = mean;
                     max_all = max;
                 } else {
                     min_all = std::min(min,min_all);
                     mean_all += mean;
                     max_all = std::max(max,max_all);
                 }
             }
        }

        if (! perComponentSwitch.getValue()) {
            printf("%14g%14g%14g\n", min ,mean, max);
        }


        scalarImage->Initialize();
        vectorImage->Initialize();
    }

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS(argv[0]);
}

