#include "heimdali/cmdhelper.hxx"

using namespace std;

namespace Heimdali {

itk::ImageIOBase::Pointer
read_informations(string filename)
{
    itk::ImageIOBase::Pointer imageio;

    if (filename == "" or filename == "-") {
        typedef float PixelType;
        const unsigned int Dimension = 3;
        typedef itk::VectorImage<PixelType, Dimension> VectorImageType;
        typedef Heimdali::CmdReader<VectorImageType> CmdReaderType;
        CmdReaderType* cmdreader = CmdReaderType::make_cmd_reader(0, filename);
        cmdreader->next_iteration();
        cmdreader->Update();
        imageio = cmdreader->reader()->GetImageIO();
    } else {
        imageio = itk::ImageIOFactory::CreateImageIO( filename.c_str(), itk::ImageIOFactory::ReadMode);
        imageio->SetFileName(filename);
    }

    imageio->ReadImageInformation();
    return imageio;
}

}
