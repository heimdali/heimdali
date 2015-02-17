#include <iostream>

#include "heimdali/inrimage.hxx"

using namespace std;

typedef float PixelType;
typedef Heimdali::InrImage<PixelType> ImageType;

PixelType
imtest_value(int iz, int iy, int ix, int iv)
{
    return iz*1000 + iy*100 + ix*10 + iv;
}

bool
check_plane(ImageType& image, int offsetz, int iz, int sy, int sx, int sv)
{
    PixelType value;
    PixelType expected_value;
    for (int iy = 0 ; iy < sy ; ++iy) {
    for (int ix = 0 ; ix < sx ; ++ix) {
    for (int iv = 0 ; iv < sv ; ++iv) {
        value = image(ix,iy,iz=iz,iv=iv);
        expected_value = imtest_value(offsetz+iz,iy,ix,iv);
        if (value != expected_value) {
            cerr << "ERROR: image[" << iz << "," << iy << "," 
                 << ix << "," << iv << "]"
                 << " value is " << value 
                 << ", but expected " << expected_value << endl;
            return false;
        }
    }}}
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " IMAGE-IN" << endl;
        return 1;
    }
    string filename = argv[1];

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

    int offsetz = 2;
    int iz = 0;
    image.read(offsetz);

    if (! check_plane(image,offsetz,iz,sy,sx,sv))
        return 1;

    return 0;
}
