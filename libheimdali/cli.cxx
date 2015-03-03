/* Command Line Interface */

#include <sstream>

#include "heimdali/cli.hxx"

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
        throw(TCLAP::ArgException(error_msg.str()));
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
        throw(TCLAP::ArgException(error_msg.str()));
    }
}

};
