// $Id: vxMesh.C,v 1.1 2007/07/14 01:23:12 frank Exp $
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
#include "vxMesh.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


/**
 * Constructor
 */
vxMesh::vxMesh()
{
} // vxMesh()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxMesh::vxMesh( const vxMesh & other ) :
m_mesh( other.m_mesh )
{
} // vxMesh( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxMesh & vxMesh::operator =( const vxMesh & other )
{
  m_mesh = other.m_mesh;
  return * this;
} // operator =( other )


// undefines
#undef FILE_REVISION


// $Log: vxMesh.C,v $
// Revision 1.1  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMesh.C,v 1.1 2007/07/14 01:23:12 frank Exp $
// $Id: vxMesh.C,v 1.1 2007/07/14 01:23:12 frank Exp $
