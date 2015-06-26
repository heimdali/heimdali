#ifndef INRIMAGE2_CLI_HXX_INCLUDED
#define INRIMAGE2_CLI_HXX_INCLUDED

#include <vector>
#include <string>

#include <tclap/CmdLine.h>

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE-IN] [IMAGE-OUT]",parser);

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE0-IN] [IMAGE1-IN] [IMAGE-OUT]",parser);

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE-IN] [IMAGE0-OUT] [IMAGE1-OUT]",parser);

#define HEIMDALI_TCLAP_IMAGE_IN_IMAGE_IN_IMAGE_OUT_IMAGE_OUT(filenamesArgs,parser) TCLAP::UnlabeledMultiArg<string> filenamesArg("filenames", "Input and ouput images",false,"[IMAGE0-IN] [IMAGE1-IN] [IMAGE0-OUT] [IMAGE1-OUT]",parser);


namespace Heimdali{

class PixelTypeArgParser
{
    public:
        PixelTypeArgParser();

        void
        parse(TCLAP::SwitchArg* floating_switch,
              TCLAP::SwitchArg* fixed_switch,
              TCLAP::ValueArg<int>* nbytes_arg,
              TCLAP::ValueArg<int>* nbits_arg=NULL);

        bool is_floating_point;
        bool is_fixed_point;
        bool is_binary;
        int nbytes;
};

std::ostream &operator<<(std::ostream &os, PixelTypeArgParser const &p) { 
    return os 
        << "is_floating_point: " << p.is_floating_point << std::endl
        << "is_fixed_point: " << p.is_fixed_point << std::endl
        << "number of bytes: " << p.nbytes << std::endl
        << "is_binary: " << p.is_binary;
}

std::vector<std::string>
preprocess_argv(int argc, char** inrimage_argv);

unsigned int
count_arguments(std::vector<std::string> argv);

std::vector<std::string>
consume_option_values(std::string option,
                      std::vector<std::string> & argv,
                      int nconsumed_max);

void
parse_tclap_image_in_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename,
                               std::string& outputFilename);
void
parse_tclap_image_in_image_in_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename0,
                               std::string& inputFilename1,
                               std::string& outputFilename);

void
parse_tclap_image_in_image_out_image_out(TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
                               std::string& inputFilename,
                               std::string& outputFilename0,
                               std::string& outputFilename1);

void
parse_tclap_image_in_image_in_image_out_image_out(
       TCLAP::UnlabeledMultiArg<std::string>& filenamesArg,
       std::string& inputFilename0, std::string& inputFilename1,
       std::string& outputFilename0, std::string& outputFilename1);

bool
is_floating_point_type(bool floatingSwitchIsSet, bool fixedSwitchIsSet,
                       bool defaultsToFloating = true);

};

#endif
