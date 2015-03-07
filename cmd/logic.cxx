#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser("Boolean operation on image",
                      ' ', HEIMDALI_VERSION);

HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArg,parser)

TCLAP::SwitchArg andSwitch("","et", "Boolean and", parser, false);
TCLAP::SwitchArg orSwitch("","ou", "Boolean or", parser, false);
TCLAP::SwitchArg xorSwitch("","xou", "Boolean xor", parser, false);
TCLAP::ValueArg<int> maArg("","ma","And with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> foArg("","fo","Or with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::ValueArg<int> coArg("","co","Xor with hexadecimal constant HH",false,0,"HH",parser);
TCLAP::SwitchArg invSwitch("","inv", "", parser, false);

vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);
parser.parse(tclap_argv);

} // End of 'try' block.


// Command line parser.
catch (TCLAP::ArgException &e) { 
    cerr << "mb: ERROR: " << e.error() << " for arg " << e.argId() << endl;
}

// Input/output.
catch (Heimdali::IOError &e) {
    cerr << "mb: ERROR: " << e.getMessage() << endl;
}

catch (Heimdali::NotImplementedError &e) {
    cerr << "mb: ERROR: " << e.getMessage() << endl;
}

return 0;

}
