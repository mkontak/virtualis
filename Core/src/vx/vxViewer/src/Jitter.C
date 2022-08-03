// $Id: Jitter.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright© 2000,2001 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "Jitter.h"

/**
 * Constructor.
 */
Jitter::Jitter()
{
  // fill the buffer with random values between 0 and 1/2
  for (uint4 i(0); i<m_eNumBuffers*m_eSize*m_eSize; i++)
  {
    m_vfAllBuffers[i] = float4(Drand48(0.0F, 0.5F));
  } // end for i
  
  // make the current buffer point to the start of the full buffer
  m_pfCurrentBuffer = m_vfAllBuffers;

} // Jitter()


// $Log: Jitter.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.1  2002/07/17 15:30:52  ingmar
// moved Jitter class from v3DColon/Readlib to 75_renderUtils
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:00   ingmar
// Initial revision.
// Revision 1.6  2001/06/26 16:21:28  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.5  2001/04/10 13:13:39  frank
// Code walkthrough results.
//
// Revision 1.4  2001/03/19 12:18:43  frank
// Updated to reflect new coding standards.
//
// Revision 1.3  2001/02/12 17:14:51  jeff
// throw viatronix exceptions for V1K errors instead of aborting
//
// Revision 1.2  2000/09/19 12:52:24  frank
// Added memory allocation check
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 5     8/31/00 9:06a Frank
// Updated comments
// 
// 4     8/28/00 11:28a Antonio
// updated header
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 3
//Comment:
//  
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/18/00  Time:  1:35p
//Checked in $/v1k/src/ReaderLib
//Comment:
//  class Global --> class ReaderGlobal
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created Jitter.cpp
//Comment:  
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/Jitter.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: Jitter.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
