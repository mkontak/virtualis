// $Id: TabStripItem.C,v 1.2 2007/03/02 14:33:04 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// include declarations
#include "StdAfx.h"
#include "TabStripItem.h"

// namespaces
using namespace WinUtils;


/**
 * Consturctor
 */
TabStripItem::TabStripItem() 
{
  CommonInit();
} // TabStripItem()


/**
 * Constructor from text
 *
 * @param   sText   item text
 */
TabStripItem::TabStripItem( const std::string sText ) : m_sText( sText )
{
  CommonInit();
} // TabStripItem( const std::string sText )


/**
 * Copy constructor
 *
 * @param other  the other object
 */
TabStripItem::TabStripItem( const TabStripItem & other ) :
  m_sText( other.m_sText ),
  m_bounds( other.m_bounds ),
  m_pData( other.m_pData ),
  m_bEnabled( other.m_bEnabled )
{
} // TabStripItem( const TabStripItem & other ) 


/**
 * Destructor
 */
TabStripItem::~TabStripItem() 
{
} // ~TabStripItem()


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
TabStripItem & TabStripItem::operator =( const TabStripItem & rhs )
{
  if( this != &rhs )
  {
    m_sText = rhs.m_sText;
    m_bounds = rhs.m_bounds;
    m_pData = rhs.m_pData;
    m_bEnabled = rhs.m_bEnabled;
  }
  return * this;
} // operator =( const TabStripItem & rhs )


/**
 * Common initialization
 */
void TabStripItem::CommonInit()
{
  m_pData = NULL;
  m_bEnabled = true;
} // CommonInit()



// $Log: TabStripItem.C,v $
// Revision 1.2  2007/03/02 14:33:04  geconomos
// code review preparation
//
// Revision 1.1  2005/10/03 12:57:54  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TabStripItem.C,v 1.2 2007/03/02 14:33:04 geconomos Exp $
// $Id: TabStripItem.C,v 1.2 2007/03/02 14:33:04 geconomos Exp $
