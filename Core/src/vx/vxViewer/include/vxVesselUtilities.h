// $Id: vxVesselUtilities.h,v 1.4 2006/09/25 13:20:03 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (gdavidson@viatronix.com)


#pragma once


#include "vxShareableObject.h"
#include "vxPlaneOblique.h"
#include "vxVesselSection.h"


// forward declarations
class vxComponentArray;
class vxVessel;
class vxVolumeHeader;
class vxRibbon;
class vxEnvironment;
class vxCamera;
class vxWinRect;
class ZeroOne;
template<class T> class Normal;
template<class T> class Point3D;


namespace vxVesselUtilities
{
  
  /// gets the current vessel (or null)
  VX_VIEWER_DLL vxVessel * GetCurrentVessel( vxComponentArray * pComponents );

  /// Get the reference center of the volume
  VX_VIEWER_DLL Point3Df GetVolReferencePoint( const vxVolumeHeader & volHeader );

  /// Return the associated section at this position on the screen.
  VX_VIEWER_DLL float4 GetVesselIndex( const int2 & iScreenY, vxRibbon * pRibbon, const int4 iViewportHeight );

  /// Return the screen y position given a section index
  VX_VIEWER_DLL float4 GetScanlinePosition( const float4 fSectionIndex, vxRibbon * pRibbon );

  /// Relocate the new oblique view
  VX_VIEWER_DLL void RelocateNewObliqueView( vxShareableObject< vxCamera > * pCamera, vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique,
                                             const Point3D<float4> & position, const Normal<float4> & planeNormal, const Normal<float4> & upDirection );

  /// Utility function to determine the current scanline given a screen position.
  VX_VIEWER_DLL bool GetScanline( float4 & fOutScanline, const float4 fScreenPosY, const vxWinRect & winRect, const uint4 uImageHeight );

  /// Orient the camera so that it lets the vessel rotate about any of four specific axes (x, y, z, v).
  VX_VIEWER_DLL bool OrientCameraToVessel( vxEnvironment & environment, const uint1 uKeyId );

    /// Compute an interpolated contour
  VX_VIEWER_DLL void GetInterpolatedContour( vxVesselSection::SectionContour & outContour, const vxVesselSection::SectionContour & contourA,
                               const vxVesselSection::SectionContour & contourB, const ZeroOne & fWeightA );



} // namespace vxVesselUtilities


#pragma region revision history

// $Log: vxVesselUtilities.h,v $
// Revision 1.4  2006/09/25 13:20:03  geconomos
// added "GetInterpolatedContour"
//
// Revision 1.3  2006/09/14 20:10:18  geconomos
// added OrientCameraToVessel
//
// Revision 1.2  2006/08/28 21:46:34  gdavidson
// Modified the parameters for RelocateNewObliqueView
//
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVesselUtilities.h,v 1.4 2006/09/25 13:20:03 geconomos Exp $
// $Id: vxVesselUtilities.h,v 1.4 2006/09/25 13:20:03 geconomos Exp $


#pragma endregion