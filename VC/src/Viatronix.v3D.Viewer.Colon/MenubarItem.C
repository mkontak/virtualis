// $Id: MenubarItem.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// includes
#include "StdAfx.h"
#include "MenubarItem.h"

// namespaces
using namespace WinUtils;


/**
 * Constructor
 */
MenubarItem::MenubarItem() : 
  m_iPosition( -1 ),
  m_eState( MenubarItem::NORMAL )
{
} // MenubarItem()


/**
 * Constructor from position and text
 *
 * @param   iPosition   menu item position
 * @param   sText       item text
 */
MenubarItem::MenubarItem( const int4 iPosition, const std::string & sText ) :
  m_iPosition( iPosition ),
  m_sText( sText ),
  m_eState( MenubarItem::NORMAL )
{
} // MenubarItem( const std::string & sText )


/**
 * Destructor
 */
MenubarItem::~MenubarItem()
{
} // ~MenubarItem()


// $Log: MenubarItem.C,v $
// Revision 1.3  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.2  2007/03/01 21:40:08  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MenubarItem.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
// $Id: MenubarItem.C,v 1.3 2007/03/13 03:10:51 jmeade Exp $
