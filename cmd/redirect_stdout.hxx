#ifndef INRIMAGE2_REDIRECT_STDOUT_HXX_INCLUDED
#define INRIMAGE2_REDIRECT_STDOUT_HXX_INCLUDED

#include <string>
#include <fstream>
#include <iostream>

namespace Heimdali {

class RedirectStdout
{
    public:
        RedirectStdout(std::string filename);
        ~RedirectStdout();

    private:
        enum Destination {dest_cout, dest_cerr, dest_file};

        std::streambuf* m_cout_rdbuf;
        Destination m_dest;
        std::ofstream m_file;
};

};

#endif
