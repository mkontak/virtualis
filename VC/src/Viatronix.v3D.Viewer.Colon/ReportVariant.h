//////////////////////////////////////////////////////////////////////
// $Id: ReportVariant.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: George Economos  geconomos@viatronix.net

#if !defined(AFX_REPORTVARIANT_H__7EE2A27F_4960_4DCC_A152_33E764179E62__INCLUDED_)
#define AFX_REPORTVARIANT_H__7EE2A27F_4960_4DCC_A152_33E764179E62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"
#include "Point.h"
#include "Normal.h"

namespace ColonReport
{
  /**
   * helper class for saving overview trackball during report??
   */
  class ReportVariant : public COleSafeArray  
  {
  public:
    /// constructor
    ReportVariant();
    /// constructor
    ReportVariant(const Quaternion<float4> & vQuaternion);
    /// constructor
    ReportVariant(const Matrix<float4> & mMatatrix);
    /// constructor
    ReportVariant(const Point<float4> & vPoint);
    /// constructor
    ReportVariant(const Normal<float4> & vNormal);
    /// constructor
    ReportVariant(const Vector<float4> & vVector);
    /// constructor
    ReportVariant(const Point<float4> & vPoint, const Normal<float4> & vOrientation, const Normal<float4> & vUp);
  	
    /// destructor
    virtual ~ReportVariant();

    /// type cast operator
    Quaternion<float4> asQuaternion();
    /// type cast operator
    Matrix<float4> asMatrix();
    /// type cast operator
    Normal<float4> asNormal();
    /// type cast operator
    Point<float4>  asPoint();
    /// type cast operator
    Vector<float4> asVector();
    /// type cast operator
    void asCameraCoordinates(Point<float4> & vPoint, Normal<float4> & vOrientation, Normal<float4> & vUp);
  }; // ReportVariant

} // namespace ColonReport

#endif // !defined(AFX_REPORTVARIANT_H__7EE2A27F_4960_4DCC_A152_33E764179E62__INCLUDED_)


// Revision History:
// $Log: ReportVariant.h,v $
// Revision 1.3  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 03:00:07  jmeade
// Camera coordinates to a variant stream.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:44   ingmar
// Initial revision.
// Revision 1.4  2001/04/13 02:34:23  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportVariant.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportVariant.h,v 1.3 2007/03/09 23:38:54 jmeade Exp $
