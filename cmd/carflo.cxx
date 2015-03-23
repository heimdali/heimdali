#include <tclap/CmdLine.h>

#include "heimdali/version.hxx"
#include "heimdali/error.hxx"

using namespace std;

int main(int argc, char** argv)
{ 

    try {

    typedef unsigned short InputType;

    TCLAP::CmdLine parser(
        "Converts from fixed point (unsigned char) to floating point",
        ' ', HEIMDALI_VERSION);

    TCLAP::UnlabeledMultiArg<InputType> fixedPointArgs("FixedPoints", "Fixed point number to convert",
                                                  false,"FIXED-POINT",parser);

    parser.parse(argc,argv);

    vector<InputType> fixed_points = fixedPointArgs.getValue();
    vector<InputType>::iterator it;

    for(it =  fixed_points.begin() ; it != fixed_points.end() ; ++it) {
        printf("%.6f ",*it / 255.);
    }
    cout << endl;

    } // End of 'try' block.

    HEIMDALI_CATCH_EXCEPTIONS_WITHOUT_ITK(argv[0])
}
