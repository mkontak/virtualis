// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <string>

#include "Logger.h"
#include "vxBaseLibrary.h"
#include "vxBlockVolumeLibrary.h"
#include "vxDicomLibrary.h"
#include "vxConversionLibrary.h"


#pragma warning(disable : 4251)
#pragma warning(disable : 4482)

#define USING_CONVERSION_NS  using namespace Viatronix::Conversion;
#define OPEN_CONVERSION_NS   namespace Viatronix { namespace Conversion {
#define CLOSE_CONVERSION_NS  } } 


/// Callback used for the dicom services
public delegate void CallbackDelegate(cvn::PCVN_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData);


/*
 * Converts a mananged System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string represenetatioon of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ss()

/** 
 * Converts the string to a char *
 */
inline char * ToCharArray( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  int length = string->Length + 1;

  char * returnBuffer = new char [length];

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  memset( returnBuffer, 0, length );
  memcpy( returnBuffer, ( void * )buffer.ToPointer(), string->Length);
  
	
  Marshal::FreeHGlobal( buffer );

  return returnBuffer;
} // ToCharArray()

// Namespaces
OPEN_CONVERSION_NS


#pragma region Exceptions


/**
 * Impelements an Abort Exception
 */
public ref class AbortException : public System::Exception 
{
public:
  /// Constructor
  AbortException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  AbortException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Destructor
  virtual ~AbortException() { this->!AbortException(); }

  /// Finalizer
  !AbortException() { }

};  // class AbortException

/**
 * Implements a base ConversionException exception
 ^ All other exceptions should inheret from this.
 */
public ref class ConversionException : public System::Exception
{
public:
  /// Constructor
  ConversionException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  ConversionException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Constructor
  ConversionException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : System::Exception(pInfo, context)  {  }

  /// Destructor
  virtual ~ConversionException() { this->!ConversionException(); }

  /// Finalizer
  !ConversionException() {}

};  // class ConversionException

/**
 * Implements a duplicate element exception
 */
public ref class AlreadyStartedException : public ConversionException
{
public:
  /// Constructor
  AlreadyStartedException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  AlreadyStartedException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

	/// Constructor
  AlreadyStartedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~AlreadyStartedException() { this->!AlreadyStartedException(); }

  /// Finalizer
  !AlreadyStartedException() { }

};  // class AlreadyStartedException

/**
 * Implements a duplicate element exception
 */
public ref class DuplicateElementException : public ConversionException
{
public:
  /// Constructor
  DuplicateElementException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  DuplicateElementException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

	/// Constructor
  DuplicateElementException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~DuplicateElementException() { this->!DuplicateElementException(); }

  /// Finalizer
  !DuplicateElementException() { }

};  // class DuplicateElementException

/**
 * Implements a connection lost exception
 */
public ref class AssociationNotAcceptedException : public ConversionException
{
public:
  /// Constructor
  AssociationNotAcceptedException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  AssociationNotAcceptedException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  AssociationNotAcceptedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~AssociationNotAcceptedException() { this->!AssociationNotAcceptedException(); }

  /// Finalizer
  !AssociationNotAcceptedException() { }

};  // class AssociationNotAcceptedException


/**
 * Implements an Dicom shutdown Exception
 */
public ref class ShutdownException : public ConversionException
{
public:
  /// Constructor
  ShutdownException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  ShutdownException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  ShutdownException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~ShutdownException() { this->!ShutdownException(); }

  /// Finalizer
  !ShutdownException() {}

};  // class ShutdownException


/**
 * Implements an UnsupportedException
 */
public ref class UnsupportedException : public ConversionException
{
public:
  UnsupportedException(System::String ^ sMessage) : ConversionException(sMessage) { }
  UnsupportedException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  UnsupportedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~UnsupportedException() { this->!UnsupportedException(); }

  /// Finalizer
  !UnsupportedException() { }

};  // class UnsupportedException

/**
* Implements a generic image exception
*/
public ref class ImageException : public ConversionException
{
public:
  /// Constructor
  ImageException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  ImageException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  ImageException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~ImageException() { this->!ImageException(); }

   /// Destructor
  !ImageException() { }

};  // class NoImageException

/**
 * Implements a no image exception
 */
public ref class NoImageException : public ImageException
{
public:
  /// Constructor
  NoImageException(System::String ^ sMessage) : ImageException(sMessage) { }

  /// Constructor
  NoImageException(System::String ^ sMessage, System::Exception ^ ex) : ImageException(sMessage, ex) { }

  /// Constructor
  NoImageException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ImageException(pInfo, context)  {  }

  /// Destructor
  virtual ~NoImageException() { this->!NoImageException(); }

  /// Destructor
  !NoImageException() { }

};  // class NoImageException

/**
 * Implements an invalid data exception
 */
public ref class InvalidDataException : public ConversionException
{
public:
  /// Constructor
  InvalidDataException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  InvalidDataException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  InvalidDataException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~InvalidDataException() { this->!InvalidDataException(); }

  /// Finalizer
  !InvalidDataException() { }

};  // class InvalidDataException

/**
 * Implements a general Not Found Exception
 */
