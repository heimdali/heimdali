#ifndef INRIMAGE_HXX_INCLUDED
#define INRIMAGE_HXX_INCLUDED

#include <string>

namespace Heimdali {

template <typename PixelType>
class InrImage 
{
public:
    InrImage();
    InrImage(std::string filename);
private:
    std::string m_filename;

};

};

#include "inrimage.txx"

#endif
