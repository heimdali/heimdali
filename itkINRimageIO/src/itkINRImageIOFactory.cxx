#include "itkINRImageIOFactory.h"
#include "itkINRImageIO.h"
#include "itkVersion.h"

namespace itk
{
void INRImageIOFactory::PrintSelf(std::ostream &, Indent) const
{}

INRImageIOFactory::INRImageIOFactory()
{
  this->RegisterOverride( "itkImageIOBase",
                          "itkINRImageIO",
                          "INR Image IO",
                          1,
                          CreateObjectFunction< INRImageIO >::New() );
}

INRImageIOFactory::~INRImageIOFactory()
{}

const char *
INRImageIOFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char *
INRImageIOFactory::GetDescription() const
{
  return "INR ImageIO Factory, allows the loading of INR images into insight";
}

// Undocumented API used to register during static initialization.
// DO NOT CALL DIRECTLY.

static bool INRImageIOFactoryHasBeenRegistered;

void INRImageIOFactoryRegister__Private(void)
{
  if( ! INRImageIOFactoryHasBeenRegistered )
    {
    INRImageIOFactoryHasBeenRegistered = true;
    INRImageIOFactory::RegisterOneFactory();
    }
}

} // end namespace itk

