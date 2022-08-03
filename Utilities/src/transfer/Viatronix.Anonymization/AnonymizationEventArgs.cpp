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
#include "anonymizationeventargs.h"
using namespace System;
#using <mscorlib.dll>

// namesaces
USING_ANMZN_NS

/*
 *Constructor
 */
AnonymizationEventArgs::AnonymizationEventArgs(void)
{
}//AnonymizationEventArgs(void)


/*
 ^ Constructor
 *
 ^ @param sMessage          event message
 ^ @param iProgress         progress
 */
AnonymizationEventArgs::AnonymizationEventArgs( String ^ sMessage, int iProgress )
{
      this->m_sMessage  = sMessage;
      this->m_iProgress = iProgress;

}//AnonymizationEventArgs( String ^ sMessage, int iProgress )



// $Log: AnonymizationEventArgs.cpp,v $
// Revision 1.5  2007/03/12 15:24:37  mkontak
// coding standards
//
// Revision 1.4  2005/09/22 15:33:24  romy
// Version LogInfo was missing.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Anonymization/AnonymizationEventArgs.cpp,v 1.5 2007/03/12 15:24:37 mkontak Exp $
// $Id: AnonymizationEventArgs.cpp,v 1.5 2007/03/12 15:24:37 mkontak Exp $