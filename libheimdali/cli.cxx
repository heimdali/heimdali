/* Command Line Interface */

#include <sstream>

#include "tclap/ArgException.h"

#include "heimdali/cli.hxx"
#include "heimdali/util.hxx"
#include "heimdali/error.hxx"

using namespace std;

namespace Heimdali {

PixelTypeArgParser::PixelTypeArgParser():
    is_floating_point(false),
    is_fixed_point(true),
    is_binary(false),
    nbytes(1)
{
}

void
PixelTypeArgParser::parse(TCLAP::SwitchArg* floating_switch,
                          TCLAP::SwitchArg* fixed_switch,
                          TCLAP::ValueArg<int>* nbytes_arg,
                          TCLAP::ValueArg<int>* nbits_arg)
{
    // Check for incompatible args.
    if (floating_switch->isSet() && fixed_switch->isSet())
        throw(TCLAP::ArgException("-r and -f flags are incompatible."));

    if (nbits_arg != NULL && nbits_arg->isSet()) {
        if (floating_switch->isSet())
            throw(TCLAP::ArgException("-r and -b flags are incompatible."));

        if (nbytes_arg->isSet())
            throw(TCLAP::ArgException("-o and -b flags are incompatible."));
    }

    // Process floating_switch switch.
    if (floating_switch->isSet()) {
        is_floating_point = true;
        is_fixed_point = false;
        if (nbytes_arg->isSet()) {
            switch (nbytes_arg->getValue())
            {
            case 4:
                nbytes = 4;
                break;
            case 8:
                nbytes = 8;
                break;
            default:
                throw(TCLAP::ArgException("-o for floating points must be 4 or 8."));
            }
        } else {
            nbytes = 4;
        }

    // Process fixed switch.
    } else {
        is_floating_point = false;
        is_fixed_point = true;
        if (nbytes_arg->isSet()) {
            switch (nbytes_arg->getValue())
            {
            case 1:
                nbytes = 1;
                break;
            case 2:
                nbytes = 2;
                break;
            case 4:
                nbytes = 4;
                break;
            default:
                throw(TCLAP::ArgException("-o for fixed points must be 1, 2 or 4."));
            }
        } else if (nbits_arg->isSet()) {
            if (nbits_arg->getValue() == 8) {
                is_binary = true;
                nbytes = 1;
            }
            else  {
                throw(TCLAP::ArgException("Only -b 8 is possible."));
            }
        } else {
            nbytes = 1;
        }
    }
}

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

/* Consums and returns command line option values.
 *
 * For example, with the command line invocation:
 *     './main -n 0 1 arg1 arg2', argv is:
 *
 * the argv argument is:
 *     ["./main", "-n", "0", "1", "arg1", "arg2"]:
 *
 * After calling:
 *     vector<string> values = consume_options_values("-n", argv, 2);
 *
 * argv become:
 *     ["./main", "arg1", "arg2"]:
 *
 * and the returned value 'values' is:
 *     ["0", "1"]
 */
vector<string>
consume_option_values(string option, vector<string> & argv, int nconsumed_max)
{
    char dash = '-';
    bool consuming = false;
    int nargs = argv.size();
    vector<string> consumed_args;
    int consuming_start = nargs;
    int consuming_end = nargs;
    for (int iarg = 0 ; iarg < nargs ; iarg++) {
        string arg = argv[iarg];

        if (consuming && arg[0] == dash || consumed_args.size() == nconsumed_max) {
            consuming_end = iarg;
            break;
        }

        if (consuming) {
            consumed_args.push_back(arg);
        }

        if (arg == option) {
            consuming = true;
            consuming_start = iarg;
        }
    }
    
    argv.erase(argv.begin() + consuming_start,
               argv.begin() + consuming_end);

    return consumed_args;
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
is_floating_point_type(bool floatingSwitchIsSet, bool fixedSwitchIsSet, bool defaultsToFloating) 
{
    if (fixedSwitchIsSet && floatingSwitchIsSet) {
        throw(TCLAP::ArgException("-r and -f flags are incompatible"));
    } else if (floatingSwitchIsSet) {
        return true;
    } else if (fixedSwitchIsSet) {
        return false;
    } else {
        return defaultsToFloating;
    }
}

};
