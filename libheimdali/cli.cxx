/* Command Line Interface */

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
vector<string> preprocess_argv(int argc, char** inrimage_argv)
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

};
