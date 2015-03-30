#ifndef UTIL_HXX_INCLUDED
#define UTIL_HXX_INCLUDED

#include <string>
#include <sstream>

namespace Heimdali {

bool
is_float(std::string str);

};

bool file_exists(const std::string filename);

#endif

