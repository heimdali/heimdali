#include "itksys/SystemTools.hxx"

#include "heimdali/cmdhelper.hxx"

using namespace std;

namespace Heimdali {

/* Note: This function is not part of the CmdReader class, as this class is
 * templated of PixelType. We may want to know PixelType before using
 * CmdReader. 
 */
itk::ImageIOBase::Pointer
open_from_stdin_or_file(const string inputFilename)
{
    itk::ImageIOBase::Pointer io;
    ostringstream error_msg;

    if (inputFilename == "" or inputFilename == "-") {

        // If reading on stdin, we create a file virtual image that mimic the
        // file on the disk.
        // TODO: provide a C++ API in h5unixpipe to do in one line:
        //     H5::H5File* fileimage = h5unixpipe::fileimage_from_stdin
        H5::H5File* fileimage;
        MTB_T* tb = MTB_new_tb();
        int end_of_stdin;
        hid_t fileimage_id = H5UPfileimage_from_stdin(&end_of_stdin, tb); MTB_S;
        fileimage = h5unixpipe::fileid_to_h5file(fileimage_id);

        // Create HDF5 Image IO
        itk::HDF5ImageIO::Pointer HDF5io = itk::HDF5ImageIO::New();

        // The normal ITK way is to open `filename` to obtain a `file descriptor`.
        // When reading from stdin, we don't want ITK to try to open `ghost.h5`,
        // but instead directly use `fileimage` as the `file descriptor`.
        HDF5io->SetH5File(fileimage);

        // Convert itkHDF5ImageIO to itkImageIOBase
        itk::ImageIOBase* io_raw_pointer = dynamic_cast<itk::ImageIOBase*>(HDF5io.GetPointer());
        io = io_raw_pointer;

    } else {
        // Check file exists.
        if ( ! itksys::SystemTools::FileExists(inputFilename) ) {
            error_msg << "No such file to read: " << inputFilename;
            throw(Heimdali::Exception(error_msg.str()));
        }

        // Create Image IO.
        io = itk::ImageIOFactory::CreateImageIO(inputFilename.c_str(),
                                                itk::ImageIOFactory::ReadMode);
        // Check a ImageIO has been found.
        if (io.IsNull()) {
            error_msg << "No driver can read file: " << inputFilename;
            throw(Heimdali::Exception(error_msg.str()));
        }

        io->SetFileName(inputFilename);
    }

    io->ReadImageInformation();

    return io;
}

bool
is_floating_point_type(itk::ImageIOBase::Pointer io)
{
    ostringstream error_msg;
    switch (io->GetComponentType()) {
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
            error_msg << "Pixel component type is unknown";
            throw(Heimdali::Exception(error_msg.str()));
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
            throw(Heimdali::Exception(error_msg.str()));
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
        throw(Heimdali::Exception(error_msg.str()));
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
        throw(Heimdali::Exception(error_msg.str()));
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
        throw(Heimdali::Exception(error_msg.str()));
    }
}


itk::ImageIOBase::IOComponentType 
map_to_itk_component_type(bool is_floating_point_type,
                          unsigned int component_size)
{
    ostringstream error_msg;

    if (is_floating_point_type) {
        switch(component_size)
        {
        case 4:
            return itk::ImageIOBase::FLOAT;
            break;
        case 8:
            return itk::ImageIOBase::DOUBLE;
            break;
        default:
            error_msg 
                << "Expected floating point component size to be 4 or 8, "
                << "but got: " << component_size;
            throw(Heimdali::Exception(error_msg.str()));
            break;
        }

    } else {
        switch(component_size)
        {
        case 1:
            return itk::ImageIOBase::UCHAR;
            break;
        case 2:
            return itk::ImageIOBase::USHORT;
            break;
        case 4:
            return itk::ImageIOBase::UINT;
            break;
        default:
            error_msg 
                << "Expected fixed point component size to be 1, 2 or 4, "
                << "but got: " << component_size;
            throw(Heimdali::Exception(error_msg.str()));
            break;
        }
    }
}

}
