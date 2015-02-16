#include "heimdali/inrimage.hxx"

namespace Heimdali {

template <typename PixelType>
InrImage<PixelType>::InrImage():
    m_filename("")
{
}

template <typename PixelType>
InrImage<PixelType>::InrImage(std::string filename):
    m_filename(filename)
{
}

}

