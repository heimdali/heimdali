#include <iostream>

#include "heimdali/inrimage.hxx"

using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " IMAGE-IN" << endl;
        return 1;
    }
    string filename = argv[1];

    typedef float PixelType;
    typedef Heimdali::InrImage<PixelType> ImageType;

    ImageType image = ImageType(filename);
    image.setRealz(1);
    image.openForRead(); 

    cout << "sz: " << image.getDim(Heimdali::INR_ALONGZ) << endl;
    cout << "sy: " << image.getDim(Heimdali::INR_ALONGY) << endl;
    cout << "sx: " << image.getDim(Heimdali::INR_ALONGX) << endl;
    cout << "sv: " << image.getDim(Heimdali::INR_ALONGV) << endl;

    return 0;
}
