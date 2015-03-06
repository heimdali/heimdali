#include <string>
#include <sstream>

#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

try {

TCLAP::CmdLine parser(
    "Converts from fixed point to floating point",
    ' ', HEIMDALI_VERSION);

TCLAP::UnlabeledMultiArg<int> fixedPointArgs("FixedPoints", "Fixed point number to convert",
                                              false,"FIXED-POINT",parser);
// -s
TCLAP::SwitchArg signedSwitch("s","signed", "Fixed point is signed.", parser, false);

// -f
TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert from fixed point.", parser, true);

// -b
TCLAP::ValueArg<int> binarySwitch("b","binary","Convert from binary",false,0,"NBITS",parser);

// -o
TCLAP::ValueArg<int> oSwitch("o","bytes","Number of bytes per pixel component.",false,1,"NBYTES",parser);

// -e
TCLAP::ValueArg<int> eSwitch("e","exponent","Exponent value",false,1,"EXPONENT",parser);

parser.parse(argc,argv);
bool is_signed = signedSwitch.getValue();
bool is_fixed = fixedSwitch.getValue();

vector<int> fixed_points = fixedPointArgs.getValue();
vector<int>::iterator it;

float inputValue, outputValue;
ostringstream error_msg;

for(it =  fixed_points.begin() ; it != fixed_points.end() ; ++it)
{
    inputValue = *it;
    if (is_fixed) {
        if (is_signed)
            outputValue = inputValue > 0 ? inputValue / 127. : inputValue / 128.;
        else
            outputValue = inputValue / 255.;
    } else {
        error_msg << "Not implemented";
        throw(Heimdali::NotImplementedError(error_msg.str()));
    }
    printf("%.6f ", outputValue);
}
cout << endl;

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
