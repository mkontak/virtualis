// $Id: AnonymizerImage.cpp,v 1.9.2.1 2007/03/27 00:49:27 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: [Romy Sreedharan] [ romy@viatronix.com]

// includes
#include "StdAfx.h"
#include "SearchAndReplace.h"
#include "AnonymizationInformation.h"
#include "anonymizerimage.h"
#include "Exceptions.h"

#using <mscorlib.dll>

// namespaces
USING_ANMZN_NS
using namespace System;
using namespace System::Collections;

using namespace Viatronix::Logging;
using namespace Viatronix::Console::ImageRedactor;


/*
 ^ Constructor
 */
AnonymizerImage::AnonymizerImage() : m_mpResetEvent(gcnew System::Threading::AutoResetEvent(false))
{
}//AnonymizerImage(void)

/*
 *Destructor
 */
AnonymizerImage::!AnonymizerImage()
{
}//~AnonymizerImage(void)

/*
 ^ Construtor
 *
 ^ @param anonymizationInfo     Anonymization infromation
 */
AnonymizerImage::AnonymizerImage(AnonymizationInformation ^ anonymizationInfo) : Anonymizer(anonymizationInfo), m_mpResetEvent(gcnew System::Threading::AutoResetEvent(false))
{
} // AnonymizerImage( anonymizationInfo )

/*
 ^ Performs the anonymization
 *
 ^ @param sSourceFiles      List of source files
 ^ @param sDestFiles        List of destination files
 */
void AnonymizerImage::Anonymize(List<String ^> ^ sSourceFiles, List<String ^> ^ sDestFiles)
{
  RaiseProgressEvent( "Anonymizing Snapshot" , 0 );

  m_sSourceFiles = gcnew List<String ^>();
  m_sSourceFiles = sSourceFiles;

  m_sDestFiles = gcnew List<String ^>();
  m_sDestFiles = sDestFiles;

  m_mpResetEvent->Reset();

  // Show the redactor.
  m_mpDispatcher->BeginInvoke(gcnew ShowRedactorInterfaceDelegate(this, &AnonymizerImage::ShowRedactorInterface));
  
  m_mpResetEvent->WaitOne();
} // Anonymize( sSourceFiles, sDestFiles )

    
/**
 * Shows the Redactor editor GUI
 */
void AnonymizerImage::ShowRedactorInterface()
{
  RedactorForm ^ redactor = gcnew RedactorForm();
  redactor->RedactImages( m_sSourceFiles , m_sDestFiles );
  redactor->ShowDialog();
  RaiseProgressEvent("Finished Snapshots Anonymization" , m_sSourceFiles->Count);

  // User is done, set the event.
  m_mpResetEvent->Set();
} // ShowRedactorInterface()


// $Log: AnonymizerImage.cpp,v $
// Revision 1.9.2.1  2007/03/27 00:49:27  romy
// displaying of redactor modified.
//
// Revision 1.9  2007/03/12 14:43:20  mkontak
// coding standards
//
// Revision 1.8  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.7  2006/12/05 21:00:01  romy
// Console Dlls Consolidation
//
// Revision 1.6  2005/10/20 15:16:12  romy
// added progress events and Exceptions
//
// Revision 1.5  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizerImage.cpp,v 1.9.2.1 2007/03/27 00:49:27 romy Exp $
// $Id: AnonymizerImage.cpp,v 1.9.2.1 2007/03/27 00:49:27 romy Exp $