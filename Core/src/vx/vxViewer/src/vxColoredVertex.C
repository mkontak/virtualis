// $Id: vxColoredVertex.C,v 1.1 2007/07/14 01:23:11 frank Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A data structure that holds a 2D colored mesh
 */


// includes
#include "StdAfx.h"
#include "vxColoredVertex.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Constructor
 */
vxColoredVertex::vxColoredVertex()
{
} // vxColoredVertex()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxColoredVertex::vxColoredVertex( const vxColoredVertex & other ) :
m_position( other.m_position ),
m_normal( other.m_normal ),
m_color( other.m_color )
{
} // vxColoredVertex( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxColoredVertex & vxColoredVertex::operator =( const vxColoredVertex & other )
{
  m_position = other.m_position;
  m_normal = other.m_normal;
  m_color = other.m_color;
  return * this;
} // operator =( other )


// undefines
#undef FILE_REVISION


// $Log: vxColoredVertex.C,v $
// Revision 1.1  2007/07/14 01:23:11  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxColoredVertex.C,v 1.1 2007/07/14 01:23:11 frank Exp $
// $Id: vxColoredVertex.C,v 1.1 2007/07/14 01:23:11 frank Exp $
