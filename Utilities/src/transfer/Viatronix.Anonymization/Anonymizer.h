// $Id: Anonymizer.h,v 1.9.2.2 2007/07/03 18:35:03 mkontak Exp $ 
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

using namespace System;
using namespace System::Windows::Threading;
using namespace Collections::Generic;
 
// namespaces
OPEN_ANMZN_NS

// forward declarations
ref class AnonymizationInformation;
ref class AnonymizationEventArgs;


/**
 * Base anoymizer object
 */
public ref class Anonymizer abstract
{
// delegates
public:

  /// delegate declaration
  delegate void AnonymizationProgressEventHandler(Object ^ mpSender, AnonymizationEventArgs ^ mpArgs);

// events
public:

  /// Now, create a public event  whose type is our AnonymizationEventHandler delegate. 
  event AnonymizationProgressEventHandler ^ anonymizationProgressEvent;
  
// construction
public:

  /// Constructor
  Anonymizer(void);
  
  /// Constructor
  Anonymizer(AnonymizationInformation ^ mpnonymizationInfo);

  /// Abort method which sets the abort flag
  void Abort( bool bAbort );

  /// Sets the dispatcher to perform ui operations.
  void SetDispatcher(Dispatcher ^ mpDispatcher) { m_mpDispatcher = mpDispatcher; } 
  
  /// Destructor
  virtual ~Anonymizer(void);

  /// Interface method.
  virtual void Anonymize( List<String ^> ^ mpSourceFiles, List<String ^> ^ mpDestFiles ) = 0;
  
protected:

  ///Raise the Progress Event to the User object.
  virtual void RaiseProgressEvent(String ^ mpMessage, int iFileCount);

  ///Check for the Abort Status
  bool IsAbort();

  ///Replaces all occurences of the search strings with their corresponding replacement strings in the string supplied.
  String ^ ReplaceAll(String ^ mpOriginal);
  
  ///Replaces all occurences of the search strings with their corresponding replacement strings in the string supplied.
  String ^ ReplaceDirectoryNames(String ^ mpPath);

// fields
protected:

  ///Anonymization Infomration
  AnonymizationInformation ^ m_mpAnonymizationInfo;

  ///Flag determines Abort operation.
  bool m_bAbort;

  /// The dispatcher to perform UI operations.
  Dispatcher ^ m_mpDispatcher;
  
};  // class Anoymizer

CLOSE_ANMZN_NS

// $Log: Anonymizer.h,v $
// Revision 1.9.2.2  2007/07/03 18:35:03  mkontak
// Issue 5707: Added gcnew method ReplaceDirectoryNames() that will anonymize the individual directory names
// within the supplied path. Uses only search and replace object that are associated
// with our naming conventions.
//
// Revision 1.9.2.1  2007/03/27 00:49:12  romy
// added Frame object as a datamember
//
// Revision 1.9  2007/03/12 14:43:19  mkontak
// coding standards
//
// Revision 1.8  2005/11/15 19:54:06  mkontak
// Moved the GetVolumeType() and GetVolumeDataType() out of anonymization
// and into utilties.
//
// Revision 1.7  2005/10/20 15:15:41  romy
// added abort methid
//
// Revision 1.6  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/Anonymizer.h,v 1.9.2.2 2007/07/03 18:35:03 mkontak Exp $
// $Id: Anonymizer.h,v 1.9.2.2 2007/07/03 18:35:03 mkontak Exp $
