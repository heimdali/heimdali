#ifndef HEIMDALI_CMDHELPERER_HXX_INCLUDED
#define HEIMDALI_CMDHELPERER_HXX_INCLUDED

#include <string>

#include "itkImageIOBase.h"
#include "itkObjectFactoryBase.h"

#include "heimdali/cmdreader.hxx"

namespace Heimdali {

itk::ImageIOBase::Pointer
open_from_stdin_or_file(const string inputFilename);

bool
is_floating_point_type(itk::ImageIOBase::Pointer io);

void
read_information(itk::ImageIOBase::Pointer io, string name, unsigned int& value);

void
read_information(itk::ImageIOBase::Pointer io, string name, float& value);

void
read_information(itk::ImageIOBase::Pointer io, string name, bool& value);

itk::ImageIOBase::IOComponentType 
map_to_itk_component_type(bool is_floating_point_type,
                          unsigned int component_size);

}

#endif
