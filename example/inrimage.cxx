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

    int sz = image.getDim(Heimdali::INR_ALONGZ);
    int sy = image.getDim(Heimdali::INR_ALONGY);
    int sx = image.getDim(Heimdali::INR_ALONGX);
    int sv = image.getDim(Heimdali::INR_ALONGV);

    cout << "sz: " << sz << endl;
    cout << "sy: " << sy << endl;
    cout << "sx: " << sx << endl;
    cout << "sv: " << sv << endl;

    int iz = 2;
    image.read(iz);

    for (int iy = 0 ; iy < sy ; ++iy) {
        for (int ix = 0 ; ix < sx ; ++ix) {
            cout << "image[" << iz << "," << iy << "," << ix << "] = ";
            for (int iv = 0 ; iv < sv ; ++iv) {
                cout << image(ix,iy,iz=0,iv=iv) << " ";
            }
            cout << endl;
        }
    }

    return 0;
}
