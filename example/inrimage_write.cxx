#include "heimdali/inrimage.hxx"
#include "heimdali/itkhelper.hxx"

using namespace std;

typedef float PixelType;
typedef Heimdali::InrImage<PixelType> InrImageType;
typedef itk::VectorImage<PixelType,3> ImageType;

/* This example write to the disk a new image of size:
 *     nz = 5
 *     ny = 5
 *     nx = 3
 *     nv = 2
 *  and pixel values are : iz*1000 + iy*100 + ix*10 + iv
 *
 * The image is written by iterating on the planes, holding in the program
 * memory only a buffer of the size of one plane.
 */

//! Pixels of imtest_z5_y4_x3_c2.h5 have been set with these values.
PixelType
imtest_value(int iz, int iy, int ix, int iv)
{
    return iz*1000 + iy*100 + ix*10 + iv;
}

//! Check size of the image buffer allocated by ITK is as expected.
bool
check_buffered_region(string label, InrImageType& image,
                      int ix, int sx, int iy, int sy, int offsetz, int nz)
{
    ImageType::RegionType expected_region = Heimdali::CreateRegion(ix,sx,iy,sy,offsetz,nz);
    if (image.getImage()->GetBufferedRegion() != expected_region) {
        cerr << label << ": "
             << "buffered region is " << image.getImage()->GetBufferedRegion()
             << ", but expected: " << expected_region << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    // Parse command line.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " IMAGE-OUT" << endl;
        return 1;
    }
    string filename = argv[1];


    // Set up image for write.
    int sz=5, sy=4, sx=3, sv=2;
    int nz = 1;
    InrImageType image = InrImageType(sx,sy,sz,sv);
    image.setFilename(filename);
    image.setRealz(nz);
    image.openForWrite();

    // Write to disk plane by plane.
    int iz=0;
    for (int offsetz=0 ; offsetz<sz ; ++offsetz) {
        for (int iy = 0 ; iy < sy ; ++iy) {
        for (int ix = 0 ; ix < sx ; ++ix) {
        for (int iv = 0 ; iv < sv ; ++iv) {
            image(ix,iy,iz,iv) = imtest_value(offsetz+iz,iy,ix,iv);
        }}}
        image.write(offsetz);
        check_buffered_region("image", image, 0, sx, 0, sy, 0, nz);
    }

    return 0;
}