public ref class NotFoundException : public ConversionException
{
public:
  /// Constructor
  NotFoundException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  NotFoundException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  NotFoundException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  // Destrcutor
  virtual ~NotFoundException() { this->!NotFoundException(); }

  // Destrcutor
  !NotFoundException() { }

};  // class NotFoundException


/**
 * Implements an already exists exception
 */
public ref class AlreadyExistsException : public ConversionException
{
public:
  /// Constructor
  AlreadyExistsException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  AlreadyExistsException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  AlreadyExistsException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~AlreadyExistsException() { this->!AlreadyExistsException(); }

  /// Finalizer
  !AlreadyExistsException() { }

}; // class AlreadyExistsException

/**
 * Implements Invalid IP Address Exception
 */
public ref class InvalidIPAddressException : public InvalidDataException
{
public:
  /// Constructor
  InvalidIPAddressException(System::String ^ sMessage) : InvalidDataException(sMessage) { }

  /// Constructor
  InvalidIPAddressException(System::String ^ sMessage, System::Exception ^ ex) : InvalidDataException(sMessage, ex) { }

	/// Constructor
  InvalidIPAddressException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : InvalidDataException(pInfo, context)  {  }

  /// Destructor
  virtual ~InvalidIPAddressException() { this->!InvalidIPAddressException(); }

  /// Finalizer
  !InvalidIPAddressException() { }


}; // class InvalidIPAddressException

/**
 * Implements a connection excedption
 */
public ref class ConnectionException : public ConversionException
{
public:
  /// Constructor
  ConnectionException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  ConnectionException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  ConnectionException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~ConnectionException() { this->!ConnectionException(); }

  /// Finalizer
  !ConnectionException() { }

};  // class ConnectionException


/**
 * Implements a generic Dicom PACS exception
 */
public ref class PacsException : public ConversionException
{
public:
  /// Constructor
  PacsException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  PacsException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  PacsException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~PacsException() { this->!PacsException(); }

  /// Destructor
  !PacsException() { }

};  // class PacsException

/**
 * Implements a no value exception Exception
 */
public ref class NoElementValueException : public ConversionException
{
public:
  /// Constructor
  NoElementValueException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  NoElementValueException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  NoElementValueException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~NoElementValueException() { this->!NoElementValueException(); }

   /// Finalizer
  !NoElementValueException() { }

}; // class NoElementValueException()

/**
 * Implements a already being received Exception
 */
public ref class AlreadyBeingReceivedException : public ConversionException
{
public:
  /// Constructor
  AlreadyBeingReceivedException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  AlreadyBeingReceivedException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  AlreadyBeingReceivedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~AlreadyBeingReceivedException() { this->!AlreadyBeingReceivedException(); }

  /// FinalizerDestructor
  !AlreadyBeingReceivedException() { }

}; // class AlreadyBeingReceivedException()

/**
 * Implements an Operation Exception
 */
public ref class  OperationException : public ConversionException
{
public:
  /// Constructor
  OperationException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  OperationException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

	/// Constructor
  OperationException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~OperationException() { this->!OperationException(); }

  /// Destructor
  !OperationException() { }

}; // class OperationException()


/**
 * Implements an Invalid Version Exception
 */
public ref class  UnsupportedVersionException : public ConversionException
{
public:
  /// Constructor
  UnsupportedVersionException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  UnsupportedVersionException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

	/// Constructor
  UnsupportedVersionException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  /// Destructor
  virtual ~UnsupportedVersionException() { this->!UnsupportedVersionException(); }

  /// Destructor
  !UnsupportedVersionException() { }

}; // class UnsupportedVersionException()


/**
 * Implements a general element not found Exception
 */
public ref class ElementNotFoundException : public NotFoundException
{
public:
  /// Constructor
  ElementNotFoundException(System::String ^ sMessage) : NotFoundException(sMessage) { }

  /// Constructor
  ElementNotFoundException(System::String ^ sMessage, System::Exception ^ ex) : NotFoundException(sMessage, ex) { }

  /// Constructor
  ElementNotFoundException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : NotFoundException(pInfo, context)  {  }

  // Destrcutor
  virtual ~ElementNotFoundException() { this->!ElementNotFoundException(); }

  // Destrcutor
  !ElementNotFoundException() { }

};  // class NotFoundException


/**
 * Implements a general element not found Exception
 */
public ref class IOException : public ConversionException
{
public:
  /// Constructor
  IOException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  IOException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  IOException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  // Destrcutor
  virtual ~IOException() { this->!IOException(); }

  // Destrcutor
  !IOException() { }

};  // class IOException


/**
 * Implements a configurtaion exception
 */
public ref class ConfigurationException : public ConversionException
{
public:
  /// Constructor
  ConfigurationException(System::String ^ sMessage) : ConversionException(sMessage) { }

  /// Constructor
  ConfigurationException(System::String ^ sMessage, System::Exception ^ ex) : ConversionException(sMessage, ex) { }

  /// Constructor
  ConfigurationException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : ConversionException(pInfo, context)  {  }

  // Destrcutor
  virtual ~ConfigurationException() { this->!ConfigurationException(); }

  // Destrcutor
  !ConfigurationException() { }

};  // class ConfigurationException


#pragma endregion


CLOSE_CONVERSION_NS