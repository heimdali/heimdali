#include <iostream>
#include <sstream>

#include "heimdali/inrimage.hxx"
#include "heimdali/itkhelper.hxx"

/* This example read the file imtest_z5_y4_x3_c2.h5 using the class InrImage,
 * which API is designed to be compatiable with Inr++.
 *
 * The file imtest_z5_y4_x3_c2.h5 has size:
 *     nz = 5
 *     ny = 5
 *     nx = 3
 *     nv = 2
 *  and pixel values are : iz*1000 + iy*100 + ix*10 + iv
 *
 * This example test that InrImage is able to correctly read 1 or more plane
 * in imtest_z5_y4_x3_c2.h5, without reading the full image.
  */

using namespace std;

typedef float PixelType;
typedef Heimdali::InrImage<PixelType> InrImageType;
typedef itk::VectorImage<PixelType,3> ImageType;

//! Pixels of imtest_z5_y4_x3_c2.h5 have been set with these values.
PixelType
imtest_value(int iz, int iy, int ix, int iv)
{
    return iz*1000 + iy*100 + ix*10 + iv;
}

//! Check values of plane passed as argument are as expected.
bool
check_plane(InrImageType& image, int offsetz, int iz, int sy, int sx, int sv)
{
    PixelType value;
    PixelType expected_value;
    for (int iy = 0 ; iy < sy ; ++iy) {
    for (int ix = 0 ; ix < sx ; ++ix) {
    for (int iv = 0 ; iv < sv ; ++iv) {
        value = image(ix,iy,iz,iv);
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
        cerr << "Usage: " << argv[0] << " imtest_z5_y4_x3_c2.h5" << endl;
        return 1;
    }
    string filename = argv[1];

    // Set up image for read.
    InrImageType image = InrImageType(filename);
    image.setRealz(1);
    image.openForRead(); 

    // Read image dimensions
    int sz = image.getDim(Heimdali::INR_ALONGZ);
    int sy = image.getDim(Heimdali::INR_ALONGY);
    int sx = image.getDim(Heimdali::INR_ALONGX);
    int sv = image.getDim(Heimdali::INR_ALONGV);
    if (sz != 5) {
        cerr << "Expected 5 planes, but got " << sz << endl;
        return 1;
    }
    if (sy != 4) {
        cerr << "Expected 4 rows, but got " << sy << endl;
        return 1;
    }
    if (sx != 3) {
        cerr << "Expected 3 columns, but got " << sx << endl;
        return 1;
    }
    if (sv != 2) {
        cerr << "Expected 2 components, but got " << sv << endl;
        return 1;
    }

    // Read plane 2.
    int offsetz = 2;
    int iz=0, iy=0, ix=0;
    int nz=1;
    image.read(offsetz);
    if (! check_plane(image,offsetz,iz,sy,sx,sv)) return 1;
    if (! check_buffered_region("plane 2",image,ix,sx,iy,sy,offsetz,nz) ) return 1;

    // Read plane 0.
    offsetz = 0;
    image.read(offsetz);
    if (! check_plane(image,offsetz,iz,sy,sx,sv)) return 1;
    if (! check_buffered_region("plane 0",image,ix,sx,iy,sy,offsetz,nz) ) return 1;

    // Read planes 1, 2 and 3.
    offsetz = 1;
    nz = 3;
    image.read(offsetz, nz);
    ostringstream label;
    for (iz = 0 ; iz < nz ; ++iz) {
        label << "plane " << iz << " of (1,2,3)";
        if (! check_plane(image,offsetz,iz,sy,sx,sv)) return 1;
        if (! check_buffered_region(label.str(),image,ix,sx,iy,sy,offsetz,nz) ) return 1;
    }

    // Read all planes.
    offsetz = 0;
    nz = 5;
    image.read();
    for (iz = 0 ; iz < nz ; ++iz) {
        label << "plane " << iz << " of all";
        if (! check_plane(image,offsetz,iz,sy,sx,sv)) return 1;
        if (! check_buffered_region(label.str(),image,ix,sx,iy,sy,offsetz,nz) ) return 1;
    }

    cout << "All test passed successfully." << endl;
    return 0;
}
