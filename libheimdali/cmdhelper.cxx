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

bool
is_floating_point_type(itk::ImageIOBase::Pointer io)
{
    ostringstream error_msg;
    switch (io->GetComponentType()) {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Pixel component type is unknown";
            throw(Heimdali::ValueError(error_msg.str()));
            break;
        case itk::ImageIOBase::FLOAT:
            return true;
            break;
        case itk::ImageIOBase::DOUBLE:
            return true;
            break;
        case itk::ImageIOBase::UCHAR:
            return false;
            break;
        case itk::ImageIOBase::USHORT:
            return false;
            break;
        case itk::ImageIOBase::UINT:
            return false;
            break;
        default:
            error_msg
                << "Expected pixel component type to be "
                << "FLOAT, DOUBLE, UCHAR, USHORT or UINT"
                << "but, got " << io->GetComponentTypeAsString(io->GetComponentType());
            throw(Heimdali::ValueError(error_msg.str()));
            break;
    }
}

void
read_information(itk::ImageIOBase::Pointer io, string name, unsigned int& value)
{
    unsigned int ZD=2, YD=1, XD=0;

    if (name == "nplanes")
        value = io->GetDimensions(ZD);

    else if (name == "nrows")
        value = io->GetDimensions(YD);

    else if (name == "ncolumns")
        value = io->GetDimensions(XD);

    else if (name == "ncomponents")
        value = io->GetNumberOfComponents();

    else if (name == "componentsize")
        value = io->GetComponentSize();

    else {
        ostringstream error_msg;
        error_msg << "Exected name to be nplanes, nrows, ncolumns, "
                  << "ncomponents or componentsize, but got: "
                  << name;
        throw(Heimdali::ValueError(error_msg.str()));
    }
}

void
read_information(itk::ImageIOBase::Pointer io, string name, float& value)
{
    unsigned int ZD=2, YD=1, XD=0;
    if (name == "z0")
        value = io->GetOrigin(ZD);

    else if (name == "y0")
        io->GetOrigin(YD);

    else if (name == "x0")
        io->GetOrigin(XD);

    else {
        ostringstream error_msg;
        error_msg << "Exected name to be z0, y0 or x0, but got: " << name;
        throw(Heimdali::ValueError(error_msg.str()));
    }
}

void
read_information(itk::ImageIOBase::Pointer io, string name, bool& value)
{
    if (name == "floating-point")
        value = Heimdali::is_floating_point_type(io);

    else if (name == "fixed-point")
        value = ! Heimdali::is_floating_point_type(io);

    else {
        ostringstream error_msg;
        error_msg << "Exected name to be floating-point or fixed-point, "
                  << "but got: " << name;
        throw(Heimdali::ValueError(error_msg.str()));
    }
}

}
