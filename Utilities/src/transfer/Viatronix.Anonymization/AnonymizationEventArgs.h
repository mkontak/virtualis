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

using namespace System;

// namespaces
OPEN_ANMZN_NS 

/* 
 ^ Class used to define the event args which helps the messaging of anonymization progress.
 */
 public ref class AnonymizationEventArgs: public System::EventArgs 
{

public:
  ///Constructor with args
  AnonymizationEventArgs( String ^ sMessage, int iProgress );

  ///default constructor
  AnonymizationEventArgs(void);

  ///GetMessage Property
  property String ^ Message
  { 
    String ^ get() { return m_sMessage; }
  }

  ///Get Progress property.
  property int Progress
  { 
    int get() { return m_iProgress; }
  }

private:

  ///Progress message
  String ^ m_sMessage;

  /// Progress count.
  int m_iProgress;

}; // class AnonymizationEventArgs

CLOSE_ANMZN_NS

// $Log: AnonymizationEventArgs.h,v $
// Revision 1.5  2007/03/12 15:24:37  mkontak
// coding standards
//
// Revision 1.4  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationEventArgs.h,v 1.5 2007/03/12 15:24:37 mkontak Exp $
// $Id: AnonymizationEventArgs.h,v 1.5 2007/03/12 15:24:37 mkontak Exp $
