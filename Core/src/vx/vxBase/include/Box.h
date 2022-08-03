// $Id: Box.h,v 1.3 2006/08/09 17:14:44 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Wei Li (liwei@viatronix.com)

/**
 * define 3-D bounding box
 */

#ifndef Box_h
#define Box_h


// includes 
#include "BoundingRect.h"
#include "Enums.h"
#include "Point3D.h"
#include "Triple.h"
#include "Vector3D.h"


/**
 * A 3D bounding box
 */
template <class T>
class Box : public BoundingRect<T>
{
// member functions
public:

  /// default constructor
  inline Box();

  /// constructor
  inline Box( const T minX, const T minY, const T minZ, const T maxX, const T maxY, const T maxZ );
  
  /// copy constructor
  template <class C>
  inline Box( const Box<C> & other )
  {
    m_minX = (T) other.GetMinX();
    m_minY = (T) other.GetMinY();
    m_minZ = (T) other.GetMinZ();
    m_maxX = (T) other.GetMaxX();
    m_maxY = (T) other.GetMaxY();
    m_maxZ = (T) other.GetMaxZ();
  } // Box( other )
  
  /// Min/max point constructor
  inline Box( const Point3D<T> minPoint, const Point3D<T> maxPoint );
  
  /// constructor with a bounding rect
  inline Box( const BoundingRect<T> & boundingRect, const T minZ, const T maxZ, const nsORIENTATION::ORIENTATION eOrientation ); 

  /// set parameters
  inline void Set( const T minX, const T minY, const T minZ, const T maxX, const T maxY, const T maxZ );

  /// set the bounding rect
  inline void Set( const BoundingRect<T>& boundingRect, const nsORIENTATION::ORIENTATION eOrientation );
	
  /// Assignment operator
  void operator =( const Box<T> & other );

  /// operator ==
  bool operator ==( const Box<T> & other ) const;

  /// operator !=
  bool operator !=( const Box<T> & other ) const;

  /// Return the origin
  inline const Point3D<T> GetOrigin() const { return Point3D<T>( m_minX, m_minY, m_minZ ); }

  /// Return the dimension
  inline const Triple<T> GetDim() const { return Triple<T>( m_maxX - m_minX, m_maxY - m_minY, m_maxZ - m_minZ ); }

  /// get the minimum z value
  inline const T & GetMinZ() const { return m_minZ;}
  
  /// get the middle z value
  inline const T GetMidZ() const { return ( m_minZ + m_maxZ ) / 2; }

  /// get the maximum z value
  inline const T & GetMaxZ() const { return m_maxZ; }

  /// set the minimum z value
  inline void SetMinZ( const T z ) { m_minZ = z; }

  /// set the maximum z value
  inline void SetMaxZ( const T z ) { m_maxZ = z; }
  
  /// set the minimum point
  inline void SetMinPoint( const Point3D<T> other );

  /// set the maximum point
  inline void SetMaxPoint( const Point3D<T> other );

  /// get the z dimension
  inline const T GetDimZ() const { return m_maxZ - m_minZ; }

  /// get the minimum point
  inline Point3D<T> GetMinPoint() const { return Point3D<T>( m_minX, m_minY, m_minZ ); }

  /// get the middle point
  inline Point3D<T> GetMidPoint() const { return Point3D<T>( ( m_minX + m_maxX ) / 2, ( m_minY + m_maxY ) / 2, ( m_minZ + m_maxZ ) / 2); }
  
  /// get the maximum point
  inline Point3D<T> GetMaxPoint() const { return Point3D<T>( m_maxX, m_maxY, m_maxZ ); }
  
  /// set the dimension
  inline void SetDim( const Triple<T> & dim );

  /// get if the position is inside the box
  inline bool IsInside( const Triple<T> & position ) const;
	
  /// Expand the bounding box to include the given point.
  inline void Union( const Point3D<T> & pointToInclude );

  /// Expand the bounding box to include the given point.
  inline void Union( const Vector3D<T> & pointToInclude );

