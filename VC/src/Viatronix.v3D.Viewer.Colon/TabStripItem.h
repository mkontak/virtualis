// $Id: TabStripItem.h,v 1.2 2007/03/02 14:33:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragma declarations
#pragma once

// namespaces
OPEN_WINUTILS_NS

// class declaration
class TabStripItem
{
// member functions
public:
  
  // constructor
  TabStripItem();

  // constructor from text
  TabStripItem( const std::string sText );

  /// copy constructor
  TabStripItem( const TabStripItem & other );

  // desturctor
  virtual ~TabStripItem();

  /// gets the text
  const std::string & GetText() const { return m_sText; }

  /// sets the text
  void SetText( const std::string & sText ) { m_sText = sText; }

  /// gets the enabled flag
  const bool & GetEnabled() const { return m_bEnabled; }

  /// sets the enabled flag
  void SetEnabled( const bool & bEnabled ) { m_bEnabled = bEnabled; }

  /// gets the bounds
  const Gdiplus::Rect & GetBounds() const { return m_bounds; }

  /// sets the bounds
  void SetBounds( const Gdiplus::Rect & bounds ) { m_bounds = bounds; }

  /// gets the data
  const void * GetData() const { return m_pData; }

  /// sets the data
  void SetData( void * pData ) { m_pData = pData; }

  /// assignment operator
  TabStripItem & operator =( const TabStripItem & rhs );

private:

  // common initialization
  void CommonInit();

// member variables
private:
  
  /// the text of this item
  std::string m_sText;

  /// if this item is enabled
  bool m_bEnabled;

  /// the bounds of this item
  Gdiplus::Rect m_bounds;

  /// the data of this item
  void * m_pData;

}; // class TabStripItem

CLOSE_WINUTILS_NS

// $Log: TabStripItem.h,v $
// Revision 1.2  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TabStripItem.h,v 1.2 2007/03/02 14:33:04 geconomos Exp $
// $Id: TabStripItem.h,v 1.2 2007/03/02 14:33:04 geconomos Exp $
