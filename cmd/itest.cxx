#include <sstream>

#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cmdhelper.hxx"

using namespace std;

static int ZD = 2, YD = 1, XD = 0;

template <typename S, typename T>
void
test_value(string label, S value, T expected_value, ostringstream& error_msg)
{
    if (value != expected_value)
        error_msg << label << " are different: " << value 
                  << " VS " << expected_value << " ." << endl;;
}

int main(int argc, char** argv)
{
    try {

    // Parse command line.
    TCLAP::CmdLine parser("Perform simple tests on image informations", ' ', HEIMDALI_VERSION);

    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",false,"IMAGE0-IN [IMAGE1-IN]",parser);

    // -z -y -x -v
    TCLAP::ValueArg<unsigned int> zValue("z","nplanes", "Number of planes",false,0,"NZ", parser);
    TCLAP::ValueArg<unsigned int> yValue("y","nrows", "Number of rows",false,0,"NY", parser);
    TCLAP::ValueArg<unsigned int> xValue("x","ncolumns", "Number of columns",false,0,"NX", parser);
    TCLAP::ValueArg<unsigned int> vValue("v","component", "Number of pixel components",false,0, "NV", parser);

    // -z0 -y0 -x0
    TCLAP::ValueArg<unsigned int> z0Value("","z0", "z origin",false,0,"Z0", parser);
    TCLAP::ValueArg<unsigned int> y0Value("","y0", "y origin",false,0,"Y0", parser);
    TCLAP::ValueArg<unsigned int> x0Value("","x0", "x origin",false,0,"X0", parser);

    // -o
    TCLAP::ValueArg<unsigned int> oValue("o","", "Number of bytes of a pixel component",false,0,"NBYTES", parser);

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    // Parse input file names.
    vector<string> inputFilenames = inputFilenamesArg.getValue();
    string inputFilename0;
    string inputFilename1;
    ostringstream error_msg;
    switch (inputFilenames.size())
    {
    case 1:
        inputFilename0 = inputFilenames[0];
        break;
    default:
        error_msg << "One or two fileanmes expected, but got: "
                  << inputFilenames.size();
        throw(Heimdali::ValueError(error_msg.str()));
        break;
    }

    itk::ImageIOBase::Pointer io = Heimdali::read_informations(inputFilename0);

    if (zValue.isSet()) test_value("Number of plane", io->GetDimensions(ZD), zValue.getValue(), error_msg); 
    if (yValue.isSet()) test_value("Number of rows", io->GetDimensions(YD), yValue.getValue(), error_msg); 
    if (xValue.isSet()) test_value("Number of columns", io->GetDimensions(XD), xValue.getValue(), error_msg); 
    if (vValue.isSet()) test_value("Number of components", io->GetNumberOfComponents(), vValue.getValue(), error_msg); 

    if (z0Value.isSet()) test_value("z origin", io->GetOrigin(ZD), z0Value.getValue(), error_msg);
    if (y0Value.isSet()) test_value("y origin", io->GetOrigin(YD), y0Value.getValue(), error_msg);
    if (x0Value.isSet()) test_value("x origin", io->GetOrigin(XD), x0Value.getValue(), error_msg);

    if (oValue.isSet())
        test_value("Number of bytes of a pixel component", io->GetComponentSize(), oValue.getValue(), error_msg);


    string error_msg_str = error_msg.str();
    if (error_msg.str().empty()) {
        cout << "Image informations are the same." << endl;
        return 0;
    } else {
        cout << "Image informations are different." << endl;
        cout << error_msg.str();
        return 1;
    }

    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "itest: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "itest: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "itest: ERROR: " << e.getMessage() << endl;
    }
}
