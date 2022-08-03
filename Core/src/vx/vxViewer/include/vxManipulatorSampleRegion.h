// $Id: vxManipulatorSampleRegion.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (mailto:jaddonisio@viatronix.com)

/*
  Class definition for vxManipulatorSampleRegion
*/

#ifndef vxManipulatorSampleRegion_h
#define vxManipulatorSampleRegion_h


// includes
#include "vxManipulator.h"


// class definition
class VX_VIEWER_DLL vxManipulatorSampleRegion : public vxManipulator
{
// functions
public:

  /// constructor
  vxManipulatorSampleRegion(vxEnvironment & environment);

  /// destructor
  virtual ~vxManipulatorSampleRegion() {};

  /// process mouse move event
  virtual bool OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState);
  
  /// process mouse wheel event
  virtual bool OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState);

  /// process key pressed event
  virtual bool OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState);

  /// process mouse leave event
  virtual void OnMouseLeave();

private:

  /// copy constructor
  vxManipulatorSampleRegion(vxManipulatorSampleRegion & other);                   // should only be public if really implemented!

  /// assignment operator
  const vxManipulatorSampleRegion & operator=(vxManipulatorSampleRegion & other); // should only be public if really implemented!

  /// perform sampling using the stored mouse position after a change in parameters
  void PerformSampling();

}; // vxManipulatorSampleRegion()


#endif // #ifdnef vxManipulatorSampleRegion_h


// Revision History:
// $Log: vxManipulatorSampleRegion.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2004/01/23 16:24:41  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.1  2003/12/19 13:50:05  michael
// Renaming: vxManipulatorRegionSampler -> vxManipulatorSampleRegion
//
// Revision 1.9  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.8  2003/05/07 21:00:21  dongqing
// code walkthrough
//
// Revision 1.7  2003/05/07 13:17:31  michael
// initial code review
//
// Revision 1.6  2003/01/03 16:58:46  frank
// Extended region sampling to 3D spherical region.
//
// Revision 1.5  2002/10/17 22:16:47  geconomos
// Removed VX_VIEWER_DLL from class specification
//
// Revision 1.4  2002/08/20 22:43:20  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.3  2002/07/02 20:44:59  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/26 14:41:04  jaddonisio
// Set rendering mode to VIEW_CHANGING to make 3D faster.
//
// Revision 1.1  2002/06/21 18:02:11  jaddonisio
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxManipulatorSampleRegion.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxManipulatorSampleRegion.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
