#include <string>

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
imtest_value(unsigned int iz, unsigned int iy, unsigned int ix, unsigned int iv)
{
    return iz*1000 + iy*100 + ix*10 + iv;
}

//! Check size of the image buffer allocated by ITK is as expected.
bool
check_buffered_region(string label, InrImageType& image,
                      unsigned int ix, unsigned int sx,
                      unsigned int iy, unsigned int sy,
                      unsigned int offsetz, unsigned int nz)
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

//! Write image to file, iterating on planes and holding in memory only one
//  plane.
bool
write_one_plane_a_time(unsigned int sz, unsigned int sy, unsigned int sx,
                       unsigned int sv, string filename)
{
    unsigned int nz = 1;
    InrImageType image = InrImageType(sx,sy,sz,sv);
    image.setFilename(filename);
    image.setRealz(nz);
    image.openForWrite();

    // Write to disk plane by plane.
    unsigned int iz=0;
    for (unsigned int offsetz = 0 ; offsetz < sz ; ++offsetz) {
        for (unsigned int iy = 0 ; iy < sy ; ++iy) {
        for (unsigned int ix = 0 ; ix < sx ; ++ix) {
        for (unsigned int iv = 0 ; iv < sv ; ++iv) {
            image(ix,iy,iz,iv) = imtest_value(offsetz+iz,iy,ix,iv);
        }}}
        image.write(offsetz);
        if (! check_buffered_region("one_plane_a_time", image, 0, sx, 0, sy, 0, nz)) return false;
    }
    return true;
}

//! Allocate and fill the full image, and write it in file.
bool
write_full_image(unsigned int sz, unsigned int sy, unsigned int sx,
                 unsigned int sv, string filename)
{
    InrImageType image = InrImageType(sx,sy,sz,sv);
    image.setFilename(filename);
    image.setRealz(sz);
    image.openForWrite();

    // Write to disk plane by plane.
    for (unsigned int iz = 0 ; iz < sz ; ++iz) {
    for (unsigned int iy = 0 ; iy < sy ; ++iy) {
    for (unsigned int ix = 0 ; ix < sx ; ++ix) {
    for (unsigned int iv = 0 ; iv < sv ; ++iv) {
        image(ix,iy,iz,iv) = imtest_value(iz,iy,ix,iv);
    }}}}

    image.write();
    if (! check_buffered_region("full_image", image, 0, sx, 0, sy, 0, sz)) return false;
    return true;
}

bool
write_three_then_two_planes(unsigned int sz, unsigned int sy,
                            unsigned int sx, unsigned int sv, string filename)
{
    unsigned int nz3 = 3;
    unsigned int nz2 = 2;
    InrImageType image = InrImageType(sx,sy,sz,sv);
    image.setFilename(filename);
    image.setRealz(nz3);
    image.openForWrite();

    // Write 3 planes.
    unsigned int offsetz = 0;
    for (unsigned int iz = 0 ; iz < nz3 ; ++iz) {
    for (unsigned int iy = 0 ; iy < sy ; ++iy) {
    for (unsigned int ix = 0 ; ix < sx ; ++ix) {
    for (unsigned int iv = 0 ; iv < sv ; ++iv) {
        image(ix,iy,iz,iv) = imtest_value(offsetz+iz,iy,ix,iv);
    }}}}
    image.write(offsetz);
    if (! check_buffered_region("two_planes", image, 0, sx, 0, sy, 0, nz3)) return false;

    // Write 2 last planes.
    offsetz = nz3;
    for (unsigned int iz = 0 ; iz < nz2 ; ++iz) {
    for (unsigned int iy = 0 ; iy < sy ; ++iy) {
    for (unsigned int ix = 0 ; ix < sx ; ++ix) {
    for (unsigned int iv = 0 ; iv < sv ; ++iv) {
        image(ix,iy,iz,iv) = imtest_value(offsetz+iz,iy,ix,iv);
    }}}}
    image.write(offsetz,nz2);
    if (! check_buffered_region("three_planes", image, 0, sx, 0, sy, 0, 3)) return false;

    return true;
}

int main(int argc, char** argv)
{
    // Parse command line.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " IMAGE-OUT-EXTENSION" << endl;
        cerr << "    example: " << argv[0] << " h5" << endl;
        cerr << "    will prodcude files: inrimage_write_0.h5, inrimage_write_1.h5, inrimage_write_2.h5" << endl;
        return 1;
    }
    string ext = argv[1];

    // Set up image for write.
    unsigned int sz=5, sy=4, sx=3, sv=2;

    string filename = "inrimage_write_0." + ext;
    if (! write_one_plane_a_time(sz,sy,sx,sv,filename)) return 1;

    filename = "inrimage_write_1." + ext;
    if (! write_full_image(sz,sy,sx,sv,filename)) return 1;

    filename = "inrimage_write_2." + ext;
    if (! write_three_then_two_planes(sz,sy,sx,sv,filename)) return 1;

    return 0;
}
