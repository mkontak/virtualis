// $Id: v3D_ViewerbarButtonCollection.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_ViewerbarButton.h"

// namespaces
OPEN_VISUALIZATION_NS

// forward managed declarations
interface class IViewerbarButton;
ref class Viewerbar;

// class definition
public ref class ViewerbarButtonCollection : 
  public System::Collections::ICollection,
  public System::Collections::IEnumerable,
  public System::Collections::IList
{

// ICollection implementation
public:

  /// returns the numer of items in the collection
  virtual property int Count { int get() { return m_mpItems->Count; }}

  /// returns a value indicating whether access to the collection is synchronized (thread-safe)
  virtual property bool IsSynchronized { bool get() { return m_mpItems->IsSynchronized; }}

  /// return an object that can be used to synchronize access to the collection.
  virtual property System::Object ^ SyncRoot { System::Object ^ get() { return m_mpItems->SyncRoot; }}

  /// copies from the specified array into the collection starting at the given index
  virtual void CopyTo( System::Array ^ mpArray, int iIndex );

// IEnumerable implementation
public:

  // return the IEnumerator interface for the collection
  virtual System::Collections::IEnumerator ^ GetEnumerator() { return m_mpItems->GetEnumerator(); }

// IList implementation
public:

  /// returns if the collection is a fixed size
  virtual property bool IsFixedSize { bool get() { return false; }}

  /// return if the collection is read only
  virtual property bool IsReadOnly { bool get() { return false; }}

  /// returns the item at the specified index
  virtual property System::Object ^ default[ int ]
  {
    System::Object ^ get( int iIndex ) { return m_mpItems[ iIndex ]; }
    void set( int iIndex, System::Object ^ mpItem ) { m_mpItems[ iIndex ] = mpItem; }
  }

  /// adds the specified item at the end of the collection
  virtual int Add( System::Object ^ mpItem );

  /// clears out the collection
  virtual void Clear();

  /// returns if the collection contains the specified item
  virtual bool Contains( System::Object ^ mpItem );

  /// returns the index of the specified item
  virtual int IndexOf( System::Object ^ mpItem );

  /// inserts the specified item at the specified index
  virtual void Insert( int iIndex, System::Object ^ mpItem );

  /// removes the specified object
  virtual void Remove( System::Object ^ mpItem );
    
  /// removes the item at the sepcified index
  virtual void RemoveAt( int iIndex );

  // adds a range of buttons to the collection
  virtual void AddRange( array< Viatronix::Visualization::ViewerbarButton ^ > ^ mpButtons );

// functions
public:

  /// initializer
  ViewerbarButtonCollection( Viatronix::Visualization::Viewerbar ^ mpViewerbar );

// data
private:

  // internal implementation of collection
  System::Collections::ArrayList ^ m_mpItems;

  // parent viewer of viewerbar
  Viatronix::Visualization::Viewerbar ^ m_mpViewerbar;

}; // class ViewerbarButtonCollection

CLOSE_VISUALIZATION_NS

// $Log: v3D_ViewerbarButtonCollection.h,v $
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2004/10/13 16:04:34  geconomos
// implementing as a collection ViewerbarButtons instead of IViewerbarButtons.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.6  2003/05/16 13:13:57  frank
// formatting
//
// Revision 1.5  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.4  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.3  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.2  2003/03/20 13:45:48  geconomos
// Added forward declaration of ViewerBar.
//
// Revision 1.1  2002/11/18 19:59:23  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerbarButtonCollection.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_ViewerbarButtonCollection.h,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
