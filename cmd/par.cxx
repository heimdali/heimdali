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
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdhelper.hxx"

#define print_switch(name, value, canceling_value) \
    print_canceled = (value == canceling_value && ! opt.has_switch); \
    if (opt.name && ! print_canceled )  \
        smsg << "-" << #name << " "<< value << "\t"

using namespace std;
using namespace itk;

static int ZD = 2, YD = 1, XD = 0;

struct Options
{
    vector<string> inputFilenames;
    bool filename;
    bool z,y,x,v;
    bool z0,y0,x0;
    bool o,F,r,f;
    bool has_switch;
    string outputFilename;
};

bool
get_switch(TCLAP::SwitchArg& switch_arg, Options& opt)
{
    bool value  = switch_arg.getValue();
    if (value) opt.has_switch = true;
    return value;
}

Options
parse_command_line(vector<string> tclap_argv)
{
    // Parse command line.
    bool not_required = false;
        
    TCLAP::CmdLine parser("List image formats", ' ', HEIMDALI_VERSION);

    // -z -y -x -v
    TCLAP::SwitchArg zSwitch("z","planes", "Print number of planes", parser);
    TCLAP::SwitchArg ySwitch("y","rows", "Print number of rows", parser);
    TCLAP::SwitchArg xSwitch("x","columns", "Print number of columns", parser);
    TCLAP::SwitchArg vSwitch("v","component", "Print number of pixel component", parser);

    // -o
    TCLAP::SwitchArg oSwitch("o","", "Print number of bytes of a pixel component.", parser);

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
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",false,"FILENAMES",parser);

    parser.parse(tclap_argv);

    // Store values in opt.
    Options opt;
    opt.has_switch = false;
    opt.inputFilenames = inputFilenamesArg.getValue();
    if (opt.inputFilenames.size() == 0)
        opt.inputFilenames.push_back("-");
    opt.z = get_switch(zSwitch, opt);
    opt.y = get_switch(ySwitch, opt);
    opt.x = get_switch(xSwitch, opt);
    opt.v = get_switch(vSwitch, opt);
    opt.z0 = get_switch(z0Switch, opt);
    opt.y0 = get_switch(y0Switch, opt);
    opt.x0 = get_switch(x0Switch, opt);
    opt.o = get_switch(oSwitch, opt);
    opt.F = get_switch(FSwitch, opt);
    opt.r = get_switch(rSwitch, opt);
    opt.f = get_switch(fSwitch, opt);
    opt.outputFilename = wrSwitch.getValue();

    return opt;
}

void
postprocess_options(Options& opt)
{
    if (opt.has_switch)
        opt.filename = false;
    else {
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
    }
}

void
print_informations(ImageIOBase::Pointer io, Options opt)
{
    ostringstream smsg; // Stream MeSsaGe
    bool print_canceled;
    if (opt.x)  smsg << "-x " << io->GetDimensions(XD) << "\t";
    if (opt.y)  smsg << "-y " << io->GetDimensions(YD) << "\t";
    print_switch(z, io->GetDimensions(ZD), 1);
    print_switch(v, io->GetNumberOfComponents(), 1);
    print_switch(x0, io->GetOrigin(XD), 0);
    print_switch(y0, io->GetOrigin(YD), 0);
    print_switch(z0, io->GetOrigin(ZD), 0);
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
        ImageIOBase::Pointer imageio = Heimdali::open_from_stdin_or_file(opt.inputFilenames[ifile]);
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
