// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <WinSock2.h>
#include <Windows.h> 

#include "Logger.h"
#include "vxDicomLibrary.h"

#include <string>


#pragma warning(disable : 4251)
#pragma warning(disable : 4482)



#define USING_DICOM_NS  using namespace Viatronix::Dicom;
#define OPEN_DICOM_NS   namespace Viatronix { namespace Dicom {
#define CLOSE_DICOM_NS  } } 


/// Callback used for the dicom services
public delegate void DicomCallbackDelegate(dcm::PDCM_CALLBACK_PARAMETERS pParameters, LPVOID lpCallbackData);


/*
 * Converts a managed System::String to a std::string 
 *
 * @param string      String to convert to an unmanaged std::string
 *
 * @returns Returns the std::string representation of the String supplied.
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
} // ss()


// Namespaces
OPEN_DICOM_NS


#pragma region Exceptions


/**
 * Implements an Abort Exception
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
 * Implements a base Dicom exception
 * All other exceptions should inherit from this.
 */
public ref class DicomException : public System::Exception
{
public:
  /// Constructor
  DicomException(System::String ^ sMessage) : System::Exception(sMessage) { }

  /// Constructor
  DicomException(System::String ^ sMessage, System::Exception ^ ex) : System::Exception(sMessage, ex) { }

  /// Constructor
  DicomException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : System::Exception(pInfo, context)  {  }

  /// Destructor
  virtual ~DicomException() { this->!DicomException(); }

  /// Finalizer
  !DicomException() {}

};  // class DicomException

/**
 * Implements a duplicate element exception
 */
public ref class AlreadyStartedException : public DicomException
{
public:
  /// Constructor
  AlreadyStartedException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  AlreadyStartedException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

    /// Constructor
  AlreadyStartedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~AlreadyStartedException() { this->!AlreadyStartedException(); }

  /// Finalizer
  !AlreadyStartedException() { }

};  // class AlreadyStartedException

/**
 * Implements a duplicate element exception
 */
public ref class DuplicateElementException : public DicomException
{
public:
  /// Constructor
  DuplicateElementException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  DuplicateElementException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

    /// Constructor
  DuplicateElementException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~DuplicateElementException() { this->!DuplicateElementException(); }

  /// Finalizer
  !DuplicateElementException() { }

};  // class DuplicateElementException

/**
 * Implements a connection lost exception
 */
public ref class AssociationNotAcceptedException : public DicomException
{
public:
  /// Constructor
  AssociationNotAcceptedException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  AssociationNotAcceptedException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  AssociationNotAcceptedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~AssociationNotAcceptedException() { this->!AssociationNotAcceptedException(); }

  /// Finalizer
  !AssociationNotAcceptedException() { }

};  // class AssociationNotAcceptedException


/**
 * Implements an Dicom shutdown Exception
 */
public ref class ShutdownException : public DicomException
{
public:
  /// Constructor
  ShutdownException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  ShutdownException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  ShutdownException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~ShutdownException() { this->!ShutdownException(); }

  /// Finalizer
  !ShutdownException() {}

};  // class ShutdownException


/**
 * Implements an UnsupportedException
 */
public ref class UnsupportedException : public DicomException
{
public:
  UnsupportedException(System::String ^ sMessage) : DicomException(sMessage) { }
  UnsupportedException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  UnsupportedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~UnsupportedException() { this->!UnsupportedException(); }

  /// Finalizer
  !UnsupportedException() { }

};  // class UnsupportedException

