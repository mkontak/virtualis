// $Id: VCRenderUtils.h,v 1.5 2007/03/08 18:29:00 jmeade Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Joseph Addonisio (jaddonisio@viatronix.net)

/**
 * Class created just for VC so that it can still use a reference to the old volume
 * which is being removed from vxBase. Yes, this class is for the interim until we
 * completely remove the old volumes. 
 */


#if !defined(VCRenderUtils_h)
#define VCRenderUtils_h

// includes

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

#include "Point.h"
#include "Rect.h"
#include "Vector.h"
#include "Matrix.h"
#include "TransferFunctionVC.h"
#include "Volume.h"
#include "GLFontGeorge.h"
#include "MeasureLineList.h"

namespace ReaderLib
{

  class VCRenderUtils
  {
  // Member Functions
  public:

    /// Constructor.
	  VCRenderUtils();

    /// Destructor.
	  virtual ~VCRenderUtils();
    
    /// Draw the profile of the last measurement.
    static void RenderMeasurementProfile(MeasureLineList & measureList, const Rect<uint4> & vDrawArea,
                                        TransferFunctionVC & transferFunction, LinVolUC & volData,
                                        const Matrix<float4> & worldToVolumeMatrix);
  protected:
  private:

  // Data Members
  public:

    /// Segment colors. -- Nope, sorry Kev.  These pasty-bright colors only used in SliceViewerDBG :-) jrm
    static Triple<uint1> m_vTranslucentSliceViewColors[16];

  private:

  }; // VCRenderUtils

} // namespace ReaderLib

#endif // !defined(VCRenderUtils_h)


// Revision History:
// $Log: VCRenderUtils.h,v $
// Revision 1.5  2007/03/08 18:29:00  jmeade
// code standards.
//
// Revision 1.4  2006/01/31 14:55:08  frank
// split up volume measure file
//
// Revision 1.3  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.2  2005/08/16 21:58:20  jmeade
// namespaces for ReaderLib.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.3  2002/08/12 13:44:24  michael
// Took out include of vxTransferFunction and ../51annotations/... since they
// are not needed for compilation.
//
// Revision 1.2  2002/08/08 16:27:47  jmeade
// Like girls moving furniture: TransferFunction --> vxTransferFunction.
//
// Revision 1.1  2002/05/03 21:24:48  jaddonisio
// New files created for the interim while we start deleteing old volumes.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VCRenderUtils.h,v 1.5 2007/03/08 18:29:00 jmeade Exp $
// $Id: VCRenderUtils.h,v 1.5 2007/03/08 18:29:00 jmeade Exp $
