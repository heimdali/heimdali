#include <string>
#include <sstream>
#include <math.h>

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
TCLAP::SwitchArg fixedSwitch("f","fixed", "Convert from fixed point.", parser, false);

// -b
TCLAP::ValueArg<unsigned int> bitsSwitch("b","bits","Convert from binary",false,0,"NBITS",parser);

// -o
TCLAP::ValueArg<int> oSwitch("o","bytes","Number of bytes per pixel component.",false,1,"NBYTES",parser);

// -e
TCLAP::ValueArg<int> eSwitch("e","exponent","Exponent value",false,0,"EXPONENT",parser);

parser.parse(argc,argv);
bool is_signed = signedSwitch.getValue();
unsigned int nbits;
int exponent = eSwitch.getValue();
ostringstream error_msg;

if (bitsSwitch.getValue() && fixedSwitch.getValue()) {
    error_msg << "-b and -f are incompatible";
    throw(TCLAP::ArgException(error_msg.str()));
}

if (bitsSwitch.getValue() != 0) {
    nbits = bitsSwitch.getValue();
} else {
    nbits = 8 * oSwitch.getValue();
}

vector<int> fixed_points = fixedPointArgs.getValue();
vector<int>::iterator it;

float inputValue, outputValue, factor;


for(it =  fixed_points.begin() ; it != fixed_points.end() ; ++it)
{
    inputValue = *it;
    if (is_signed) {
        if (inputValue > 0) {
            factor = pow(2,exponent) / (pow(2, nbits-1) - 1);
        }
        else {
            factor = pow(2,exponent) / pow(2, nbits-1);
        }
    } else {
        factor = pow(2,exponent) / (pow(2, nbits) - 1);
    }
    outputValue = inputValue * factor;
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
