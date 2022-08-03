// $Id: vxColoredVertex.h,v 1.1 2007/07/14 01:23:12 frank Exp $
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
#include "Point3D.h"
#include "Normal3D.h"
#include "PixelRGBA.h"


/**
 * A data structure that holds a 2D colored mesh
 */
class VX_VIEWER_DLL vxColoredVertex
{
public:

  /// default constructor
  vxColoredVertex();

  /// copy constructor
  vxColoredVertex( const vxColoredVertex & other );

  /// assignment operator
  vxColoredVertex & operator =( const vxColoredVertex & other );

  /// get the position
  const Point3Df & GetPosition() const { return m_position; }

  /// set the position
  void SetPosition( const Point3Df & value ) { m_position = value; }

  /// get the normal
  const Normal3Df & GetNormal() const { return m_normal; }

  /// set the normal
  void SetNormal( const Normal3Df & value ) { m_normal = value; }

  /// get the color
  const PixelRGBA< uint1 > & GetColor() const { return m_color; }

  /// set the color
  void SetColor( const PixelRGBA< uint1 > & value ) { m_color = value; }

private:

  /// the position
  Point3Df m_position;

  /// the normal direction
  Normal3Df m_normal;

  /// the color
  PixelRGBA< uint1 > m_color;

}; // class vxColoredVertex


// $Log: vxColoredVertex.h,v $
// Revision 1.1  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxColoredVertex.h,v 1.1 2007/07/14 01:23:12 frank Exp $
// $Id: vxColoredVertex.h,v 1.1 2007/07/14 01:23:12 frank Exp $
