// $Id: BezierCurve.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BezierCurve.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


/** 
 * Constructor
 * @param point0
 * @param point1
 * @param point2
 * @param point3
 */
BezierCurve::BezierCurve(const Point2D<float4>& pt0, const Point2D<float4>& pt1, 
                         const Point2D<float4>& pt2, const Point2D<float4>& pt3)
//*************************************************************
{
  m_vCtrlPts.resize(4);
  m_vCtrlPts[0] = point0;
  m_vCtrlPts[1] = point1;
  m_vCtrlPts[2] = point2;
  m_vCtrlPts[3] = point3;
  m_vCurvePts.resize(GetDivisions());
  
  CreateArrayRepresentation();
} // constructor


/** 
 * Computes how many divisions are necessary to approximate the curve
 * @return number of divisions
 */
uint2 BezierCurve::GetDivisions() const
//*************************************************************
{
  uint2 iDivisions(200*(m_vCtrlPts[3].m_x - m_vCtrlPts[0].m_x));

  if (iDivisions < 4)
  {
    iDivisions = 4;
  }

  return iDivisions;
} // GetDivisions()


/** 
 * Fills up the vector with interpolated points.
 */
void BezierCurve::CreateArrayRepresentation()
//*************************************************************
{
  for (int4 k(0); k<m_vCurvePts.size(); ++k)
  {
    // t is running parameter from 0 to 1 (including 0 and 1)
    float4 t(float4(k)/(m_vCurvePts.size()-1)); float4 t2(t*t); float4 t3(t2*t);
    float4 s(1.0-t); float4 s2(s*s); float4 s3(s2*s);

    m_vCurvePts[k].m_x = m_vCtrlPts[0].m_x *     s3
                       + m_vCtrlPts[1].m_x * 3 * s2 * t
                       + m_vCtrlPts[2].m_x * 3 * s  * t2
                       + m_vCtrlPts[3].m_x *          t3;
    m_vCurvePts[k].m_y = m_vCtrlPts[0].m_y *     s3
                       + m_vCtrlPts[1].m_y * 3 * s2 * t
                       + m_vCtrlPts[2].m_y * 3 * s  * t2
                       + m_vCtrlPts[3].m_y *          t3;
  }
} // CreateArrayRepresentation()


/** 
 * Evaluate the Bezier curve at the index position (actually just lookup in the Array)
 * @param bin index
 * @return approximate 2D position of the curve
 */
Point2D<float4> BezierCurve::EvaluateAt(const uint2 index)
//*************************************************************
{
  return m_vCurvePts[index];
} // EvaluateAt


// Revision History:
// $Log: BezierCurve.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/15 11:52:13  michael
// code review
//
// Revision 1.4  2003/05/14 14:06:34  dongqing
// code review
//
// Revision 1.3  2002/03/11 20:01:14  michael
// compiles again
//
// Revision 1.2  2002/03/08 13:54:46  michael
// reworked 10_math and gained some compilation time
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/BezierCurve.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: BezierCurve.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
