#ifndef INRIMAGE2_CLI_HXX_INCLUDED
#define INRIMAGE2_CLI_HXX_INCLUDED

#include <vector>
#include <string>

#include <tclap/CmdLine.h>

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE-IN] [IMAGE-OUT]",parser);

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE0-IN] [IMAGE1-IN] [IMAGE-OUT]",parser);


namespace Heimdali{

std::vector<std::string>
preprocess_argv(int argc, char** inrimage_argv);

void
parse_tclap_image_in_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename,
                               std::string& outputFilename);
void
parse_tclap_image_in_image_in_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename0,
                               std::string& inputFilename1,
                               std::string& outputFilename);
};


#endif
