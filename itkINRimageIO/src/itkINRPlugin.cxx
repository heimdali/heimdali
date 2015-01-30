#include "itkINRPlugin.h"
#include "itkINRImageIOFactory.h"

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */
itk::ObjectFactoryBase* itkLoad()
{
  static itk::INRImageIOFactory::Pointer f = itk::INRImageIOFactory::New();
  return f;
}
