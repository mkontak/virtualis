// $Id: vxModifiedSet.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxModifiedSet.h"
#include "vxModifiedInfo.h"
#include "vxObserver.h"


// defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 * @param client observer
 * @param modified info
 */
vxModifiedSet::vxModifiedSet(vxObserver * pClient, 
                             const vxModifiedInfo & info)
//*******************************************************************
{
  // verify that the client is not null
  if (pClient == NULL)
  {
    throw ex::NullPointerException(LogRec("The client pointer is null.","vxModifiedSet","vxModifiedSet"));
  }
  m_pClient = pClient;
  
  m_messages.PushBack(info);
} // vxModifiedSet()


/**
 * Appends the collection of messages
 * @param odified information
 */
void vxModifiedSet::AddMessage(const vxModifiedInfo & info)
{
  m_messages.PushBack(info);
} // AddMessage()


#undef FILE_REVISION


// Revision History:
// $Log: vxModifiedSet.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/12/16 13:25:07  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.3  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.1  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.6  2002/07/08 15:25:27  dmitry
// Changed to work with vxModifiedInfoList.
//
// Revision 1.5  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.4  2002/05/08 18:23:26  dmitry
// NoOpp call removed.
//
// Revision 1.3  2002/05/07 18:22:43  frank
// Moved includes from .inl file to .h file.
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxModifiedSet.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxModifiedSet.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $