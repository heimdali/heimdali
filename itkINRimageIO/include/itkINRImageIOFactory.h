#ifndef __itkINRImageIOFactory_h
#define __itkINRImageIOFactory_h


#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

namespace itk
{
/** \class INRImageIOFactory
   * \author David Froger
   * \brief Create instances of INRImageIO objects using an object factory.
   * \ingroup ITKIOINR
   */
class ITK_EXPORT INRImageIOFactory:public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef INRImageIOFactory          Self;
  typedef ObjectFactoryBase          Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Class methods used to interface with the registered factories. */
  virtual const char * GetITKSourceVersion(void) const;

  virtual const char * GetDescription(void) const;

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(INRImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void)
  {
    INRImageIOFactory::Pointer metaFactory = INRImageIOFactory::New();

    ObjectFactoryBase::RegisterFactoryInternal(metaFactory);
  }

protected:
  INRImageIOFactory();
  ~INRImageIOFactory();
  virtual void PrintSelf(std::ostream & os, Indent indent) const;

private:
  INRImageIOFactory(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented
};
} // end namespace itk

#endif
