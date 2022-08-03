// $Id: vxShareableObject.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)


// includes
#include "vxModifiedQueue.h"
#include "vxModifiedInfo.h"
#include "vxModifiedInfoList.h"
#include "vxObserver.h"


/**
 * Connects a client to receive events
 * @param pClient Pointer to a client
 * @return The number of clients curently connected to this object
 */
template<class T>
uint2 vxShareableObject<T>::Connect(vxObserver * pClient)
//*******************************************************************
{
  Disconnect(pClient);

  // reference count
  std::map<vxObserver *, int4>::iterator iter(ObserverReferenceCountMap().find(pClient));

  if (iter != ObserverReferenceCountMap().end())
    ++(*iter).second; 
  else 
    ObserverReferenceCountMap().insert(std::make_pair(pClient, 1));

  m_clients.push_back(pClient);

  return m_clients.size();
} // Connect()


/**
 * Disconects a client and deletes all messages to it
 * @param pClient Pointer to a client
 * @return The number of clients curently connected to this object
 */
template<class T>
uint2 vxShareableObject<T>::Disconnect(vxObserver * pClient)
//*******************************************************************
{
  if (std::find(m_clients.begin(), m_clients.end(), pClient) != m_clients.end())
  {
    m_clients.remove(pClient);
    
    // reference count
    std::map<vxObserver *, int4>::iterator iter(ObserverReferenceCountMap().find(pClient));
    if (iter != ObserverReferenceCountMap().end())
    {
      if ((*iter).second == 1) 
      {
        ObserverReferenceCountMap().erase(iter);
        vxModifiedQueue::Disconnected(pClient);
      }
    }
  } // if client in list

  return m_clients.size();
} // Disconnect()


/**
 * Queues the modified information to be sent to the clients
 * @param info           list of modified messages
 * @param bPrimaryOnly   flag indicating if only the primary observer should get the notification
 */
template<class T>
void vxShareableObject<T>::Modified(vxModifiedInfoList & info, 
                                    bool bPrimaryOnly)
//*******************************************************************
{
  for (const vxModifiedInfo * pInfo(info.Begin()); pInfo != NULL; pInfo = info.Next())
  {
    Modified(*pInfo, bPrimaryOnly);
  }
} // Modified()


/**
 * Queues the modified information to be sent to the clients
 * @param pInfo          modified message
 * @param bPrimaryOnly   flag indicating if only the primary observer should get the notification
 */
template<class T>
void vxShareableObject<T>::Modified(const vxModifiedInfo & info, 
                                    bool bPrimaryOnly)
//*******************************************************************
{
  // set the sender of the event
  const_cast<vxModifiedInfo & >(info).SetSender( this );
  
  if (bPrimaryOnly == true && PrimaryObserver() != NULL)
  {
    vxModifiedQueue::Push(PrimaryObserver(), info);
  }
  else
  {
    for (std::list<vxObserver *>::iterator iter(m_clients.begin()); iter != m_clients.end(); ++iter)
    {
      vxModifiedQueue::Push((*iter), info);
    }
  }
} // Modified()


// Revision History:
// $Log: vxShareableObject.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/25 12:49:06  michael
// cosmetics
//
// Revision 1.5  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.4  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.3  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.2  2003/01/06 18:56:00  ingmar
// moved static instance to static member functions
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.11  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.10  2002/07/15 22:54:57  ingmar
// removed pointer delete to allow compolation NEED TO FIX THIS SO THERE IS NO MEMORY LEAK
//
// Revision 1.9  2002/07/08 15:25:27  dmitry
// Changed to work with vxModifiedInfoList.
//
// Revision 1.8  2002/06/13 19:24:05  michael
// whatever ... Dmitry did this on my machine
//
// Revision 1.7  2002/06/12 20:55:19  michael
// added funtionality so that on mouse drag window/level only gets updated in
// current viewer, update on all others OnButtonUp()
//
// Revision 1.6  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.5  2002/05/10 21:13:55  michael
// cosmetics
//
// Revision 1.4  2002/05/07 18:23:08  frank
// Moved includes from .inl file to .h file.
//
// Revision 1.3  2002/05/01 13:49:00  michael
// Started code review related stuff ...
//
// Revision 1.2  2002/04/17 19:24:13  dmitry
// Switched to the new event handling architecture.
//
// Revision 1.1  2002/03/25 00:51:53  michael
// added the renamed fil4es to 74_vxUtiles, avoiding forward dependencies
//
// Revision 1.2  2002/03/13 19:15:54  dmitry
// Changed method Reset to ResetSinks.
//
// Revision 1.1  2002/03/07 22:46:42  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxShareableObject.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxShareableObject.inl,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
