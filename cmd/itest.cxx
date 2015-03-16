#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

int main(int argc, char** argv)
{
    try {

    // Parse command line.
    TCLAP::CmdLine parser("Perform simple tests on image informations", ' ', HEIMDALI_VERSION);

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

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
