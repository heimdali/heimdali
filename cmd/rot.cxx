#include <tclap/CmdLine.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"

int
main(int argc, char** argv)
{
    try {

    TCLAP::CmdLine parser("z-axis rotation and translation", ' ', HEIMDALI_VERSION);

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
    parser.parse(tclap_argv);


    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "asm: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::NotImplementedError &e) {
        cerr << "asm: ERROR: " << e.getMessage() << endl;
    }
}
