#ifndef INRIMAGE2_CLI_HXX_INCLUDED
#define INRIMAGE2_CLI_HXX_INCLUDED

#include <vector>
#include <string>

#include <tclap/CmdLine.h>

namespace Heimdali{

std::vector<std::string>
preprocess_argv(int argc, char** inrimage_argv);

void
parse_tclap_image_in_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename,
                               std::string& outputFilename);
};

#endif
