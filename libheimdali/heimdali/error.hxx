#ifndef COMMON_HXX
#define COMMON_HXX

#include <string>

using namespace std;

namespace Heimdali {

class ValueError {
    public:
        ValueError(const string& msg) : msg_(msg) {}
        ~ValueError() {}

        string getMessage() const {return(msg_);}
    private:
        string msg_;
};

class IOError {
    public:
        IOError(const string& msg) : msg_(msg) {}
        ~IOError() {}

        string getMessage() const {return(msg_);}
    private:
        string msg_;
};

class NotImplementedError {
    public:
        NotImplementedError(const string& msg) : msg_(msg) {}
        ~NotImplementedError() {}

        string getMessage() const {return(msg_);}
    private:
        string msg_;
};

};

#endif
