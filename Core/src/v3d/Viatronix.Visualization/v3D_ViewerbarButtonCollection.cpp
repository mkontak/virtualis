// $Id: v3D_ViewerbarButtonCollection.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "stdafx.h"
#include "v3D_ViewerbarButtonCollection.h"
#include "v3D_Viewerbar.h"

// namespaces
USING_VISUALIZATION_NS

/**
 * Initializes a ViewerbarButtonCollection object with the specified Viewerbar object.
 *
 * @param mpViewerbar Parent Viewerbar object.
 */
 ViewerbarButtonCollection::ViewerbarButtonCollection( Viewerbar ^ mpViewerbar )
{
  m_mpViewerbar = mpViewerbar;

  m_mpItems = gcnew System::Collections::ArrayList();
} // ViewerbarButtonCollection()


/**
 *  Copies from the specified array into the collection starting at the given index
 *
 * @param mpArray source array to copy from 
 * @param iIndex starting index to copy from
 */
void ViewerbarButtonCollection::CopyTo( System::Array ^ mpArray, int iIndex )
{
  m_mpItems->CopyTo( mpArray, iIndex );
  m_mpViewerbar->RecalculateLayout();
} // CopyTo()


/**
 * Adds the specified item at the end of the collection
 *
 * @param mpItem item to add
 * @return  index of added item
 */
int ViewerbarButtonCollection::Add( System::Object ^ mpItem )
{
  int iIndex(m_mpItems->Add( mpItem ));
  m_mpViewerbar->RecalculateLayout();

  return iIndex;
} // Add()


/**
 * Clears out the collection
 */
void ViewerbarButtonCollection::Clear()
{
  m_mpItems->Clear();
  m_mpViewerbar->RecalculateLayout();
} //Clear()


/**
 * Returns if the collection contains the specified item
 *
 * @param mpItem item to locate within the collection
 * @return true if the item is contained within the collection , false otherwise
 */
bool ViewerbarButtonCollection::Contains( System::Object ^ mpItem )
{
  return m_mpItems->Contains( mpItem );
} // Contains()


/**
 * Returns the index of the specified item
 *
 * @param mpItem item to find index of
 * @retun index of item within collection, -1 if not found
 */
int ViewerbarButtonCollection::IndexOf( System::Object ^ mpItem )
{
  return m_mpItems->IndexOf( mpItem );
} // IndexOf()

/**
 * Inserts the specified item at the specified index
 *
 * @param iIndex index to isert item
 * @param mpItem item to insert
 */
void ViewerbarButtonCollection::Insert( int iIndex, System::Object ^ mpItem )
{
  m_mpItems->Insert( iIndex, mpItem );
  m_mpViewerbar->RecalculateLayout();
} // Insert()


/**
 * Removes the specified object
 *
 * @param mpItem item to remove
 */
void ViewerbarButtonCollection::Remove( System::Object ^ mpItem )
{
  m_mpItems->Remove( mpItem );
  m_mpViewerbar->RecalculateLayout();
} // Remove()
    

/**
 * Removes the item at the sepcified index
 *
 * @param iIndex index of item to remove
 */
void ViewerbarButtonCollection::RemoveAt( int iIndex )
{
  m_mpItems->RemoveAt( iIndex );
  m_mpViewerbar->RecalculateLayout();
} // RemoveAt()


/**
 * Adds a range of buttons to the collection
 *
 * @param mpButtons buttons to add the collection
 */
void ViewerbarButtonCollection::AddRange( array< ViewerbarButton ^ > ^ mpButtons )
{
  for ( int index(0); index < mpButtons->Length; ++index )
    m_mpItems->Add( mpButtons[ index ] );

  m_mpViewerbar->RecalculateLayout();
} // AddRange()


// $Log: v3D_ViewerbarButtonCollection.cpp,v $
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
// Revision 1.6  2003/08/20 14:28:56  michael
// added "mailto:"
//
// Revision 1.5  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.4  2003/05/13 19:08:32  michael
// code review
//
// Revision 1.3  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.2  2003/05/13 13:28:47  michael
// code review
//
// Revision 1.1  2002/11/18 19:59:23  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_ViewerbarButtonCollection.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
// $Id: v3D_ViewerbarButtonCollection.cpp,v 1.3 2006/10/04 18:29:06 gdavidson Exp $
