// $Id: Triangle.h,v 1.2 2006/02/17 15:09:12 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * define tringle
 */

#ifndef Triangle_h
#define Triangle_h


/**
 * Class to contain and operate on a triangle.
 */
template <typename T>
class Triangle
{
// member functions
public:

  /// Constructor
	inline Triangle( const T & p1, const T & p2, const T & p3 );

  /// Copy constructor
  inline Triangle( const Triangle<T> & other );

  /// Assignment operator
  inline Triangle & operator =( const Triangle<T> & other );

  /// Access the vertices;
  inline const T & GetVertex( const uint4 iIndex ) const;

  /// Compute the triangle area.
  inline float8 GetArea() const;
  
// data members
private:

  /// first vertex
  T m_v1;

  /// second vertex
  T m_v2;

  /// third vertex
  T m_v3;

}; // class Triangle


#include "Triangle.inl"


#endif // Triangle_h


// $Log: Triangle.h,v $
// Revision 1.2  2006/02/17 15:09:12  frank
// improved the accuracy of area estimates
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/21 13:11:36  dongqing
// code review
//
// Revision 3.7  2003/05/20 20:24:07  frank
// code review
//
// Revision 3.6  2003/05/16 11:44:21  michael
// code review
//
// Revision 3.5  2003/02/27 18:46:04  frank
// Changed from 2D area to full 3D area (still works for 2D) !!!
//
// Revision 3.4  2003/01/02 19:39:03  ingmar
// better documentation structure
//
// Revision 3.3  2002/02/25 23:12:14  geconomos
// Updated for vc7. See George for a list of effected files.
//
// Revision 3.2  2002/01/02 21:20:09  ingmar
// moved to Sec_12_mathGFX
//
// Revision 3.1  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
//    Rev 2.3   Oct 09 2001 18:34:36   liwei
// Put information such as frame rate, time to create the bounding  rectangle and bounding contour, the total area of all the slices and the total area covered by the rectangles and contours.
// 
//    Rev 2.2   Sep 19 2001 14:33:20   liwei
// No change. Checked out by error.
// 
//    Rev 2.1   Sep 18 2001 17:51:02   liwei
// Added ComputeSignedArea().
// 
//    Rev 2.0   Sep 16 2001 23:47:00   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:20   ingmar
// Initial revision.
// Revision 1.4  2001/09/14 22:10:52  liwei
// Code cleaned.
//
// Revision 1.3  2001/09/14 21:06:54  liwei
// The storage of points depends on the template class.
//
// Revision 1.2  2001/09/07 23:38:10  liwei
// Triangle now have templatized vertices.
//
// Revision 1.1  2001/09/07 16:07:26  liwei
// Added Triangle class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Triangle.h,v 1.2 2006/02/17 15:09:12 frank Exp $
// $Id: Triangle.h,v 1.2 2006/02/17 15:09:12 frank Exp $
