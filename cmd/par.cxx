#include <iostream>
#include <fstream>

#include <tclap/CmdLine.h>

#include <itkImage.h>
#include <itkINRImageIOFactory.h>
#include <itkImageFileReader.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/redirect_stdout.hxx"

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
    
    bool z0;
    bool y0; 
    bool x0;

    bool o;
    string outputFilename;
};


Options parse_command_line(vector<string> tclap_argv)
{
    // Parse command line.
    bool not_required = false;
    bool required = true;
        
    TCLAP::CmdLine parser("List image formats", ' ', "0.0.0");

    // -z -y -x
    TCLAP::SwitchArg zSwitch("z","planes", "Print number of planes", parser);
    TCLAP::SwitchArg ySwitch("y","rows", "Print number of rows", parser);
    TCLAP::SwitchArg xSwitch("x","columns", "Print number of columns", parser);

    // -o
    TCLAP::SwitchArg oSwitch("o","", "Print number of bytes of a pixel.", parser);

    // -z0 -y0 -x0
    TCLAP::SwitchArg z0Switch("","z0", "Print z origin.", parser);
    TCLAP::SwitchArg y0Switch("","y0", "Print y origin.", parser);
    TCLAP::SwitchArg x0Switch("","x0", "Print x origin.", parser);

    // --wr
    TCLAP::ValueArg<string> wrSwitch("","wr","Output file",not_required,"","output.txt",parser);

    // input.h5
    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilename", 
        "Input image file name.",required,"INPUT",false);

    parser.add(inputFilenamesArg);

    parser.parse(tclap_argv);

    // Store values in opt.
    Options opt;
    opt.inputFilenames = inputFilenamesArg.getValue();
    opt.z = zSwitch.getValue();
    opt.y = ySwitch.getValue();
    opt.x = xSwitch.getValue();
    opt.z0 = z0Switch.getValue();
    opt.y0 = y0Switch.getValue();
    opt.x0 = x0Switch.getValue();
    opt.o = oSwitch.getValue();
    opt.outputFilename = wrSwitch.getValue();

    return opt;
}

void postprocess_options(Options& opt)
{
    if (not (opt.z or opt.y or opt.x or opt.z0 or opt.y0 or opt.x0 or opt.o)) {
        opt.filename = true;
        opt.z = true;
        opt.y = true;
        opt.x = true;
        opt.z0 = true;
        opt.y0 = true;
        opt.x0 = true;
        opt.o = true;
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
    //nc = io->GetNumberOfComponents();

    if (opt.filename) cout << io->GetFileName();
    if (opt.z) cout << " -z " << io->GetDimensions(ZD);
    if (opt.y) cout << " -y " << io->GetDimensions(YD);
    if (opt.x) cout << " -x " << io->GetDimensions(XD);
    if (opt.z0) cout << " -z0 " << io->GetOrigin(ZD);
    if (opt.y0) cout << " -y0 " << io->GetOrigin(YD);
    if (opt.x0) cout << " -x0 " << io->GetOrigin(XD);
    if (opt.o) cout << " -o " << io->GetComponentSize();
    if (opt.filename)
      cout << endl;
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
    for (int ifile=0 ; ifile < opt.inputFilenames.size() ; ifile++) {
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
}
