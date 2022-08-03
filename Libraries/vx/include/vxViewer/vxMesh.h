// $Id: vxMesh.h,v 1.2 2008/05/02 04:59:07 cqian Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <vector>
#include "vxElement.h"
#include "vxColoredVertex.h"


/**
 * A data structure that holds a 2D colored mesh
 */
class VX_VIEWER_DLL vxMesh : public vxElement
{
public:

  /// default constructor
  vxMesh();

  /// copy constructor
  vxMesh( const vxMesh & other );

  /// assignment operator
  vxMesh & operator =( const vxMesh & other );

  /// get the 2D array of vertices
  std::vector< std::vector< vxColoredVertex > > & GetMesh() { return m_mesh; }


private:

  /// 2D array of vertices
  std::vector< std::vector< vxColoredVertex > > m_mesh;

}; // class vxMesh


// $Log: vxMesh.h,v $
// Revision 1.2  2008/05/02 04:59:07  cqian
// check in
//
// Revision 1.1  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMesh.h,v 1.2 2008/05/02 04:59:07 cqian Exp $
// $Id: vxMesh.h,v 1.2 2008/05/02 04:59:07 cqian Exp $
