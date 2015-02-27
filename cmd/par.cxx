#include <iostream>
#include <fstream>
#include <sstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/redirect_stdout.hxx"
#include "heimdali/version.hxx"

using namespace std;
using namespace itk;

static int ZD = 2, YD = 1, XD = 0;

struct Options
{
    vector<string> inputFilenames;

    bool filename;

    bool z;
    bool y;
    bool x;
    bool v;
    
    bool z0;
    bool y0; 
    bool x0;

    bool o;

    bool F;
    bool r;
    bool f;
    string outputFilename;
};


Options parse_command_line(vector<string> tclap_argv)
{
    // Parse command line.
    bool not_required = false;
    bool required = true;
        
    TCLAP::CmdLine parser("List image formats", ' ', HEIMDALI_VERSION);

    // -z -y -x
    TCLAP::SwitchArg zSwitch("z","planes", "Print number of planes", parser);
    TCLAP::SwitchArg ySwitch("y","rows", "Print number of rows", parser);
    TCLAP::SwitchArg xSwitch("x","columns", "Print number of columns", parser);
    TCLAP::SwitchArg vSwitch("v","component", "Print number of pixel component", parser);

    // -o
    TCLAP::SwitchArg oSwitch("o","", "Print number of bytes of a pixel.", parser);

    // -F
    TCLAP::SwitchArg FSwitch("F","image_type", "Print image type, HDF5 or INRimage", parser);

    // -z0 -y0 -x0
    TCLAP::SwitchArg z0Switch("","z0", "Print z origin.", parser);
    TCLAP::SwitchArg y0Switch("","y0", "Print y origin.", parser);
    TCLAP::SwitchArg x0Switch("","x0", "Print x origin.", parser);

    // -f -r
    TCLAP::SwitchArg fSwitch("f","integral", "Print pixel type, float or integer", parser);
    TCLAP::SwitchArg rSwitch("r","float", "Print pixel type, float or integer", parser);

    // --wr
    TCLAP::ValueArg<string> wrSwitch("","wr","Output file",not_required,"","output.txt",parser);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilename", 
        "Input image file name.",not_required,"INPUT",false);

    parser.add(inputFilenamesArg);

    parser.parse(tclap_argv);

    // Store values in opt.
    Options opt;
    opt.inputFilenames = inputFilenamesArg.getValue();
    opt.z = zSwitch.getValue();
    opt.y = ySwitch.getValue();
    opt.x = xSwitch.getValue();
    opt.v = vSwitch.getValue();
    opt.z0 = z0Switch.getValue();
    opt.y0 = y0Switch.getValue();
    opt.x0 = x0Switch.getValue();
    opt.o = oSwitch.getValue();
    opt.F = FSwitch.getValue();
    opt.r = rSwitch.getValue();
    opt.f = fSwitch.getValue();
    opt.outputFilename = wrSwitch.getValue();

    return opt;
}

void postprocess_options(Options& opt)
{
    if (not (opt.z or opt.y or opt.x or opt.v or opt.z0 or opt.y0 or opt.x0 
                   or opt.o or opt.F or opt.f or opt.r)) {
        opt.filename = true;
        opt.z = true;
        opt.y = true;
        opt.x = true;
        opt.v = true;
        opt.z0 = true;
        opt.y0 = true;
        opt.x0 = true;
        opt.o = true;
        opt.r = true;
        opt.f = true;
    } else {
        opt.filename = false;
    }
}

ImageIOBase::Pointer
read_informations(string filename)
{
    ImageIOBase::Pointer imageio = 
        ImageIOFactory::CreateImageIO( filename.c_str(),
                                       ImageIOFactory::ReadMode);
    imageio->SetFileName(filename);
    imageio->ReadImageInformation();
    return imageio;
}

void print_informations(ImageIOBase::Pointer io, Options opt)
{
    ostringstream smsg; // Stream MeSsaGe
    if (opt.x)  smsg << "-x " << io->GetDimensions(XD) << "\t";
    if (opt.y)  smsg << "-y " << io->GetDimensions(YD) << "\t";
    if (opt.z && io->GetDimensions(ZD) != 1)  smsg << "-z " << io->GetDimensions(ZD) << "\t";
    if (opt.v && io->GetNumberOfComponents() != 1)  smsg << "-v " << io->GetNumberOfComponents() << "\t";
    if (opt.x0 && io->GetOrigin(XD) != 0) smsg << "-x0 " << io->GetOrigin(XD) << "\t";
    if (opt.y0 && io->GetOrigin(YD) != 0) smsg << "-y0 " << io->GetOrigin(YD) << "\t";
    if (opt.z0 && io->GetOrigin(ZD) != 0) smsg << "-z0 " << io->GetOrigin(ZD) << "\t";
    if (opt.o)  smsg << "-o " << io->GetComponentSize() << "\t";
    if (opt.F) {
        if ((string) io->GetNameOfClass() == (string) "HDF5ImageIO")
            smsg << "-F HDF5\t";
        else if ((string) io->GetNameOfClass() == (string) "INRImageIO")
            smsg << "-F Inrimage\t";
        else
            smsg << "-F " << io->GetNameOfClass() << "\t";
    }

    if (opt.r or opt.f) {
        ostringstream error_msg;
        switch (io->GetComponentType()) {
            case ImageIOBase::UNKNOWNCOMPONENTTYPE:
                error_msg << "Component type is unknown";
                throw(Heimdali::ValueError(error_msg.str()));
                break;
            case ImageIOBase::FLOAT:
                smsg << "-r\t";
                break;
            case ImageIOBase::DOUBLE:
                smsg << "-r\t";
                break;
            default:
                smsg << "-f\t";
                break;
        }
    }

    string msg = smsg.str();
    msg = msg.substr(0, msg.size()-1); // Remove trailing tabluation.

    if (opt.filename) cout << io->GetFileName();
    cout << " " << msg;
    if (opt.filename) cout << endl;
}


int main(int argc, char** argv)
{

    try {

    // Parse command line.
    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    Options opt = parse_command_line(tclap_argv);
    postprocess_options(opt);

    // Redirect stdout to stderr or to file.
    Heimdali::RedirectStdout redirection(opt.outputFilename, false);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Print informations about images.
    for (unsigned int ifile=0 ; ifile < opt.inputFilenames.size() ; ifile++) {
        ImageIOBase::Pointer imageio = read_informations(opt.inputFilenames[ifile]);
        read_informations(opt.inputFilenames[ifile]);
        print_informations(imageio,opt);
    }


    //////////////////////////////////////////////////////////////////////////
    // Catch exceptions.
    //////////////////////////////////////////////////////////////////////////


    } // End of 'try' block.
    

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "par: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }


    // Input/output.
    catch (Heimdali::IOError &e) {
        cerr << "par: ERROR: " << e.getMessage() << endl;
    }

    return 0;
}
