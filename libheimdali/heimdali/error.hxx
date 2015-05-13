#ifndef COMMON_HXX
#define COMMON_HXX

#include <string>

using namespace std;

namespace Heimdali {

#define HEIMDALI_CATCH_EXCEPTIONS(name) \
    catch (TCLAP::ArgException &e) { \
        cerr << name << ": ERROR: " << e.error() << " for arg " << e.argId() << endl; \
        return 1; \
    }  \
    catch (Heimdali::Exception &e) { \
        cerr << name << ": ERROR: " << e.getMessage() << endl; \
        return 1; \
    } \
    catch( itk::ExceptionObject & err ) { \
        cerr << name << ": ERROR: " << err.what() << endl; \
        return 1; \
    } \
    return 0;

#define HEIMDALI_CATCH_EXCEPTIONS_WITHOUT_ITK(name) \
    catch (TCLAP::ArgException &e) { \
        cerr << name << ": ERROR: " << e.error() << " for arg " << e.argId() << endl; \
        return 1; \
    }  \
    catch (Heimdali::Exception &e) { \
        cerr << name << ": ERROR: " << e.getMessage() << endl; \
        return 1; \
    } \
    return 0;

class Exception {
    public:
        Exception(const string& msg) : msg_(msg) {}
        ~Exception() {}

        string getMessage() const {return(msg_);}
    private:
        string msg_;
};


};

#endif
