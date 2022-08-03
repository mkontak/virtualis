// $Id: vxCropBox.h,v 1.9 2007/07/20 19:56:57 cqian Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <vector>
#include "vxElement.h"
#include "Box.h"
#include "Vector.h"
#include "Point.h"
#include "vxHandlesObj.h"


/**
 * A class to create and modify a cropbox.
 */
class VX_VIEWER_DLL vxCropBox : public vxElement, public vxHandlesObj
{
// Member Functions
public:
  
  /// Constructor
  vxCropBox();
  
  /// Destructor
  virtual ~vxCropBox();

  /// Create a crop box
  void CreateCropBox( const Point<float4> & fMeasurePoint, const Vector<float4> & vMajorAxis );
  
  /// Retrieve a list of handle locations
  virtual Point<float4> GetHandlePosition( uint2 uIndex ) const;

  /// Move the selected handle point
  virtual void SetHandlePosition( const Point<float4> & newMeasurePoint, int2 iHandleIndex, const uint4 uAxisIndex, Box<float4> fWorldBox );

  /// get number of handles
  virtual uint4 GetNumHandles() const;

  /// get the indexed handle
  virtual vxHandle GetHandle( const uint2 uHandleIndex ) const;

  /// set the indexed handle
  virtual void SetHandle( const uint2 uHandleIndex, const vxHandle & handle );

  /// Get the 3D box
  inline const Box<float4> & GetBox() const { return m_box; }

  /// Initialize the position, but don't calculate
  void SetBox( const Box<float4> & box ) { m_box = box; }

  /// Get the crop rectangle for a view
  const BoundingRect<float4> GetCropRect( const uint4 uAxisIndex );

  /// Get the active handle
  const int4 & GetActiveHandle() const { return m_iActiveHandle; }

  /// Set the active handle
  void SetActiveHandle( int4 iActiveHandle ) { m_iActiveHandle = iActiveHandle; }

// data members
private:

  /// The 3D box in volume coordinates
  Box<float4> m_box;

  /// the active handle
  int4 m_iActiveHandle;

}; // class vxCropBox


// $Log: vxCropBox.h,v $
// Revision 1.9  2007/07/20 19:56:57  cqian
// Check in for testing
//
// Revision 1.8  2007/03/22 01:08:13  geconomos
// removed incorrect "inline" fom GetCropRect
//
// Revision 1.7  2006/03/08 14:46:40  frank
// brought back the constant get property
//
// Revision 1.6  2006/03/07 14:19:50  vxconfig
// Removed the const specifier from the GetBox property due to  fix daily build
//
// Revision 1.5  2006/03/02 15:47:23  frank
// made a property constant
//
// Revision 1.4  2005/05/16 13:48:38  frank
// made the crop box easier to handle
//
// Revision 1.3  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.2  2004/03/11 17:41:28  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/05/22 14:57:30  dongqing
// code review
//
// Revision 1.7  2003/05/22 13:04:28  frank
// code review
//
// Revision 1.6  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.5  2003/01/22 21:44:16  ingmar
// added missing includes
//
// Revision 1.4  2002/08/09 20:54:25  uday
// GetHandlePoint() takes the volume dimensions into account.
//
// Revision 1.3  2002/08/09 15:40:33  uday
// Replaced Rect<> by BoundingRect<>.
//
// Revision 1.2  2002/08/08 15:52:19  uday
// Removed display m_longAxis.
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.4  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.3  2002/06/11 13:06:20  manju
// Made the destructor virtual.
//
// Revision 1.2  2002/06/10 16:42:00  dmitry
// beautify
//
// Revision 1.1  2002/06/10 15:13:04  manju
// Renamed AnnotationBOI as vxCropBox and moved it from 51_annotation to
// 74_vxUtils.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCropBox.h,v 1.9 2007/07/20 19:56:57 cqian Exp $
// $Id: vxCropBox.h,v 1.9 2007/07/20 19:56:57 cqian Exp $
