// $Id: BezierCurve.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BezierCurve.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)

#ifndef BezierCurve_h
#define BezierCurve_h


// includes
#include "Point2D.h"
#include "Typedef.h"


// class definiton
class BezierCurve : Sec_math_1D
{
// functions
public:

  /// constructor
  inline BezierCurve(const Point2D<float4>& point0, const Point2D<float4>& point1, 
                     const Point2D<float4>& point2, const Point2D<float4>& point3);

  /// evaluate the Bezier curve at the index position (actually just lookup in the Array)
  inline Point2D<float4> EvaluateAt(const uint2 index);

  /// computes how many divisions are necessary to approximate the curve
  inline uint2 GetDivisions() const;

private: 

  /// copy constructor
  BezierCurve(const BezierCurve & other);                   // should only be public if really implemented!

  /// assignment operator
  const BezierCurve & operator=(const BezierCurve & other); // should only be public if really implemented!

  /// fills up the vector with interpolated points
  inline void CreateArrayRepresentation();

// data
private:

  /// four Bezier curve Control Points
  std::vector<Point2D<float4> > m_vCtrlPts;
  
  /// point interpilated along the Bezier curve 
  std::vector<Point2D<float4> > m_vCurvePts;
}; // BezierCurve


#include "BezierCurve.inl"


#endif // #ifndef BezierCurve_h


// Revision History:
// $Log: BezierCurve.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.4  2003/01/02 19:37:17  ingmar
// better documentation structure
//
// Revision 1.3  2001/01/02 18:51:33  ingmar
// better documentation structure
//
// Revision 1.2  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BezierCurve.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: BezierCurve.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
