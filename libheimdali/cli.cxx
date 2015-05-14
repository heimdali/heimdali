/* Command Line Interface */

#include <sstream>

#include "tclap/ArgException.h"

#include "heimdali/cli.hxx"
#include "heimdali/util.hxx"
#include "heimdali/error.hxx"

using namespace std;

namespace Heimdali {

/* Convert INRimage command line argument format to TCLAP format.
 *
 * Long argument starting with only one dash '-' must be fixed by
 * prepending a second dash.
 *
 * For example:
 *     -foo need to have a dash prepended to --foo .
 *     --foo does not need to have a dash prepended.
 *     -f does not need to have a dash prepended.
 */
vector<string>
preprocess_argv(int argc, char** inrimage_argv)
{
    vector<string> tclap_argv;
    string arg;
    char dash = '-';
    for (int iarg = 0 ; iarg < argc ; iarg++) {
        arg = (string) inrimage_argv[iarg];
        if (arg.size() >= 3 && arg[0] == dash && arg[1] != dash)
            arg = dash + arg;
        tclap_argv.push_back(arg);
    }
    return tclap_argv;
}

/* Counts command line arguments (ie, not flag).
 *
 * A word if not a flag if starts with '-' or if it is a float.
 *
 * For example:
 *   ./main first second         # 2 arguments
 *   ./main -f 5 first second    # 2 arguments
 *   ./main -f five first second # 3 arguments
 */
unsigned int
count_arguments(vector<string> argv)
{
    unsigned int nargs = 0;
    char dash = '-';
    string arg;
    for (unsigned int iarg = 1 ; iarg < argv.size() ; iarg++) {
        arg = argv[iarg];
        if (arg[0] != dash && ! Heimdali::is_float(arg)) nargs++;
    }
    return nargs;
}

/* Parse IMAGE-IN IMAGE-OUT command line arguments */
void
parse_tclap_image_in_image_out(TCLAP::UnlabeledMultiArg<string>& filenamesArg,
                               string& inputFilename,
                               string& outputFilename)
{
    vector<string> filenames = filenamesArg.getValue();
    ostringstream error_msg;
    switch (filenames.size())
    {
    case(2):
        inputFilename = filenames[0];
        outputFilename = filenames[1];
        break;
    case(1):
        inputFilename = filenames[0];
        outputFilename = "-";
        break;
    case(0):
        inputFilename = "-";
        outputFilename = "-";
        break;
    default:
        error_msg << "Expected two, one, on zero file names, but got " << filenames.size();
        throw(Heimdali::Exception(error_msg.str()));
    }
}

/* Parse IMAGE0-IN IMAGE1-IN IMAGE-OUT command line arguments */
void
parse_tclap_image_in_image_in_image_out(TCLAP::UnlabeledMultiArg<string>& filenamesArg,
                               string& inputFilename0,
                               string& inputFilename1,
                               string& outputFilename)
{
    vector<string> filenames = filenamesArg.getValue();
    ostringstream error_msg;
    switch (filenames.size())
    {
    case(3):
        inputFilename0 = filenames[0];
        inputFilename1 = filenames[1];
        outputFilename = filenames[2];
        break;
    case(2):
        inputFilename0 = filenames[0];
        inputFilename1 = filenames[1];
        outputFilename = "-";
        break;
    case(1):
        inputFilename0 = filenames[0];
        inputFilename1 = "-";
        outputFilename = "-";
        break;
    default:
        error_msg << "Expected two, one, on zero file names, but got " << filenames.size();
        throw(Heimdali::Exception(error_msg.str()));
    }
}


/* Parse IMAGE-IN IMAGE0-OUT IMAGE1-OUT command line arguments */
void
parse_tclap_image_in_image_out_image_out(TCLAP::UnlabeledMultiArg<string>& filenamesArg,
                               string& inputFilename,
                               string& outputFilename0,
                               string& outputFilename1)
{
    vector<string> filenames = filenamesArg.getValue();
    ostringstream error_msg;
    switch (filenames.size())
    {
    case(3):
        inputFilename = filenames[0];
        outputFilename0 = filenames[1];
        outputFilename1 = filenames[2];
        break;
    default:
        error_msg << "Expected three file names, but got " << filenames.size();
        throw(Heimdali::Exception(error_msg.str()));
    }
}

/* Parse IMAGE0-IN IMAGE1-IN IMAGE0-OUT IMAGE1-OUT command line arguments */
void
parse_tclap_image_in_image_in_image_out_image_out(
       TCLAP::UnlabeledMultiArg<string>& filenamesArg,
       string& inputFilename0, string& inputFilename1,
       string& outputFilename0, string& outputFilename1)
{
    vector<string> filenames = filenamesArg.getValue();
    ostringstream error_msg;
    switch (filenames.size())
    {
    case(4):
        inputFilename0 = filenames[0];
        inputFilename1 = filenames[1];
        outputFilename0 = filenames[2];
        outputFilename1 = filenames[3];
        break;
    default:
        error_msg << "Expected four file names, but got " << filenames.size();
        throw(Heimdali::Exception(error_msg.str()));
    }
}

bool
is_floating_point_type(bool floatingSwitchIsSet, bool fixedSwitchIsSet) 
{
    if (fixedSwitchIsSet) {
        if (floatingSwitchIsSet) 
            throw(TCLAP::ArgException("-r and -f flags are incompatible"));
        return false;
    } else {
        return true;
    }
}

};
