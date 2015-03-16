#include <sstream>

#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

using namespace std;

int main(int argc, char** argv)
{
    try {

    // Parse command line.
    TCLAP::CmdLine parser("Perform simple tests on image informations", ' ', HEIMDALI_VERSION);

    TCLAP::UnlabeledMultiArg<string> inputFilenamesArg("inputFilenames", 
        "Input image file name.",false,"IMAGE0-IN [IMAGE1-IN]",parser);

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

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

    cout << "Image informations are the same." << endl;

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
