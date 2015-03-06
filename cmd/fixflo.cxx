#include <string>

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
TCLAP::ValueArg<int> binarySwitch("b","binary","Convert from binary",false,0,"NBITS",parser);

// -o
TCLAP::ValueArg<int> oSwitch("o","bytes","Number of bytes per pixel component.",false,4,"NBYTES",parser);

// -e
TCLAP::ValueArg<int> eSwitch("e","exponent","Exponent value",false,1,"EXPONENT",parser);

parser.parse(argc,argv);

vector<int> fixed_points = fixedPointArgs.getValue();
vector<int>::iterator it;

for(it =  fixed_points.begin() ; it != fixed_points.end() ; ++it)
{
    printf("%.6f ",(float) *it / 255.);
}
cout << endl;

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