	///returns an Inflated Box by delta
	inline Box<T> Inflate( const T delta )const { return Box<T> ( m_minX - delta, m_minY - delta, m_minZ - delta, m_maxX + delta, m_maxY + delta, m_maxZ + delta );	}

	inline Box<T> Deflate( const T delta )const { return Box<T> ( m_minX + delta, m_minY + delta, m_minZ + delta, m_maxX - delta, m_maxY - delta, m_maxZ - delta );	}


  /// Bound this box with another box
  template <class C>
  void BoundWithBox( const Box<C> & otherBox )
  {
    Box<T> tmpBox(*this);
    tmpBox.SetMinX( Bound( (T)otherBox.GetMinX(), GetMinX(), (T)otherBox.GetMaxX()) );
    tmpBox.SetMinY( Bound( (T)otherBox.GetMinY(), GetMinY(), (T)otherBox.GetMaxY()) );
    tmpBox.SetMinZ( Bound( (T)otherBox.GetMinZ(), GetMinZ(), (T)otherBox.GetMaxZ()) );
    tmpBox.SetMaxX( Bound( (T)otherBox.GetMinX(), GetMaxX(), (T)otherBox.GetMaxX()) );
    tmpBox.SetMaxY( Bound( (T)otherBox.GetMinY(), GetMaxY(), (T)otherBox.GetMaxY()) );
    tmpBox.SetMaxZ( Bound( (T)otherBox.GetMinZ(), GetMaxZ(), (T)otherBox.GetMaxZ()) );
    //compensate for the change in sides if any
    if(tmpBox.GetMinX() > tmpBox.GetMaxX())
    {
      float4 fTmp(tmpBox.GetMinX());  tmpBox.SetMinX(tmpBox.GetMaxX()); tmpBox.SetMaxX(fTmp);
    }
    if(tmpBox.GetMinY() > tmpBox.GetMaxY())
    {
      float4 fTmp(tmpBox.GetMinY());  tmpBox.SetMinY(tmpBox.GetMaxY()); tmpBox.SetMaxY(fTmp);
    }
    if(tmpBox.GetMinZ() > tmpBox.GetMaxZ())
    {
      float4 fTmp(tmpBox.GetMinZ());  tmpBox.SetMinZ(tmpBox.GetMaxZ()); tmpBox.SetMaxZ(fTmp);
    }
    *this = tmpBox;
  } // BoundWithBox

  /// Scale the box
  inline void Scale( const T fScale );

  /// Scale the box anisotropically
  inline void ScaleAnIso( const Triple<float4> & fScaleVec );

  /// ensures that the min coordinates are <= compared to the max coordinates
  inline void Normalize();

  /// get the intersection between two boxes
  inline Box<T> GetIntersection( const Box<T> & other ) const;

  /// get the union between two boxes
  inline Box<T> GetUnion( const Box<T> & other ) const;

  /// if box intersects with other box
  inline bool IfOverlap( const Box<T> & other ) const;

  /// Return the radius of the bounding sphere
  inline T GetOuterRadius() const;

  /// Return the radius of the largest enclosed sphere
  inline T GetInnerRadius() const;

  /// Get i'th corner.
  inline Point3D<T> GetCorner( const uint2 & i ) const;

  /// Get i'th length.
  inline Triple<T> GetLength() const;

  /// Convert to a string representation
  inline std::string ToString() const;

// data members
protected:

  /// the minimim z value
  T m_minZ;
  
  /// the maximum z value
  T m_maxZ;

}; // class Box


#include "Box.inl"


#endif // Box_h


