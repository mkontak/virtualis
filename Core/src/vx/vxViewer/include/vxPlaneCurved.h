// $Id: vxPlaneCurved.h,v 1.2 2005/03/08 19:09:24 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)

#ifndef vxPlaneCurved_h
#define vxPlaneCurved_h


// includes
#include "vxPlane.h"
#include "Typedef.h"
#include <iostream>


/**
 * This class provides a curved plane description
 */
class VX_VIEWER_DLL vxPlaneCurved : public vxPlane
{
// functions
public:

  /// default constructor
  vxPlaneCurved();
  
  /// constructor providing an angle 
  vxPlaneCurved(const float4 &);
  
  /// copy constructor
  vxPlaneCurved(const vxPlaneCurved & other);

  /// destrutor
  ~vxPlaneCurved();   
  
  /// assignment operator
  const vxPlaneCurved & operator=(const vxPlaneCurved & other);

  /// return the angle of the plane
  float4 GetAngle() const { return m_fAngle; }
  
  /// sets the angle of the plane
  void SetAngle(const float4 & fAngle);
  
  /// return the width of the plane
  float4 GetWidth() const { return m_fWidth; }
  
  /// sets the width of the plane
  void SetWidth(const float4 & fWidth) { m_fWidth = fWidth; }
    
// data
private:

  /// angle of the plane
  float4 m_fAngle; // [0,359]

  /// Width of the plane in millimeters
  float4 m_fWidth;

}; // class vxPlaneCurved


#endif // vxPlaneCurved_h


// $Log: vxPlaneCurved.h,v $
// Revision 1.2  2005/03/08 19:09:24  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/16 20:26:53  michael
// Implemented copy constructor and assignment operator
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/09/09 13:29:19  frank
// formatting
//
// Revision 1.8  2003/08/20 14:18:58  michael
// added doxygen @param
//
// Revision 1.7  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 1.6  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.5  2003/05/14 19:07:04  dongqing
// code review
//
// Revision 1.4  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.3  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.2  2002/11/18 16:35:34  michael
// added source file for include (inline)
//
// Revision 1.5  2002/08/20 14:58:27  wenli
// initial version - Vascular
//
// Revision 1.4  2002/06/19 19:29:23  frank
// Added width field.
//
// Revision 1.3.2.2  2002/05/17 23:16:04  frank
// Added width.
//
// Revision 1.3.2.1  2002/05/07 21:56:17  frank
// Removed old comment.
//
// Revision 1.3  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 1.2  2002/03/06 15:38:43  jaddonisio
// Fixed UnitTest().
//
// Revision 1.1  2002/03/06 00:20:49  jaddonisio
// Initial version.
//
// Revision 1.5  2002/03/04 20:33:23  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.4  2002/02/27 21:00:20  michael
// coding guidelines
//
// Revision 1.3  2002/02/26 17:38:13  michael
// fixed copy constructor, assignment operator and made methods const that do not alter member data
//
// Revision 1.2  2002/02/25 13:21:12  michael
// formatting
//
// Revision 1.1  2002/02/19 22:26:03  manju
// Curved Plane.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxPlaneCurved.h,v 1.2 2005/03/08 19:09:24 frank Exp $
// $Id: vxPlaneCurved.h,v 1.2 2005/03/08 19:09:24 frank Exp $