/**
* Implements a generic image exception
*/
public ref class ImageException : public DicomException
{
public:
  /// Constructor
  ImageException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  ImageException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  ImageException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

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
public ref class InvalidDataException : public DicomException
{
public:
  /// Constructor
  InvalidDataException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  InvalidDataException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  InvalidDataException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~InvalidDataException() { this->!InvalidDataException(); }

  /// Finalizer
  !InvalidDataException() { }

};  // class InvalidDataException

/**
 * Implements a general Not Found Exception
 */
public ref class NotFoundException : public DicomException
{
public:
  /// Constructor
  NotFoundException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  NotFoundException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  NotFoundException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  // Destrcutor
  virtual ~NotFoundException() { this->!NotFoundException(); }

  // Destrcutor
  !NotFoundException() { }

};  // class NotFoundException


/**
 * Implements an already exists exception
 */
public ref class AlreadyExistsException : public DicomException
{
public:
  /// Constructor
  AlreadyExistsException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  AlreadyExistsException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  AlreadyExistsException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

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
public ref class ConnectionException : public DicomException
{
public:
  /// Constructor
  ConnectionException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  ConnectionException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  ConnectionException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~ConnectionException() { this->!ConnectionException(); }

  /// Finalizer
  !ConnectionException() { }

};  // class ConnectionException


/**
 * Implements a generic Dicom PACS exception
 */
public ref class PacsException : public DicomException
{
public:
  /// Constructor
  PacsException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  PacsException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  PacsException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~PacsException() { this->!PacsException(); }

  /// Destructor
  !PacsException() { }

};  // class PacsException

/**
 * Implements a no value exception Exception
 */
public ref class NoElementValueException : public DicomException
{
public:
  /// Constructor
  NoElementValueException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  NoElementValueException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  NoElementValueException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~NoElementValueException() { this->!NoElementValueException(); }

   /// Finalizer
  !NoElementValueException() { }

}; // class NoElementValueException()

/**
 * Implements a already being received Exception
 */
public ref class AlreadyBeingReceivedException : public DicomException
{
public:
  /// Constructor
  AlreadyBeingReceivedException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  AlreadyBeingReceivedException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  AlreadyBeingReceivedException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~AlreadyBeingReceivedException() { this->!AlreadyBeingReceivedException(); }

  /// FinalizerDestructor
  !AlreadyBeingReceivedException() { }

}; // class AlreadyBeingReceivedException()

/**
 * Implements an Operation Exception
 */
public ref class  OperationException : public DicomException
{
public:
  /// Constructor
  OperationException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  OperationException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

    /// Constructor
  OperationException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  /// Destructor
  virtual ~OperationException() { this->!OperationException(); }

  /// Destructor
  !OperationException() { }

}; // class OperationException()


/**
 * Implements an Invalid Version Exception
 */
public ref class  UnsupportedVersionException : public DicomException
{
public:
  /// Constructor
  UnsupportedVersionException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  UnsupportedVersionException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

    /// Constructor
  UnsupportedVersionException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

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
public ref class IOException : public DicomException
{
public:
  /// Constructor
  IOException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  IOException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  IOException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  // Destrcutor
  virtual ~IOException() { this->!IOException(); }

  // Destrcutor
  !IOException() { }

};  // class IOException


/**
 * Implements a configurtaion exception
 */
public ref class ConfigurationException : public DicomException
{
public:
  /// Constructor
  ConfigurationException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  ConfigurationException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  ConfigurationException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  // Destrcutor
  virtual ~ConfigurationException() { this->!ConfigurationException(); }

  // Destrcutor
  !ConfigurationException() { }

};  // class ConfigurationException


/**
 * Implements a out of memory exception
 */
public ref class OutOfMemoryException : public DicomException
{
public:
  /// Constructor
  OutOfMemoryException(System::String ^ sMessage) : DicomException(sMessage) { }

  /// Constructor
  OutOfMemoryException(System::String ^ sMessage, System::Exception ^ ex) : DicomException(sMessage, ex) { }

  /// Constructor
  OutOfMemoryException(System::Runtime::Serialization::SerializationInfo ^ pInfo, System::Runtime::Serialization::StreamingContext context) : DicomException(pInfo, context)  {  }

  // Destrcutor
  virtual ~OutOfMemoryException() { this->!OutOfMemoryException(); }

  // Destrcutor
  !OutOfMemoryException() { }

};  // class OutOfMemoryException


#pragma endregion


CLOSE_DICOM_NS