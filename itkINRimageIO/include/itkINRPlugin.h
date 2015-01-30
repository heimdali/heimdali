#ifndef __itkINRPlugin_h
#define __itkINRPlugin_h

#include "itkObjectFactoryBase.h"

#ifdef WIN32
#ifdef INRPlugin_EXPORTS
#define INRPlugin_EXPORT __declspec(dllexport)
#else
#define INRPlugin_EXPORT __declspec(dllimport)
#endif
#else
#define INRPlugin_EXPORT 
#endif

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */
extern "C" {
INRPlugin_EXPORT itk::ObjectFactoryBase* itkLoad();
} 
#endif  
