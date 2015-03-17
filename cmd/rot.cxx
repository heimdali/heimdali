#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>
#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include <itkImageDuplicator.h>
#include <itkComposeImageFilter.h>
#include <itkAffineTransform.h>
#include <itkCenteredRigid2DTransform.h>
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include <itkVectorIndexSelectionCastImageFilter.h>
#include "itkSliceBySliceImageFilter.h"

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

/* Pipeline is as follow:
 *
 * reader -> Vector3DImageType  > indexer -> Scalar3DImageType ->
 * slicer -> Scalar2DImageType -> resampler -> Scalar2DImageType ->
 * duplicator -> Scalar2DImageType -> slicer -> Scalar3DImageType ->
 * composer -> Vector3DImageType -> writer
 *
 * reader -> Vector3DImageType  > indexer -> Scalar3DImageType ->
 * slicer -> Scalar2DImageType -> resampler -> Scalar2DImageType ->
 * slicer -> Scalar3DImageType -> composer -> Vector3DImageType -> writer
 *
 * pb: pas de duplicator, pas acces a l'input de resampler
 */

unsigned int XD=0, YD=1, ZD=2;

int
main(int argc, char** argv)
{
    try {

    // Parse command line.
    TCLAP::CmdLine parser("z-axis rotation and translation", ' ', HEIMDALI_VERSION);


    TCLAP::ValueArg<float> angleArg("a","angle", "Rotation angle (degree)",false,
        90,"ANGLE", parser);

    TCLAP::ValueArg<int> xcArg("","xc", "Center of rotation (-1: image center)",false, -1,"XC", parser);
    TCLAP::ValueArg<int> ycArg("","yc", "Center of rotation (-1: image center)",false, -1,"YC", parser);

    TCLAP::ValueArg<int> txArg("","tx", "Translation to apply after rotation",false, -1,"TX", parser);
    TCLAP::ValueArg<int> tyArg("","ty", "Translation to apply after rotation",false, -1,"TY", parser);

    TCLAP::ValueArg<float> xArg("x","columns", "Number of columns",false, 0,"NX", parser);
    TCLAP::ValueArg<float> yArg("y","rows", "Number of rows",false, 0,"NY", parser);

    HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    string inputFilename;
    string outputFilename;
    Heimdali::parse_tclap_image_in_image_out(filenamesArg, inputFilename, outputFilename);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Image type.
    const unsigned int Image3DDimension = 3;
    const unsigned int Image2DDimension = 2;
    typedef unsigned char PixelType;
    typedef itk::VectorImage<PixelType, Image3DDimension> Vector3DImageType;
    typedef itk::Image<PixelType, Image3DDimension> Scalar3DImageType;
    typedef itk::Image<PixelType, Image2DDimension> Scalar2DImageType;

    // reader
    typedef itk::ImageFileReader< Vector3DImageType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputFilename);

    // indexer
    typedef itk::VectorIndexSelectionCastImageFilter<Vector3DImageType, Scalar3DImageType> IndexerType;
    IndexerType::Pointer indexer = IndexerType::New();

    // interpolator
    typedef itk::LinearInterpolateImageFunction< Scalar2DImageType, double > InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    // transform
    typedef itk::CenteredRigid2DTransform<double> TransformType;
    TransformType::Pointer transform = TransformType::New();

    // resampler
    typedef itk::ResampleImageFilter< Scalar2DImageType, Scalar2DImageType > ResamplerType;
    ResamplerType::Pointer resampler = ResamplerType::New();
    resampler->SetInterpolator(interpolator);
    resampler->SetDefaultPixelValue(0);
    resampler->SetTransform(transform);

    // composer
    typedef itk::ComposeImageFilter<Scalar3DImageType> ComposerType;
    ComposerType::Pointer composer = ComposerType::New();

    // duplicator
    //typedef itk::ImageDuplicator<Scalar2DImageType> DuplicatorType;
    //typedef itk::ImageDuplicator<Scalar3DImageType> DuplicatorType;
    //typename DuplicatorType::Pointer duplicator = DuplicatorType::New();

    // slicer
    typedef itk::SliceBySliceImageFilter<Scalar3DImageType,Scalar3DImageType,ResamplerType,ResamplerType> SlicerType;
    SlicerType::Pointer slicer = SlicerType::New();

    reader->Update();
    Vector3DImageType::Pointer image3D = reader->GetOutput();
    Scalar3DImageType::SizeType size3D = image3D->GetLargestPossibleRegion().GetSize();
    Scalar3DImageType::PointType origin3D = image3D->GetOrigin();
    Scalar3DImageType::SpacingType spacing3D = image3D->GetSpacing();
    Scalar3DImageType::DirectionType direction3D = image3D->GetDirection();

    Scalar2DImageType::SizeType size2D;
    size2D[XD] = size3D[XD];
    size2D[YD] = size3D[YD];
    resampler->SetSize(size2D);

    Scalar2DImageType::PointType origin2D;
    origin2D[XD] = origin3D[XD];
    origin2D[YD] = origin3D[YD];
    resampler->SetOutputOrigin(origin2D);

    Scalar2DImageType::SpacingType spacing2D;
    spacing2D[XD] = spacing3D[XD];
    spacing2D[YD] = spacing3D[YD];
    resampler->SetOutputSpacing(spacing2D);

    Scalar2DImageType::DirectionType direction2D;
    direction2D[XD][XD] = direction3D[XD][XD];
    direction2D[XD][YD] = direction3D[XD][YD];
    direction2D[YD][XD] = direction3D[YD][XD];
    direction2D[YD][YD] = direction3D[YD][YD];
    resampler->SetOutputDirection(direction2D);

    indexer->SetInput(reader->GetOutput());

    // Rotation
    TransformType::InputPointType center;
    if (xcArg.getValue() == -1)
        center[XD] = 0.5 * spacing2D[XD] * (size2D[XD]-1.5);
    else
        center[XD] = xcArg.getValue();

    if (ycArg.getValue() == -1)
        center[YD] = 0.5 * spacing2D[YD] * (size2D[YD]-1.5);
    else
        center[YD] = ycArg.getValue();

    transform->SetCenter(center);
    transform->SetAngleInDegrees(angleArg.getValue());

    // Translation
    TransformType::OutputVectorType translation;
    translation[XD] = txArg.getValue();
    translation[YD] = tyArg.getValue();
    transform->SetTranslation(translation);

    for (unsigned int componentIndex = 0 ;
                      componentIndex < reader->GetImageIO()->GetNumberOfComponents() ;
                      componentIndex++)
    {

        indexer->SetIndex(componentIndex);
        indexer->Update();

        slicer->SetFilter(resampler);
        slicer->SetInputFilter(resampler);
        slicer->SetOutputFilter(resampler);

        slicer->SetInput(0, indexer->GetOutput() );
        slicer->Update();

        resampler->Update();

        //duplicator->SetInputImage(resampler->GetOutput());
        //duplicator->SetInputImage(indexer->GetOutput());
        //duplicator->Update();

        composer->SetInput(componentIndex, slicer->GetOutput());
    }
    composer->Update();

    // writer
    typedef itk::ImageFileWriter< Vector3DImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename);
    writer->SetInput(composer->GetOutput());
    writer->Update();


    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "rot: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::NotImplementedError &e) {
        cerr << "rot: ERROR: " << e.getMessage() << endl;
    }
}
