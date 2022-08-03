// $Id: vxShareableObject.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

#ifndef vxShareableObject_h
#define vxShareableObject_h


// includes


// forward declarations
class vxObserver;
class vxModifiedInfo;
class vxModifiedInfoList;


/// Helper class required to instantiate a static member variable
/// within a template class.
class VX_VIEWER_DLL vxShareableObjectHelper
{
// functions
public:

  /// connects the primary client to receive events
  static void ConnectPrimary(vxObserver * pClient) { PrimaryObserver() = pClient; }

// data
protected:

  /// primary observer
  static vxObserver * & PrimaryObserver();

  /// keeps reference count of connected clients
  static std::map<vxObserver *, int4> & ObserverReferenceCountMap();
};


// class definition
template <class T> 
class vxShareableObject: public vxShareableObjectHelper, public T
{
// functions
public:

  /// default constructor
  explicit vxShareableObject<T>() {};

  /// constructor
  explicit vxShareableObject<T>(const T & other)
  {
    *dynamic_cast<T*>(this) = other;
  }

  /// assignment operator
  vxShareableObject<T> & operator=(const vxShareableObject<T> & other) 
  { 
    *dynamic_cast<T*>(this) = *dynamic_cast<const T *>(&other);
    m_clients = other.m_clients;
    return *this; 
  }

  /// assignment operator FOR OBJECT ONLY
  vxShareableObject<T> & operator=(const T & other) 
  { 
    *dynamic_cast<T*>(this) = other;
    return *this; 
  }
  
  /// queues the modified information to be sent to the clients
  virtual inline void Modified(vxModifiedInfoList & modInfoList, bool bPrimaryOnly = false);

  /// queues the modified information to be sent to the clients
  virtual inline void Modified(const vxModifiedInfo & modInfoList, bool bPrimaryOnly = false);
  
  /// connects a client to receive events
  inline uint2 Connect(vxObserver * pObserver);

  /// disconnects a client and deletes all messages to it
  inline uint2 Disconnect(vxObserver * pObserver);

private:

  // copy constructor
  vxShareableObject<T>(const vxShareableObject<T> & other); // should only be public if implemented

// data
private:

  // list of clients
  std::list<vxObserver *> m_clients;
}; // class vxShareableObject


#include "vxShareableObject.inl"


#endif // vxShareableObject_h


// Revision History:
// $Log: vxShareableObject.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/25 12:49:06  michael
// cosmetics
//
// Revision 1.5  2003/05/16 11:45:02  michael
// code review
//
// Revision 1.4  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.3  2003/01/06 18:56:00  ingmar
// moved static instance to static member functions
//
// Revision 1.2  2002/10/01 13:47:09  michael
// Made constructors explicit and added another assignedment operator=(class <T>)
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.14  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.13  2002/07/08 15:25:27  dmitry
// Changed to work with vxModifiedInfoList.
//
// Revision 1.12  2002/07/02 20:44:58  geconomos
// Exporting classes .
//
// Revision 1.11  2002/06/27 19:12:54  michael
// removed unnecessary include
//
// Revision 1.10  2002/06/13 19:24:05  michael
// whatever ... Dmitry did this on my machine
//
// Revision 1.9  2002/06/12 20:55:19  michael
// added funtionality so that on mouse drag window/level only gets updated in
// current viewer, update on all others OnButtonUp()
//
// Revision 1.8  2002/06/11 14:58:07  jenny
// added <algorithm>
//
// Revision 1.7  2002/06/07 19:59:24  geconomos
// Added <algorithm>
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
// Revision 1.3  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.2  2002/04/17 19:24:13  dmitry
// Switched to the new event handling architecture.
//
// Revision 1.1  2002/03/25 00:51:53  michael
// added the renamed fil4es to 74_vxUtiles, avoiding forward dependencies
//
// Revision 1.2  2002/03/13 19:15:55  dmitry
// Changed method Reset to ResetSinks.
//
// Revision 1.1  2002/03/07 22:46:42  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxShareableObject.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxShareableObject.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
