// pragmas
#pragma once

// namespaces
using namespace System;

OPEN_PDF_NS

/**
 * Represents a generic exception within this library.
 */
public ref class PdfException : Exception
{
// construction
public:

  /// Constructor.
  PdfException();

  /// Constructor.
  PdfException(String ^ mpMessage);

  /// Constructor.
  PdfException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class PdfException


/**
 * Indicates that the key provided to the PDF library is invalid.
 */
public ref class InvalidKeyException : PdfException
{
// construction
public:

  /// Constructor.
  InvalidKeyException();

  /// Constructor.
  InvalidKeyException(String ^ mpMessage);

  /// Constructor.
  InvalidKeyException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class InvalidKeyException


/**
 * Represents an exception that identifies an unsupported race condition.
 */
public ref class UnsupportedException : PdfException
{
// construction
public:

  /// Constructor.
  UnsupportedException();

  /// Constructor.
  UnsupportedException(String ^ mpMessage);

  /// Constructor.
  UnsupportedException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class PdfUnsupportedException


/**
 * Represents an exception that identifies an abort has occurred.
 */
public ref class AbortException : PdfException
{
// construction
public:

  /// Constructor.
  AbortException();

  /// Constructor.
  AbortException(String ^ mpMessage);

  /// Constructor.
  AbortException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class AbortException


/**
 * Represents an exception that identifies a issue with configuration.
 */
public ref class ConfigurationException : PdfException
{
// construction
public:

  /// Constructor.
  ConfigurationException();

  /// Constructor.
  ConfigurationException(String ^ mpMessage);

  /// Constructor.
  ConfigurationException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class ConfigurationException


/**
 * Represents an exception that identifies there was an error reading or writing out to the disk.
 */
public ref class IOException : PdfException
{
// construction
public:

  /// Constructor.
  IOException();

  /// Constructor.
  IOException(String ^ mpMessage);

  /// Constructor.
  IOException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class IOException


/**
 * Represents an exception that identifies there was invalid data provided.
 */
public ref class InvalidDataException : PdfException
{
// construction
public:

  /// Constructor.
  InvalidDataException();

  /// Constructor.
  InvalidDataException(String ^ mpMessage);

  /// Constructor.
  InvalidDataException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class InvalidDataException


/**
 * Represents an exception that identifies the specified function was not yet implemented.
 */
public ref class NotImplementedException : PdfException
{
// construction
public:

  /// Constructor.
  NotImplementedException();

  /// Constructor.
  NotImplementedException(String ^ mpMessage);

  /// Constructor.
  NotImplementedException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class NotImplementedException


/**
 * Represents an exception that identifies we're out of memory.
 */
public ref class OutOfMemoryException : PdfException
{
// construction
public:

  /// Constructor.
  OutOfMemoryException();

  /// Constructor.
  OutOfMemoryException(String ^ mpMessage);

  /// Constructor.
  OutOfMemoryException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class OutOfMemoryException


/**
 * Represents an exception that indicates the specified coordinates are out of bounds.
 */
public ref class OutOfBoundsException : PdfException
{
// construction
public:

  /// Constructor.
  OutOfBoundsException();

  /// Constructor.
  OutOfBoundsException(String ^ mpMessage);

  /// Constructor.
  OutOfBoundsException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class OutOfBoundsException


/**
 * Represents an exception that indicates that the target was not found.
 */
public ref class NotFoundException : PdfException
{
// construction
public:

  /// Constructor.
  NotFoundException();

  /// Constructor.
  NotFoundException(String ^ mpMessage);

  /// Constructor.
  NotFoundException(String ^ mpMessage, Exception ^ mpInnerException);

}; // class NotFoundException

CLOSE_PDF_NS
