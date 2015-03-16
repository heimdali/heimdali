#ifndef HEIMDALI_CMDHELPERER_HXX_INCLUDED
#define HEIMDALI_CMDHELPERER_HXX_INCLUDED

#include <string>

#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"

#include "heimdali/cmdreader.hxx"

namespace Heimdali {

itk::ImageIOBase::Pointer
read_informations(std::string filename);

}

#endif
