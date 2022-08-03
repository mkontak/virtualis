// $Id: vxZoomFactor.h,v 1.2 2004/04/26 19:01:00 michael Exp $
//
// Copyright © 2004 , Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


/*
 This class is the class to capture the zooming. It encapsulates 
 the zoom center as well as the width of how much to view. The
 width is given in world size.
*/

#ifndef vxZoomFactor_h
#define vxZoomFactor_h


// includes
#include "Point.h"


// forward declarations
class vxEnvironment;


// class definition
class VX_VIEWER_DLL vxZoomFactor
{
// functions
public:

  /// constructor
  vxZoomFactor() {}

  /// destructor
  virtual ~vxZoomFactor() {}

  /// get the zoom center
  Point3Df GetCenter() const { return m_center; }

  /// set the zoom center
  void SetCenter(const Point3Df & center) { m_center = center; }

  /// get the zoom area size
  float4 GetSize() const { return m_fSize; }

  /// set the zoom area size
  void SetSize(const float4 & fSize) { m_fSize = fSize; }

  /// copy constructor
  vxZoomFactor(const vxZoomFactor & other);

  /// assignment operator
  const vxZoomFactor & operator=(const vxZoomFactor & other);

// data
private:

  /// center of zoom
  Point3Df m_center; 

  /// size of zoom area
  float4 m_fSize;
}; // class vxZoomFactor


#endif // vxZoomFactor_h


// Revision History:
// $Log: vxZoomFactor.h,v $
// Revision 1.2  2004/04/26 19:01:00  michael
// made Get() methods const
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/02/21 03:03:52  michael
// added missing constructor (empty)
//
// Revision 1.1  2004/02/20 23:32:50  michael
// added new zoom factor class
//
// $Id: vxZoomFactor.h,v 1.2 2004/04/26 19:01:00 michael Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxZoomFactor.h,v 1.2 2004/04/26 19:01:00 michael Exp $
