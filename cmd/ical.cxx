#include <iostream>
#include <fstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkArray.h>
#include <itkImageFileReader.h>
#include <itkStatisticsImageFilter.h>

#include <itkINRImageIOFactory.h>

#include "error.hxx"
#include "redirect_stdout.hxx"

int main(int argc, char** argv)
{
    std::cout << scientific << showpos << setprecision(7) << uppercase;

    try {


    //////////////////////////////////////////////////////////////////////////
    // Parse comamnd line options and arguments.
    //////////////////////////////////////////////////////////////////////////

        
    TCLAP::CmdLine cmd("Compute min, max and mean of images",
        ' ', "0.0");

    // -w output.txt
    TCLAP::ValueArg<string> outputFilenameArg("o","output",
        "Write image file name and results to file.",
        false,"","output.txt",cmd);

    // --force
    TCLAP::SwitchArg forceSwitch("f","force",
        "Force output file override.", cmd);

    // --per-component
    TCLAP::SwitchArg perComponentSwitch("p","per-component",
        "Compute stat on each pixel component, instead of globally.", cmd);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",true,"filename",cmd);

    cmd.parse(argc,argv);

    vector<string> inputFilenames = inputFilenamesArg.getValue();


    //////////////////////////////////////////////////////////////////////////
    // Types and instances.
    //////////////////////////////////////////////////////////////////////////


    // Pixel
    typedef float PixelType;

    // Images
    const unsigned int Dimension = 3;
    int nz,ny,nx,nc;
    typedef itk::Image<PixelType, Dimension> ScalarImageType;
    typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
    VectorImageType::Pointer vectorImage;
    ScalarImageType::Pointer scalarImage = ScalarImageType::New();

    // Readers
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 
    typedef itk::ImageFileReader< VectorImageType >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

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
    itk::Index<3> index;

    // Redirect stdout to stderr or to file.
    Heimdali::RedirectStdout redirection(outputFilenameArg.getValue(), forceSwitch.getValue());

    //////////////////////////////////////////////////////////////////////////
    // Loop on input filenames.
    //////////////////////////////////////////////////////////////////////////

    // min, max, mean per component.
    float min, mean, max;

    // All componenent min, max, mean.
    float min_all, mean_all, max_all;

    for (int ifile=0 ; ifile<inputFilenames.size() ; ifile++) {

        // Read file into vectorImage
        // for UpdateLargestPossibleRegion, see:
        // http://itk-insight-users.2283740.n2.nabble.com/ 
        //     Using-an-ITK-reader-multiple-times-tt3793576.html#a3795509
        if (inputFilenames[ifile]  == "-")
        {
            //itk::HDF5ImageIO::Pointer HDF5io;
            //HDF5io = itk::HDF5ImageIO::New();
            //reader->SetImageIO(HDF5io);
            //itk::INRImageIO::Pointer INRio;
            //INRio = itk::INRImageIO::New();
            //reader->SetImageIO(INRio);
            //reader->SetFileName("/dev/stdin");
            reader->SetFileName("nosuch.h5stdin");
            //reader->SetFileName("/dev/stdin");
        }
        else
        {
            reader->SetFileName( inputFilenames[ifile] );
        }
        reader->UpdateLargestPossibleRegion();
        vectorImage = reader->GetOutput();

        // Get vectorImage dimension
        region = vectorImage->GetLargestPossibleRegion();
        vectorImageSize = region.GetSize();
        nz = vectorImageSize[2];
        ny = vectorImageSize[1];
        nx = vectorImageSize[0];
        nc = reader->GetOutput()->GetNumberOfComponentsPerPixel();

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

        for (int ic=0, istat=0 ; ic<nc ; ic++)
        {
             for (int iz=0 ; iz<nz ; iz++) { index[2] = iz;
             for (int iy=0 ; iy<ny ; iy++) { index[1] = iy;
             for (int ix=0 ; ix<nx ; ix++) { index[0] = ix;
                 scalarImage->SetPixel(index, vectorImage->GetPixel(index)[ic] );
             }}}
             scalarImage->Modified(); // SetPixel doesn't update the modified time.

             statisticsImageFilter->SetInput(scalarImage);
             statisticsImageFilter->Update();

             min = statisticsImageFilter->GetMinimum();
             mean = statisticsImageFilter->GetMean();
             max = statisticsImageFilter->GetMaximum();

             if (perComponentSwitch.getValue()) {
                 std::cout << ic << " " << min << " " << mean << " " << max << endl;
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
            cout << min << " " << mean << " " << max << endl;
        }


        scalarImage->Initialize();
        vectorImage->Initialize();
    }


    } 

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "ical: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }
    // Input/output.
    catch (IOError &e) {
        cerr << "ical: ERROR: " << e.getMessage() << endl;
    }
}

