// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// includes
#include "StdAfx.h"
#include "Exceptions.h"

// namespaces
USING_ANMZN_NS
using namespace System;


/**
 ^ constructor
 */
InvalidAnonymizorTypeException::InvalidAnonymizorTypeException()
{
}//InvalidAnonymizorTypeException()


/*
 ^ Constructor
 ^ @param mpMessage   error message 
 */
InvalidAnonymizorTypeException::InvalidAnonymizorTypeException(String ^ mpMessage) : ApplicationException( mpMessage )
{
}//InvalidAnonymizorTypeException( mpMessage ) 


/*
 ^ Constructor
 ^ @param mpMessage           error message 
 ^ @param mpInnerException    inner exception
 */
InvalidAnonymizorTypeException::InvalidAnonymizorTypeException(String ^ mpMessage, Exception ^ mpInnerException) : ApplicationException(mpMessage, mpInnerException)
{
}//InvalidAnonymizorTypeException::InvalidAnonymizorTypeException( mpMessage, mpInnerException )


/* 
 ^ Constructor
 */
AnonymizationException::AnonymizationException()
{
} // AnonymizationException()


/*
 ^ Constructor
 ^ @param mpMessage error message
 */
AnonymizationException::AnonymizationException(String ^ mpMessage) : ApplicationException(mpMessage)
{
}//AnonymizationException( mpMessage )


/*
 ^ Constructor
 ^ @param mpMessage         error message
 ^ @param mpInnerException  inner exception
 */
AnonymizationException::AnonymizationException(String ^ mpMessage, Exception ^ mpInnerException) : ApplicationException(mpMessage, mpInnerException)
{
} // AnonymizationException( mpMessage, mpInnerException )


/* 
 ^ Constructor
 */
InvalidSeriesException::InvalidSeriesException()
{
}//InvalidSeriesException


/*
 ^ Constructor
 ^ @param mpMessage   error message
 */
InvalidSeriesException::InvalidSeriesException(String ^ mpMessage) : ApplicationException(mpMessage)
{
}//InvalidSeriesException( mpMessage )


/*
 ^ Constructor
 ^ @param mpMessage         error message
 ^ @param mpInnerException  inner exception 
 */
InvalidSeriesException::InvalidSeriesException(String ^ mpMessage, Exception ^ mpInnerException) : ApplicationException(mpMessage, mpInnerException)
{
} // InvalidSeriesException( mpMessage, mpInnerException )


/*
 ^ Constructor
 */
DicomFileNotFoundException::DicomFileNotFoundException()
{
} // DicomFileNotFoundException()


/*
 ^ Constructor
 ^ @param mpMessage   error message
 */
DicomFileNotFoundException::DicomFileNotFoundException(String ^ mpMessage) : ApplicationException(mpMessage)
{
} // DicomFileNotFoundException( mpMessage )


/*
 ^ Constructor
 ^ @param mpMessage         error message
 ^ @param mpInnerException  inner exception
 */
DicomFileNotFoundException::DicomFileNotFoundException(String ^ mpMessage, Exception ^ mpInnerException) : ApplicationException( mpMessage, mpInnerException )
{
} // DicomFileNotFoundException( mpMessage, mpInnerException )


/*
 ^ Constructor
 */
Viatronix::Anonymization::AbortException::AbortException() : ApplicationException()
{
}//AbortException()


/*
 ^ Constructor
 ^ @param mpMessage error message 
 */
Viatronix::Anonymization::AbortException::AbortException(String ^ mpMessage) : ApplicationException(mpMessage)
{
}//AbortException( String ^ mpMessage )


/*
 ^ Constructor
 ^ @param mpMessage         error message
 ^ @param mpInnerException  inner exception
 */
Viatronix::Anonymization::AbortException::AbortException(String ^ mpMessage, Exception ^ mpInnerException) : ApplicationException(mpMessage, mpInnerException)
{
} // AbortException( mpMessage, mpInnerException )


// $Log: Exceptions.cpp,v $
// Revision 1.7  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.6  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.5  2005/09/26 15:54:53  romy
// added abort mechanism
//
// Revision 1.4  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/Exceptions.cpp,v 1.7 2007/03/12 14:43:20 mkontak Exp $
// $Id: Exceptions.cpp,v 1.7 2007/03/12 14:43:20 mkontak Exp $