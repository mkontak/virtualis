// $Id: MeasureArea.C,v 1.2 2006/01/31 14:44:38 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "MeasureArea.h"


/**
 * Constructor
 */
MeasureArea::MeasureArea() : m_iCurrentHandle(-1)
{
}


/**
 * Copy constructor.
 *
 * @param other   Source.
 */
MeasureArea::MeasureArea(const MeasureArea & other)
{
  *this = other;
}


/**
 * Copy operator.
 *
 * @param other            Source.
 * @return MeasureArea &   *this.
 */
MeasureArea & MeasureArea::operator =(const MeasureArea & other)
{
  m_centerDrawPt = other.m_centerDrawPt;
  m_majorAxisDrawVector = other.m_majorAxisDrawVector;
  m_minorAxisDrawVector = other.m_minorAxisDrawVector;
  m_iCurrentHandle = other.m_iCurrentHandle;
  return *this;
} //  operator ==


/**
 * Gets list of handle points (along circle).
 *
 * @return VectorPointFloat  Vector of handle points.
 */
MeasureArea::VectorPointFloat MeasureArea::GetDrawPoints() const
{
  VectorPointFloat vList;
  vList.push_back(m_centerDrawPt                                                );
  vList.push_back(m_centerDrawPt - m_majorAxisDrawVector - m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt                         - m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt + m_majorAxisDrawVector - m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt + m_majorAxisDrawVector                        );
  vList.push_back(m_centerDrawPt + m_majorAxisDrawVector + m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt                         + m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt - m_majorAxisDrawVector + m_minorAxisDrawVector);
  vList.push_back(m_centerDrawPt - m_majorAxisDrawVector                        );
  return vList;
} // GetDrawPoints


/**
 * Selects the nearest proximal line handle.
 *
 * @param positionPt   Position which to test.
 * @return bool        True/false whether a handle is proximal.
 */
bool MeasureArea::SelectHandle(const Point<float> & positionPt)
{
  m_iCurrentHandle = -1;
  VectorPointFloat vPoints = GetDrawPoints();
  for (uint4 i=0; i < vPoints.size(); i++)
  {
    const float4 fSelectionDistance = 10.0F;
    Vector<float4> diffVector(positionPt - vPoints[i]);
    if (diffVector.GetLength() <= fSelectionDistance)
    {
      m_iCurrentHandle = i;
    }
  }

  return (m_iCurrentHandle != -1);
} // SelectHandle


/**
 * Moves the current handle.
 *
 * @param newPositionPt   New point for the current handle.
 * @return bool           False if no handle is current, true otherwise.
 */
bool MeasureArea::MoveHandle(const Point<float> & newPositionPt)
{
  Vector<float> centerToMouseVector(newPositionPt.m_x - m_centerDrawPt.m_x, newPositionPt.m_y - m_centerDrawPt.m_y, 0);
  Vector<float> handleVector;
  bool bCorner = false;
  bool bMajorEdge = false;
  bool bMinorEdge = false;
  switch (m_iCurrentHandle)
  {
    case -1:
      return false;
      break;
    case 0:
      // moving the center handle
      m_centerDrawPt += centerToMouseVector;
      break;
    case 1:
      // lower left
      handleVector = Vector<float>(- m_majorAxisDrawVector - m_minorAxisDrawVector);
      bCorner = true;
      break;
    case 2:
      // bottom
      handleVector = -m_minorAxisDrawVector;
      bMinorEdge = true;
      break;
    case 3:
      // lower right
      handleVector = Vector<float>(+ m_majorAxisDrawVector - m_minorAxisDrawVector);
      bCorner = true;
      break;
    case 4:
      // right
      handleVector = m_majorAxisDrawVector;
      bMajorEdge = true;
      break;
    case 5:
      // upper right
      handleVector  = Vector<float>(+ m_majorAxisDrawVector + m_minorAxisDrawVector);
      bCorner = true;
      break;
    case 6:
      // top
      handleVector = m_minorAxisDrawVector;
      bMinorEdge = true;
      break;
    case 7:
      // upper left
      handleVector = Vector<float>(- m_majorAxisDrawVector + m_minorAxisDrawVector);
      bCorner = true;
      break;
    case 8:
      // left
      handleVector = -m_majorAxisDrawVector;
      bMajorEdge = true;
      break;
    default:
      return false;
      break;
  }

  // scale and rotate one of the corner handles
  if (bCorner || bMajorEdge || bMinorEdge)
  {
    // scale
    float4 fOriginalLength = handleVector.GetLength();
    float4 fNewLength = centerToMouseVector.GetLength();
    Matrix<float4> mScale, mRotate;
    mScale *= fNewLength / fOriginalLength;
    // rotate
    float4 fOriginalAngle = atan2(handleVector.m_y, handleVector.m_x);
    float4 fNewAngle = atan2(centerToMouseVector.m_y, centerToMouseVector.m_x);
    mRotate.Rotate(Normal<float>(0.0,0.0,-1.0), fNewAngle-fOriginalAngle);
    if (bCorner)
    {
      m_majorAxisDrawVector = mScale * mRotate * m_majorAxisDrawVector;
      m_minorAxisDrawVector = mScale * mRotate * m_minorAxisDrawVector;
    }
    if (bMajorEdge)
    {
      m_majorAxisDrawVector = mScale * mRotate * m_majorAxisDrawVector;
      m_minorAxisDrawVector =          mRotate * m_minorAxisDrawVector;
    }
    if (bMinorEdge)
    {
      m_majorAxisDrawVector =          mRotate * m_majorAxisDrawVector;
      m_minorAxisDrawVector = mScale * mRotate * m_minorAxisDrawVector;
    }
  }

  return true;
} // MoveHandle


// $Log: MeasureArea.C,v $
// Revision 1.2  2006/01/31 14:44:38  frank
// split up volume measure file
//
// Revision 1.1  2006/01/31 14:19:11  frank
// code review
//
// Revision 1.2  2005/11/19 12:05:08  vxconfig
// coding standards
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.4.2.2.4.1.6.1  2005/06/20 17:06:31  jmeade
// Issue 4197: Finally completing the port to Visual Studio 2k3.
//
// Revision 1.4.2.2.4.1  2005/03/16 17:12:50  frank
// Issue #3992: removed unused data fields
//
// Revision 1.4.2.2  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.4.2.1  2003/04/14 16:35:53  jmeade
// Issue 2841:  MeasureLine string operator; Comments, code standards.
//
// Revision 1.4  2003/01/22 23:06:37  ingmar
// added missing includes
//
// Revision 1.3  2002/12/24 03:46:54  jmeade
// Use std::list stream i/o.
//
// Revision 1.2  2002/09/12 18:02:41  jmeade
// Check-in from defunct/unused VC_1-2-3 branch:  Code standards.
//
// Revision 1.1.2.1  2002/07/03 17:42:50  jmeade
// Coding standards.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/MeasureArea.C,v 1.2 2006/01/31 14:44:38 frank Exp $
// $Id: MeasureArea.C,v 1.2 2006/01/31 14:44:38 frank Exp $
