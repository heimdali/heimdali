#include "heimdali/util.hxx"

#include <fstream>

using namespace std;

namespace Heimdali {

// Taken from http://stackoverflow.com/a/447307
bool
is_float(string str)
{
    istringstream iss(str);
    float f;
    // noskipws considers leading whitespace invalid.
    iss >> noskipws >> f;
    // Check the entire string was consumed and if either
    // failbit or badbit is set.
    return iss.eof() && !iss.fail(); 
}

bool file_exists(const std::string filename) {
    ifstream f(filename.c_str());
    bool answer = f.good();
    f.close();
    return answer;
}

}
