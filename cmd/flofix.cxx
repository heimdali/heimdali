#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser(
    "Converts from floating point to fixed point",
    ' ', HEIMDALI_VERSION);

parser.parse(argc,argv);

} // End of 'try' block.

// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "fixflo: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "fixflo: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "fixflo: ERROR: " << e.getMessage() << endl;
}

return 0;

}
