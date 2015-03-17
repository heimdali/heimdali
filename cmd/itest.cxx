#include <sstream>

#include <tclap/CmdLine.h>

#include <itkINRImageIOFactory.h>

#include "heimdali/error.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/version.hxx"
#include "heimdali/cmdhelper.hxx"

#define ADD_TO_MAP(map,arg) if (arg.isSet()) map[arg.getName()] = arg.getValue()
#define HAS_KEY(map,key) map.find(key) != map.end()

using namespace std;

template <typename S, typename T>
void
test_value(string label, S value, T expected_value, ostringstream& msg)
{
    if (value != expected_value)
        msg << label << " are different: " << value 
            << " VS " << expected_value << " ." << endl;;
}

/* Fill keys and values of maps.
 */
void
parse_cli_one_filename(vector<string> tclap_argv,
                       string& inputFilename,
                       map<string, unsigned int>& map_uint,
                       map<string, bool>& map_bool,
                       map<string, float>& map_float)
{
    // Parse command line.
    TCLAP::CmdLine parser("Perform simple tests on image informations", ' ', HEIMDALI_VERSION);

    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"","IMAGE-IN", parser);

    // -z -y -x -v
    TCLAP::ValueArg<unsigned int> zValue("z","nplanes", "Number of planes",false,0,"NZ", parser);
    TCLAP::ValueArg<unsigned int> yValue("y","nrows", "Number of rows",false,0,"NY", parser);
    TCLAP::ValueArg<unsigned int> xValue("x","ncolumns", "Number of columns",false,0,"NX", parser);
    TCLAP::ValueArg<unsigned int> vValue("v","ncomponents", "Number of pixel components",false,0, "NV", parser);

    // -z0 -y0 -x0
    TCLAP::ValueArg<float> z0Value("","z0", "z origin",false,0,"Z0", parser);
    TCLAP::ValueArg<float> y0Value("","y0", "y origin",false,0,"Y0", parser);
    TCLAP::ValueArg<float> x0Value("","x0", "x origin",false,0,"X0", parser);

    // -o
    TCLAP::ValueArg<unsigned int> oValue("o","componentsize", "Number of bytes of a pixel component",false,0,"NBYTES", parser);

    // -f -r
    TCLAP::SwitchArg fSwitch("f","fixed-point", "Pixel type is fixed point number", parser);
    TCLAP::SwitchArg rSwitch("r","floating-point", "Pixel type is floating point number", parser);

    parser.parse(tclap_argv);

    inputFilename = inputFilenameArg.getValue();

    ADD_TO_MAP(map_uint, zValue);
    ADD_TO_MAP(map_uint, yValue);
    ADD_TO_MAP(map_uint, xValue);
    ADD_TO_MAP(map_uint, vValue);
    ADD_TO_MAP(map_uint, oValue);

    ADD_TO_MAP(map_float, z0Value);
    ADD_TO_MAP(map_float, y0Value);
    ADD_TO_MAP(map_float, x0Value);

    ADD_TO_MAP(map_bool, fSwitch);
    ADD_TO_MAP(map_bool, rSwitch);
}

/* Read keys of maps and fill its values.
 */
template <typename T>
void
read_image_info_from_file(const string filename, map<string, T>& m,
                          itk::ImageIOBase::Pointer io)
{

    for (typename map<string, T>::iterator
            it  = m.begin() ;
            it != m.end() ;
            it++)
    {
        string key = it->first;
        Heimdali::read_information(io, key, m[key]);
    }
}

template <typename T>
void
copy_map_keys(map<string,T>& src, map<string,T>& dst)
{
    for (typename map<string,T>::iterator
            it  = src.begin() ;
            it != src.end() ;
            it++)
    {
        string key = it->first;
        dst[key] = T();
    }
}

template <typename T>
void
compare_informations(map<string,T> A, map<string,T> B, ostringstream& msg)
{
    string key;
    T valueA, valueB;
    for (typename map<string, T>::iterator
            itA  = A.begin() ;
            itA != A.end() ;
            itA++)
    {
        string key = itA->first;
        valueA = itA->second;
        valueB = B[key];
        if (valueA != valueB)
            msg << " " << key;
    }
}

int main(int argc, char** argv)
{
    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    // Put our INRimage reader in the list of readers ITK knows.
    itk::ObjectFactoryBase::RegisterFactory( itk::INRImageIOFactory::New() ); 

    map<string, unsigned int> map_uintA, map_uintB;
    map<string, bool> map_boolA, map_boolB;
    map<string, float> map_floatA, map_floatB;
    string inputFilenameA, inputFilenameB;
    itk::ImageIOBase::Pointer io;

    ostringstream error_msg;

    unsigned int numberOfInputFilenames = Heimdali::count_arguments(tclap_argv);

    switch (numberOfInputFilenames)
    {
    case 1:
        parse_cli_one_filename(tclap_argv, inputFilenameB, map_uintA, map_boolA, map_floatA);
        copy_map_keys(map_uintA, map_uintB);
        copy_map_keys(map_boolA, map_boolB);
        copy_map_keys(map_floatA, map_floatB);
        io = Heimdali::read_informations(inputFilenameB);
        read_image_info_from_file(inputFilenameB, map_uintB, io);
        read_image_info_from_file(inputFilenameB, map_boolB, io);
        read_image_info_from_file(inputFilenameB, map_floatB, io);
        break;
    default:
        error_msg << "ERROR: One or two fileanmes expected, but got: " 
                  << numberOfInputFilenames << endl
                  << "To get help on usage with one filename, use: " << endl
                  << "    itest A --help" << endl
                  << "To get help on usage with two filename, use: " << endl
                  << "    itest A B --help";
        throw(Heimdali::ValueError(error_msg.str()));
        break;
    }

    // Compare informations.
    ostringstream msg;
    compare_informations(map_uintA, map_uintB, msg);
    compare_informations(map_boolA, map_boolB, msg);
    compare_informations(map_floatA, map_floatB, msg);

    // Output message and return code.
    if (msg.str().empty()) {
        cout << "Image informations are the same." << endl;
        return 0;
    } else {
        cout << "Image informations are different:" << msg.str() << endl;
        return 1;
    }

    } // End of 'try' block.

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "itest: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }

    catch (Heimdali::IOError &e) {
        cerr << "itest: ERROR: " << e.getMessage() << endl;
    }

    catch (Heimdali::ValueError &e) {
        cerr << "itest: ERROR: " << e.getMessage() << endl;
    }
}
