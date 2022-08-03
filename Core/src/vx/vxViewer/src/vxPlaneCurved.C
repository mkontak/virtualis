// $Id: vxPlaneCurved.C,v 1.2 2005/03/08 19:09:13 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxPlaneCurved.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
vxPlaneCurved::vxPlaneCurved()
: m_fAngle( 0.0F ),
m_fWidth( 50.0F )
{  
} // vxPlaneCurved()


/**
 * Constructor
 *
 * @param fAngle angle from which to initialize the plane
 */
vxPlaneCurved::vxPlaneCurved( const float4 & fAngle )
: m_fWidth( 50.0F )
{
  m_fAngle = fAngle;
} // vxPlaneCurved()


/**
 * Destructor
 */
vxPlaneCurved::~vxPlaneCurved()
{
} // ~vxPlaneCurved()


/**
 * Copy constructor
 * @param plane
 */
vxPlaneCurved::vxPlaneCurved(const vxPlaneCurved & plane)
{
  *this = plane;
} // Copy constructor


/**
 * assignment operator
 * @param plane
 */
const vxPlaneCurved & vxPlaneCurved::operator =(const vxPlaneCurved & plane)
{
  if (this != & plane)
  {
    m_fAngle = plane.m_fAngle;
    m_fWidth = plane.m_fWidth;
  }  
  return *this;
} // operator =

/**
 * Set the angle
 *
 * @param fAngle angle by how much to move the plane
 */
void vxPlaneCurved::SetAngle( const float4 & fAngle )
{

  m_fAngle = fAngle;

  // keep the angle bweteen 0-360
  m_fAngle = Bound( 0.0F, m_fAngle, 360.0F );

} // SetAngle()


#undef FILE_REVISION


// $Log: vxPlaneCurved.C,v $
// Revision 1.2  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/16 20:26:53  michael
// Implemented copy constructor and assignment operator
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/09/09 13:29:34  frank
// formatting
//
// Revision 1.8  2003/08/20 14:18:57  michael
// added doxygen @param
//
// Revision 1.7  2003/05/16 13:08:50  frank
// code formatting
//
// Revision 1.6  2003/05/16 11:48:48  michael
// code review
//
// Revision 1.5  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.4  2003/05/14 20:31:22  dongqing
// code review
//
// Revision 1.3  2003/04/24 13:46:28  michael
// initial code reviews
//
// Revision 1.2  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.1  2002/11/18 16:12:37  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.1  2002/11/18 15:53:08  michael
// added renamed versions of the plane objects using vx prefix
//
// Revision 1.4  2002/06/19 19:29:22  frank
// Added width field.
//
// Revision 1.3.2.2  2002/05/21 22:23:41  frank
// Made it default to be wider.
//
// Revision 1.3.2.1  2002/05/17 23:16:04  frank
// Added width.
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
// Revision 1.6  2002/03/04 20:33:23  michael
// adjusted all PlaneOrtho and PlaneOblique instantiations with the new
// templetized types ...
//
// Revision 1.5  2002/03/04 16:43:27  michael
// fomatting and coding guidelines
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxPlaneCurved.C,v 1.2 2005/03/08 19:09:13 frank Exp $
// $Id: vxPlaneCurved.C,v 1.2 2005/03/08 19:09:13 frank Exp $
