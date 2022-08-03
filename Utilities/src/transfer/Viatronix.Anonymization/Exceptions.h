// $Id: 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

// pragmas
#pragma once

OPEN_ANMZN_NS 

/**
 ^ Anonymizer Invalid Type Exception handle Anonimization errors
 */
public ref class InvalidAnonymizorTypeException : public System::ApplicationException
{

public:
  
  ///Constructor
  InvalidAnonymizorTypeException();

  ///constructor
  InvalidAnonymizorTypeException( System::String ^ mpMessage );

  ///Constructor 
  InvalidAnonymizorTypeException( System::String ^ mpMessage, System::Exception ^ mpInnerException );

}; // class InvalidAnonymizorTypeException


/**
 ^ General Anonymization Exception
 */
public ref class AnonymizationException : public System::ApplicationException
{

public:

  ///Constructor
  AnonymizationException();

  ///constructor
  AnonymizationException( System::String ^ mpMessage );

  ///Constructor
  AnonymizationException( System::String ^ mpMessage, System::Exception ^ mpInnerException );

}; // class AnonymizationException


/** 
 ^ InvalidSeriesException
 */
public ref class InvalidSeriesException : public System::ApplicationException
{

public:

  ///Constructor
  InvalidSeriesException();

  ///Constructor
  InvalidSeriesException( System::String ^ mpMessage );

  ///Constructor
  InvalidSeriesException( System::String ^ mpMessage, System::Exception ^ mpInnerException );

}; // class InvalidSeriesException


/**
 ^ DicomFileNotFoundException
 */
public ref class DicomFileNotFoundException : public System::ApplicationException
{

public:

  ///Constructor
  DicomFileNotFoundException();

  ///Constructor
  DicomFileNotFoundException( System::String ^ mpMessage );

  ///Constructor
  DicomFileNotFoundException( System::String ^ mpMessage, System::Exception ^ mpInnerException );

}; // class DicomFileNotFoundException


/**
 ^ AbortException
 */
public ref class AbortException : public System::ApplicationException
{

public:

  ///Constructor
  AbortException();

  ///Constructor
  AbortException( System::String ^ mpMessage );

  ///Constructor
  AbortException( System::String ^ mpMessage, System::Exception ^ mpInnerException );

}; // class AbortException


CLOSE_ANMZN_NS 

// $Log: Exceptions.h,v $
// Revision 1.7  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.6  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.5  2005/09/26 15:54:53  romy
// added abort mechanism
//
// Revision 1.4  2005/09/22 15:32:07  romy
// Version LogInfo was missing.
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/Exceptions.h,v 1.7 2007/03/12 14:43:20 mkontak Exp $
// $Id: Exceptions.h,v 1.7 2007/03/12 14:43:20 mkontak Exp $