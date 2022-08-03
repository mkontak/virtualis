// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <math.h>
#include <utility>

#define USING_DICOM_NS    using namespace Viatronix::Dicom;
#define OPEN_DICOM_NS     namespace Viatronix {  namespace Dicom {
#define CLOSE_DICOM_NS    } } 


#define USING_DICOM_IMAGING_NS    using namespace Viatronix::Dicom::Imaging;
#define OPEN_DICOM_IMAGING_NS     namespace Viatronix {  namespace Dicom { namespace Imaging {
#define CLOSE_DICOM_IMAGING_NS    } } } 


#define BOUND(x,y,z)  ( (y) < (x) ? (x) : ( (y) > (z) ? (z) : (y) ) )


OPEN_DICOM_IMAGING_NS


/**
 * Implements a base exception used in the Dicom imaging library
 * All other exceptions should inherit from this.
 */
 public ref class DicomImagingException : public System::ApplicationException
{
public:
  /// Constructor
  DicomImagingException(System::String ^ msMessage) : System::ApplicationException(msMessage) { }

  /// Constructor
  DicomImagingException(System::String ^ msMessage, System::Exception ^ mpEx) : System::ApplicationException(msMessage, mpEx) { }

  /// Constructor
  DicomImagingException(System::Runtime::Serialization::SerializationInfo ^ mpInfo, System::Runtime::Serialization::StreamingContext context) : System::ApplicationException(mpInfo, context)  {  }

  /// Destructor
  virtual ~DicomImagingException() { }
};  // class DicomImagingException


/**
 * Implements a base exception used when no pixel data in a dico element list is a race condition
 */
 public ref class NoPixelDataException : public DicomImagingException
{
public:
  /// Constructor
  NoPixelDataException(System::String ^ msMessage) : DicomImagingException(msMessage) { }

  /// Constructor
  NoPixelDataException(System::String ^ msMessage, System::Exception ^ mpEx) : DicomImagingException(msMessage, mpEx) { }

  /// Constructor
  NoPixelDataException(System::Runtime::Serialization::SerializationInfo ^ mpInfo, System::Runtime::Serialization::StreamingContext context) : DicomImagingException(mpInfo, context)  {  }

  /// Destructor
  virtual ~NoPixelDataException() { }
};  // class NoPixelDataException

 /**
 * Implements a base exception used when no pixel data in a dico element list is a race condition
 */
 public ref class NoDicomImagesException : public DicomImagingException
{
public:
  /// Constructor
  NoDicomImagesException(System::String ^ msMessage) : DicomImagingException(msMessage) { }

  /// Constructor
  NoDicomImagesException(System::String ^ msMessage, System::Exception ^ mpEx) : DicomImagingException(msMessage, mpEx) { }

  /// Constructor
  NoDicomImagesException(System::Runtime::Serialization::SerializationInfo ^ mpInfo, System::Runtime::Serialization::StreamingContext context) : DicomImagingException(mpInfo, context)  {  }

  /// Destructor
  virtual ~NoDicomImagesException() { }
};  // class NoDicomImagesException

 /**
 * Implements a base exception used when no pixel data in a dico element list is a race condition
 */
 public ref class NotFoundException : public DicomImagingException
{
public:
  /// Constructor
  NotFoundException(System::String ^ msMessage) : DicomImagingException(msMessage) { }

  /// Constructor
  NotFoundException(System::String ^ msMessage, System::Exception ^ mpEx) : DicomImagingException(msMessage, mpEx) { }

  /// Constructor
  NotFoundException(System::Runtime::Serialization::SerializationInfo ^ mpInfo, System::Runtime::Serialization::StreamingContext context) : DicomImagingException(mpInfo, context)  {  }

  /// Destructor
  virtual ~NotFoundException() { }
};  // class NotFoundException

/**
 * Implements a an exception used when there is an invalid value retrieved from the dicom header
 */
public ref class InvalidValueException : public DicomImagingException
{
public:
  /// Constructor
  InvalidValueException(System::String ^ msMessage) : DicomImagingException(msMessage) { }

  /// Constructor
  InvalidValueException(System::String ^ msMessage, System::Exception ^ mpEx) : DicomImagingException(msMessage, mpEx) { }

  /// Constructor
  InvalidValueException(System::Runtime::Serialization::SerializationInfo ^ mpInfo, System::Runtime::Serialization::StreamingContext context) : DicomImagingException(mpInfo, context)  {  }

  /// Destructor
  virtual ~InvalidValueException() { }
};  // class InvalidValueException

CLOSE_DICOM_IMAGING_NS