// $Log: Box.h,v $
// Revision 1.3  2006/08/09 17:14:44  romy
// added deflate method
//
// Revision 1.2  2006/08/03 20:00:30  romy
// Added inflate methods
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.28  2003/12/22 12:46:22  frank
// Removed redundant functions
//
// Revision 3.27  2003/05/20 16:13:50  dongqing
// code review
//
// Revision 3.26  2003/05/15 20:05:47  frank
// code review
//
// Revision 3.25  2003/01/22 21:11:59  ingmar
// ScaleAnIso() now uses a Triplef as argument
//
// Revision 3.24  2001/01/02 19:37:17  ingmar
// better documentation structure
//
// Revision 3.23  2002/11/20 21:17:52  michael
// made const methods const
//
// Revision 3.22  2002/11/15 23:04:45  ingmar
// added operators == and !=
//
// Revision 3.21  2002/11/14 20:46:28  ingmar
// added ToString()
//
// Revision 3.20  2002/10/14 15:33:05  michael
// maded GetInner/OuterRadius() const
//
// Revision 3.19  2002/09/25 20:58:37  ingmar
// implemented the constructor inside the classs declaration to make Intel 6.0 compiler happy
// added missing include
//
// Revision 3.18  2002/08/21 22:06:31  ingmar
// code beautify and
// allow to mix int and float boxes (i.e. for BoundWithBox)
//
// Revision 3.17  2002/08/09 20:53:08  uday
// Added BoundWithBox().
//
// Revision 3.16  2002/05/02 19:08:28  frank
// Added Vector3D interface.
//
// Revision 3.15  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.14  2002/03/22 19:44:14  uday
// Set Min/Max points
//
// Revision 3.13  2002/03/16 22:13:39  jaddonisio
// Copy constructor.
//
// Revision 3.12  2002/03/08 22:36:56  michael
// include cleanup
//
// Revision 3.11  2002/02/28 21:37:11  uday
// Inner and outer radius functions added
//
// Revision 3.10  2002/02/26 18:58:10  jaddonisio
// New functions:
//  i. GetCorner - gets i'th corner of box.
// ii. GetLength - gets length of sides.
// iii. GetMinCoord - gets coordinates of corner with min (x,y,z).
// iv. GetMaxCoord - gets coordinates of corner with max (x,y,z).
//
// Revision 3.9  2002/02/26 01:03:45  ingmar
// added return values to union
//
// Revision 3.8.2.1  2002/04/08 17:19:05  frank
// Added void return type on Union().
//
// Revision 3.8  2002/01/24 23:44:01  uday
// IfOverlap() added
// checks whether two boxes overlap
//
// Revision 3.7  2002/01/02 21:20:09  ingmar
// moved to Sec_12_mathGFX
//
// Revision 3.6  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.5  2001/11/13 19:35:04  geconomos
// Added Normalize()
//
// Revision 3.4  2001/11/06 01:11:50  frank
// Added Union()
//
// Revision 3.3  2001/11/03 19:35:56  frank
// Added GetMid functions.
//
// Revision 3.2  2001/11/01 16:44:03  frank
// Added extra constructors and output functions.
//
// Revision 3.1  2001/10/17 16:09:24  geconomos
// Fixed compile error in Set()
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 03 2001 14:35:14   frank
// Coding conventions.
// #includes are necessary for all referenced class objects.
// 
//    Rev 2.2   Sep 24 2001 17:15:08   liwei
// Added Scale() functions.
// 
//    Rev 2.1   Sep 19 2001 14:33:10   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:04   ingmar
// Initial revision.
// Revision 1.2  2001/09/05 20:33:10  liwei
// Added Inside().
//
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.7  2001/08/22 23:31:12  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.6  2001/08/22 00:15:19  liwei
// Added Set() member function.
//
// Revision 1.5  2001/08/15 23:49:24  liwei
// Added couple of access function.
//
// Revision 1.4  2001/08/14 21:00:23  liwei
// Comitted for huamin to compile.
//
// Revision 1.3  2001/08/09 21:13:13  liwei
// Directly store min , max values in BoundingRect and Box, without using
// Triple and Point. The constructors are modified accordingly.
//
// Revision 1.2  2001/08/08 17:24:33  liwei
// GetMax(Min)X(Y) should return by value rather than by reference.
//
// Revision 1.1  2001/08/06 23:19:10  liwei
// Added Box<T> class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Box.h,v 1.3 2006/08/09 17:14:44 romy Exp $
// $Id: Box.h,v 1.3 2006/08/09 17:14:44 romy Exp $
