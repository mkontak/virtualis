// $Id: MenubarItem.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

OPEN_WINUTILS_NS

// class declartion
class MenubarItem
{
// enums
public:

  // available states
  enum MenuBarItemStateEnum
  {
    NORMAL,
    HOVER,
    SELECTED
  };

// member functions
public:
  
  /// constructor
  MenubarItem();

  /// constructor from position and text
  MenubarItem( const int4 iPosition, const std::string & sText );
  
  /// destructor
  virtual ~MenubarItem();

  /// gets the item position
  const int4 GetPosition() const { return m_iPosition; }

  /// sets the item position
  void SetPosition( const int4 iPosition ) { m_iPosition = iPosition; }

  /// gets the item text
  const std::string & GetText() const { return m_sText; }

  /// sets the item text
  void SetText( const std::string & sText ) { m_sText = sText; }

  /// gets the item state
  const MenuBarItemStateEnum GetState() const { return m_eState; }

  /// sets the item state
  void SetState( const MenuBarItemStateEnum eState ) { m_eState = eState; }

  /// gets the item size in pixels 
  const std::pair< float4,float4 > & GetSize() const { return m_size; }

  /// sets the item size in pixels
  void SetSize( const std::pair< float4,float4 > & size ) { m_size = size; }

// member variables
private:
  
  //  text
  std::string m_sText;

  // state
  MenuBarItemStateEnum m_eState;

  // size in pixels
  std::pair< float4, float4 > m_size;

  // position of item within menu
  int4 m_iPosition;

}; // class MenubarItem

CLOSE_WINUTILS_NS

// $Log: MenubarItem.h,v $
// Revision 1.3  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.2  2007/03/01 21:40:08  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MenubarItem.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
// $Id: MenubarItem.h,v 1.3 2007/03/13 03:10:51 jmeade Exp $
