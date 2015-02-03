#ifndef INRIMAGE2_REDIRECT_STDOUT_HXX_INCLUDED
#define INRIMAGE2_REDIRECT_STDOUT_HXX_INCLUDED

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Heimdali {

class RedirectStdout
{
    public:
        RedirectStdout(std::string filename, bool force);
        ~RedirectStdout();

    private:
        enum Destination {dest_cout, dest_cerr, dest_file};
        bool m_force;

        std::streambuf* m_cout_rdbuf;
        Destination m_dest;
        std::ofstream m_file;
};

};

#endif
