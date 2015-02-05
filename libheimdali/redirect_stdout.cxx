#include "heimdali/redirect_stdout.hxx"
#include "heimdali/error.hxx"

using namespace std;

namespace Heimdali {

RedirectStdout::RedirectStdout(string filename, bool force):
    m_force(force)
{
    m_cout_rdbuf = cout.rdbuf();

    bool isblank = filename.find_first_not_of("\t\n ") == string::npos;

    if (filename == "stdout" or isblank) {
        m_dest = dest_cout;

    } else if (filename == "stderr") {
        cout.rdbuf( cerr.rdbuf() );
        m_dest = dest_cerr;

    } else {
        // If the filename starts with '>>' (double greater than), append
        // to filename
        bool startswith_double_greater_than = (filename.compare(0,2,">>")
                                               == 0);
        const char* outputFilename;
        ios_base::openmode mode;
        if (startswith_double_greater_than) {
            outputFilename = filename.substr(2).c_str();
            mode = ios::app;
        } else {
            outputFilename = filename.c_str();
            mode = ios::trunc;
        }

        // Manage case of already exsting file.
        ifstream ifile(outputFilename);
        if (ifile.is_open() && ! m_force) {
            ostringstream msg;
            msg << "Refuse to erase existing file: '" << outputFilename << "'. "
                << "You may want to use --force option.";
            throw(IOError(msg.str()));
        }

        // Write to file.
        m_file.open(outputFilename, mode);
        if (! m_file.is_open()) {
            ostringstream msg;
            msg << "Unable to open file: '" << outputFilename << "'. ";
            throw(IOError(msg.str()));
        }

        cout.rdbuf( m_file.rdbuf());
        m_dest = dest_file;
    }
}

RedirectStdout::~RedirectStdout()
{
    if (m_dest != dest_cout) {
        cout.rdbuf( m_cout_rdbuf );
    }

    if (m_dest == dest_file) {
        m_file.close();
    }
}

};
