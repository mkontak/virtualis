// $Id: vxModifiedSet.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)
//
// Complete history at bottom of file.


#include "vxModifiedInfo.h"
#include "vxObserver.h"


// define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Constructor
 * @param pClient observer
 * @param pInfo modified information
 */
vxModifiedSet::vxModifiedSet(vxObserver * pClient, 
                             const vxModifiedInfo * pInfo)
//*******************************************************************
{
  // verify that the client is not null
  if (pClient == NULL)
  {
    throw ex::NullPointerException(LogRec("The client pointer is null.","vxModifiedSet","vxModifiedSet"));
  }
  m_pClient = pClient;
  
  if (pInfo != NULL) m_messages.PushBack(pInfo);
} // vxModifiedSet()


/**
 * Destructor
 */
vxModifiedSet::~vxModifiedSet()
//*******************************************************************
{
  // release all vxModifiedInfo objects
  m_messages.ReleaseObjects();
} // ~vxModifiedSet()


/**
 * Return a pointer to the client.
 * @return A pointer to the client.
 * /
vxObserver * vxModifiedSet::GetClient()
//*******************************************************************
{
  return m_pClient;
} // Client()


/**
 * Allows access to the collection of messages.
 * @return A reference to the messages.
 * /
vxModifiedInfoList & vxModifiedSet::GetMessages()
//*******************************************************************
{
  return m_messages;
} // Messages()


/**
 * Appends the collection of messages.
 * @param pInfo Modified information.
 */
void vxModifiedSet::AddMessage(const vxModifiedInfo * pInfo)
//*******************************************************************
{
  if (pInfo != NULL)
  {
    m_messages.PushBack(pInfo);
  }
} // AddMessage()


#undef FILE_REVISION


// Revision History:
// $Log: vxModifiedSet.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/12/16 13:25:07  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxModifiedSet.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxModifiedSet.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $