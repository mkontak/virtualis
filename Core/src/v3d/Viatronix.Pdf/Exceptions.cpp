// includes
#include "Stdafx.h"
#include "Exceptions.h"

// namespaces
USING_PDF_NS

/**
 * Constructor.
 */
Viatronix::Pdf::PdfException::PdfException() : System::Exception()
{
} // PdfException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::PdfException::PdfException(System::String ^ mpMessage) : System::Exception(mpMessage)
{
} // PdfException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::PdfException::PdfException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : System::Exception(mpMessage, mpInnerException)
{
} // PdfException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::InvalidKeyException::InvalidKeyException() : Viatronix::Pdf::PdfException()
{
} // InvalidKeyException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::InvalidKeyException::InvalidKeyException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // InvalidKeyException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::InvalidKeyException::InvalidKeyException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // InvalidKeyException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::UnsupportedException::UnsupportedException() : Viatronix::Pdf::PdfException()
{
} // UnsupportedException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::UnsupportedException::UnsupportedException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // UnsupportedException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::UnsupportedException::UnsupportedException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // UnsupportedException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::AbortException::AbortException() : Viatronix::Pdf::PdfException()
{
} // AbortException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::AbortException::AbortException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // AbortException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::AbortException::AbortException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // AbortException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::ConfigurationException::ConfigurationException() : Viatronix::Pdf::PdfException()
{
} // ConfigurationException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::ConfigurationException::ConfigurationException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // ConfigurationException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::ConfigurationException::ConfigurationException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // ConfigurationException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::IOException::IOException() : Viatronix::Pdf::PdfException()
{
} // IOException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::IOException::IOException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // IOException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::IOException::IOException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // IOException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::InvalidDataException::InvalidDataException() : Viatronix::Pdf::PdfException()
{
} // InvalidDataException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::InvalidDataException::InvalidDataException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // InvalidDataException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::InvalidDataException::InvalidDataException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // InvalidDataException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::NotImplementedException::NotImplementedException() : Viatronix::Pdf::PdfException()
{
} // NotImplementedException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::NotImplementedException::NotImplementedException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // NotImplementedException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::NotImplementedException::NotImplementedException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // NotImplementedException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::OutOfMemoryException::OutOfMemoryException() : Viatronix::Pdf::PdfException()
{
} // OutOfMemoryException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::OutOfMemoryException::OutOfMemoryException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // OutOfMemoryException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::OutOfMemoryException::OutOfMemoryException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // OutOfMemoryException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::OutOfBoundsException::OutOfBoundsException() : Viatronix::Pdf::PdfException()
{
} // OutOfBoundsException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::OutOfBoundsException::OutOfBoundsException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // OutOfBoundsException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::OutOfBoundsException::OutOfBoundsException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // OutOfBoundsException( mpMessage, mpInnerException )


/**
 * Constructor.
 */
Viatronix::Pdf::NotFoundException::NotFoundException() : Viatronix::Pdf::PdfException()
{
} // NotFoundException()


/**
 * Constructor.
 *
 * @param mpMessage  The message to indicate the fault.
 */
Viatronix::Pdf::NotFoundException::NotFoundException(System::String ^ mpMessage) : Viatronix::Pdf::PdfException(mpMessage)
{
} // NotFoundException( mpMessage )


/**
 * Constructor.
 *
 * @param mpMessage         The message to indicate the fault.
 * @param mpInnerException  The inner System::Exception.
 */
Viatronix::Pdf::NotFoundException::NotFoundException(System::String ^ mpMessage, System::Exception ^ mpInnerException) : Viatronix::Pdf::PdfException(mpMessage, mpInnerException)
{
} // NotFoundException( mpMessage, mpInnerException )
