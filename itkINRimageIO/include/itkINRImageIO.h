#ifndef __itkINRImageIO_h
#define __itkINRImageIO_h

#include <string>
#include <stdio.h>
#include <setjmp.h>
#include <iostream>

#include "itkMacro.h"

#include "itkImageIOBase.h"

#include "itkImageIORegion.h"

#include "inrimage/image"
#include "inrimage/error.h"

using namespace std;

namespace itk
{
class ITK_EXPORT INRImageIO: public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef INRImageIO          Self;
  typedef ImageIOBase          Superclass;
  typedef SmartPointer< Self > Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(INRImageIO, Superclass);

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine if the file can be read with this ImageIO implementation.
   * \author Hans J Johnson
   * \param FileNameToRead The name of the file to test for reading.
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can read the file specified.
   */
  virtual bool CanReadFile(const char *FileNameToRead);

  /** Set the spacing and dimension information for the set filename. */
  virtual void ReadImageInformation();

  /** Reads the data from disk into the memory buffer provided. */
  virtual void Read(void *buffer);

  virtual bool CanStreamRead();

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine if the file can be written with this ImageIO implementation.
   * \param FileNameToWrite The name of the file to test for writing.
   * \author Hans J. Johnson
   * \post Sets classes ImageIOBase::m_FileName variable to be FileNameToWrite
   * \return Returns true if this ImageIO can write the file specified.
   */
  virtual bool CanWriteFile(const char *FileNameToWrite);

  /** Set the spacing and dimension information for the set filename. */
  virtual void WriteImageInformation();

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  virtual void Write(const void *buffer);

  virtual bool CanStreamWrite();

  /*-------- This part is specific to INR images . ----- */

  /*
  void SetInrArgcArgv(const int InrArgc, ...);
  void SetInrArgcArgv(const int InrArgc, const char** InrArgv);
  */

  void SetInrProgVers(const char* InrProgVers);
  void SetInrUcmd(const char* InrUcmd);
  void SetInrUdetail(const char* InrUdetail);

  void SetInrMode(const char* InrMode);
  void SetInrVerif(const char* InrVerif);

  virtual ImageIORegion
  GenerateStreamableReadRegionFromRequestedRegion(const ImageIORegion & requestedRegion) const;


protected:
  INRImageIO();
  ~INRImageIO();

  // virtual SizeType GetHeaderSize(void) const;

  void PrintSelf(std::ostream & os, Indent indent) const;

  /*-------- This part is specific to INR images . ----- */
  Inrimage* m_InrImage;
  Fort_int m_InrFmt[9];

  void FreeInrArgcArgv();
  unsigned int m_InrArgc;
  char** m_InrArgv;

  string m_InrProgVers;
  string m_InrUcmd;
  string m_InrUdetail;

  string m_InrMode;
  string m_InrVerif;

  void InrInit();
  bool m_InrIsInitialized;

private:
  INRImageIO(const Self &);   //purposely not implemented
  void operator=(const Self &); //purposely not implemented

};
} // end namespace itk

#endif // __itkINRImageIO_h
