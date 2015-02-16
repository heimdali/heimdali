#include "heimdali/inrimage.hxx"

int main()
{
    typedef float PixelType;
    typedef Heimdali::InrImage<PixelType> ImageType;
    ImageType image = ImageType("image.h5");
}
