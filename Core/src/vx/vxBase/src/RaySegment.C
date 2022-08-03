// $Id: RaySegment.C,v 1.2 2004/03/23 21:14:46 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "RaySegment.h"



// Defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
RaySegment::RaySegment()
{

  m_bgnPoint = Vector4Df( 0, 0, 0, 0 );
  m_endPoint = Vector4Df( 1, 1, 1, 1 );

} // RaySegment()


/**
 * Constructor from two 4D vectors
 *
 * @param bgnPoint the starting point
 * @param endPoint the ending point
 */
RaySegment::RaySegment( const Vector4Df & bgnPoint, const Vector4Df & endPoint )
{
  m_bgnPoint = bgnPoint;
  m_endPoint = endPoint;

  if ( m_bgnPoint == m_endPoint )
  {
    LogWrn( "RaySegment created with identical begin and end points " + m_bgnPoint.ToString(), "RaySegment", "RaySegment()" );
    m_endPoint.Z() += 1;
  }
} // RaySegment( bgnPoint, endPoint )


/**
 * Constructor from two points
 *
 * @param bgnPoint the starting point
 * @param endPoint the ending point
 */
RaySegment::RaySegment( const Point3Df & bgnPoint, const Point3Df & endPoint )
{
  m_bgnPoint = Vector4Df( bgnPoint, 0 );
  m_endPoint = Vector4Df( endPoint, 1 );

  if ( bgnPoint == endPoint )
  {
    LogWrn( "RaySegment created with identical begin and end points " + bgnPoint.ToString(), "RaySegment", "RaySegment()" );
    m_endPoint.Z() += 1;
  }
} // RaySegment( bgnPoint, bgnPoint )


/**
 * Constructor from a point, direction, and length
 *
 * @param bgnPoint  the starting point
 * @param direction the direction
 * @param fLength   the length
 */
RaySegment::RaySegment( const Vector4Df & bgnPoint, const Vector4Df & direction, const float4 fLength )
{
  m_bgnPoint = bgnPoint;
  m_endPoint = bgnPoint + direction * fLength;

  if ( m_bgnPoint == m_endPoint )
  {
    LogWrn( "RaySegment created with zero length" + m_bgnPoint, "RaySegment", "constructor" );
    m_endPoint.Z() += 1;
  }
} // RaySegment( bgnPoint, direction, fLength )


/**
 * Output to a string.
 *
 * @return string representation of the data
 */
std::string RaySegment::ToString() const
{
  return "start " + m_bgnPoint.ToString() + "  end " + m_endPoint.ToString();
} // ToString()


#undef FILE_REVISION 


// $Log: RaySegment.C,v $
// Revision 1.2  2004/03/23 21:14:46  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/21 14:20:44  frank
// code review
//
// Revision 1.3  2003/02/27 22:33:03  frank
// Added a constructor from Point3D
//
// Revision 1.2  2003/01/22 21:14:07  ingmar
// the begin and end points are now represended as Vector4Df
// All interface functions have been adjusted accordingly
//
// Revision 1.1  2001/01/02 13:20:33  frank
// initial version
//
// Revision 1.1  2002/11/21 19:26:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/RaySegment.C,v 1.2 2004/03/23 21:14:46 frank Exp $
// $Id: RaySegment.C,v 1.2 2004/03/23 21:14:46 frank Exp $
