// $Id: vxRibbon.h,v 1.3 2008/04/03 22:40:06 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes
#include <string>
//#include "vxVessel.h"
#include "vxBlockVolume.h"
#include "vxWinRect.h"
#include "Triple.h"
#include "Point3D.h"
#include "vxPlaneCurved.h"
#include "vxElement.h"
#include "vxCamera.h"
#include "vxBlockVolume.h"


class vxVessel;
class vxEnvironment;


/**
 * The Ribbon class handles all the data storage and projection operations
 * for the curved and luminal MPR rendering. The curved MPR is just like an
 * oblique MPR except that each scanline is at a different distance from the
 * viewer so that it can travel through a curved object and stay in the center.
 * The luminal MPR straightens out the curved so that the organ (e.g., vessel)
 * stays in the center of the screen the entire time.
 */
class VX_VIEWER_DLL vxRibbon : public vxElement
{
// Constants, Enuemrations, Typedefs, and Inner Classes
public:
 
  /**
   * A class to hold all the data for a single scanline.
   */
  class ScanlineData
  {
  public:

    /// Default constructor
    ScanlineData() : m_fSectionIndex( -1.0F ) {}

  public:

    /// The left point of the scanline in world space
    Point3Df m_leftPoint;

    /// The right point of the scanline in world space
    Point3Df m_rightPoint;

    /// The centerline point location along the scanline in world space
    Point3Df m_centerlinePoint;

    /// The radius in world space
    float4 m_fRadius;

    /// The vessel section corresponding to this scanline, < 0 indicates no section at all
    float4 m_fSectionIndex;

  }; // class ScanlineData

  // orientation mode for the MPR
  enum OrientationModeOfMPR 
  { 
    X_AXIS, 
    Y_AXIS, 
    Z_AXIS, 
    VESSEL, 

  };


// Member Functions
public:

  /// Compute all the scanlines for a projected curved MPR projection given all the necessary information.
  void ComputeScanlinesCurvedProjected(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);
  
  /// Compute all the scanlines for a curved MPR projection given all the necessary information.
  void ComputeScanlinesCurved(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Compute all the scanlines for a curved MPR projection given all the necessary information.used when orientation mode is XYZ
  void ComputeScanlinesCurved_XYZ(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Compute all the scanlines for a curved MPR projection given all the necessary information. used when the orientation mode is VESSEL
  void ComputeScanlinesCurved_VESSEL(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Compute all the scanlines for a sheared, curved MPR projection given all the necessary information.
  void ComputeScanlinesSheared(vxVessel & vessel, vxCamera & camera, vxPlaneCurved & planeCurved, vxBlockVolume<uint2> & intensityVolume, const Triple<uint4> & imageDim);
  
  /// Compute all the scanlines for a luminal MPR projection given all the necessary information.
  void ComputeScanlinesLuminal(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Compute all the scanlines for a luminal MPR projection given all the necessary information. ( used when orientation mode is XYZ )
  void ComputeScanlinesLuminal_XYZ(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Compute all the scanlines for a luminal MPR projection given all the necessary information ( used when orientation mode is VESSEL ) 
  void ComputeScanlinesLuminal_VESSEL(vxVessel & vessel, vxEnvironment & environment, const Triple<uint4> & imageDim);

  /// Return the endpoints of the scanlines
  const std::vector<ScanlineData> & GetScanlineData() const { return m_scanlineData; }
  
  /// Return half the width of the ribbon
  const float4 GetRibbonWidthHalf() { return m_widthHalf; }

  /// Get the reference camera (defines the up direction and right direction for zero degrees)
  vxCamera & GetRefCamera() { return m_refCamera; };

  /// Get the camera that was used to render the images (defines the current up and right direction)
  vxCamera & GetRenderCamera() { return m_renderCamera; };

  /// Get the projection type string
  const std::string & GetProjectionType() const { return m_sProjectionType; }

  /// Get the projection angle in degrees - taking into account the curved MPR rotation mode
  static float4 GetProjectionAngleDegrees( vxPlaneCurved * pCurvedPlane );

  /// Get the overall height of the image
  float4 GetHeightImageMM() const { return m_fHeightImageMM; }

  /// set the orientation mode for creating the MPR
  inline enum OrientationModeOfMPR GetOriModeOfMPR () { return m_oriModeOfMPR; } 

  /// set the orientation mode for creating the MPR
  inline void SetOriModeOfMPR (  enum OrientationModeOfMPR  mode ) { m_oriModeOfMPR = mode; } 

private:

  /// Decide whether the vessel should be flipped
  bool GetFlipNecessary( const vxVessel & vessel, const vxCamera & camera );

  /// Clamp the curved plane zoom to a reasonable size
  void ClampZoom( vxPlaneCurved * pPlaneCurved, vxBlockVolume<uint2> * pVolume );
 
  /// get the smoothed scanline direction for each vessel section
  Vector3D<float4> vxRibbon::GetSmoothScanLineDir( vxVessel & vessel, int4 iInterpolatedIndex, float4  fAngleRadians );

// Data Members
private:

  /// The data for all of the scanlines
  std::vector<ScanlineData> m_scanlineData;

  /// The reference camera (defines the up direction and right direction for zero degrees)
  vxCamera m_refCamera;

  /// The camera that was used to render the images (defines the current up and right direction)
  vxCamera m_renderCamera;

  /// Half of the width of the displayed ribbon
  float4 m_widthHalf;

  /// The type of projection that was performed
  std::string m_sProjectionType;

  /// The overall height of the image
  float4 m_fHeightImageMM;

  // orientation mode for the MPR
  enum OrientationModeOfMPR  m_oriModeOfMPR;
  
}; // class vxRibbon


// $Log: vxRibbon.h,v $
// Revision 1.3  2008/04/03 22:40:06  gdavidson
// Changed the OrientationModeOfMPR enumeration
//
// Revision 1.2  2007/10/18 19:17:42  cqian
// MPR is changed
//
// Revision 1.1  2006/08/17 14:02:36  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.3  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.1  2005/08/19 17:19:18  frank
// Issue #4377: Clamped minimum zoom
//
// Revision 1.2  2005/06/08 15:02:26  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/12/23 14:17:47  frank
// code review
//
// Revision 1.5  2003/11/24 16:33:01  frank
// Fixed the field of view display
//
// Revision 1.4  2003/11/18 15:53:40  wenli
// 3D Biconvex Slab MIP
//
// Revision 1.3  2003/10/29 20:56:16  frank
// Added rotating curved MPR preference and disablement
//
// Revision 1.2  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.15  2003/05/05 17:01:04  frank
// Exposed the projection type string accessor.
//
// Revision 1.14  2003/05/05 16:03:07  frank
// Added the projection type string for display on the upper right of the overlay
//
// Revision 1.13  2003/05/05 13:58:19  frank
// code review
//
// Revision 1.12  2003/04/09 14:23:00  frank
// Moved vascular preprocessing and intermediate results to a subdirectory
//
// Revision 1.11  2003/03/24 15:45:35  frank
// Fixed projection for endpoints
//
// Revision 1.10  2003/03/24 14:06:37  frank
// Fixed the orientation of the luminal view
//
// Revision 1.9  2003/03/21 17:09:55  frank
// Flipped the scanlines in the curved view so that up is always up
//
// Revision 1.8  2003/03/07 20:41:45  geconomos
// Fixed Ingmar's Point problems!
//
// Revision 1.7  2003/02/18 17:58:31  frank
// Added graph on curved and fixed it on luminal
//
// Revision 1.6  2003/01/28 23:17:27  frank
// Cleaned up, added a utility function.
//
// Revision 1.5  2003/01/24 20:16:29  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.4  2003/01/24 18:21:22  frank
// Added stretched curved
//
// Revision 1.3  2002/11/26 18:22:52  wenli
// zooming curved-MPR using '+' and '-'
//
// Revision 1.2  2002/11/18 16:55:40  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.4  2002/09/27 21:47:10  wenli
// add curved MPR camera
//
// Revision 1.3  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.2  2002/08/15 18:20:36  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/15 14:27:10  wenli
// initial version - Vascular
//
// Revision 1.6  2002/08/02 15:29:11  michael
// Completed CameraNew --> vxCamera
//
// Revision 1.5  2002/07/22 23:17:22  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-22-2002"
// into the main trunc
//
// Revision 1.4  2002/07/22 18:24:10  michael
// Merged vxBase of branch "XP_1-0" tagged as "XP_1-0_merge_7-15-2002"
// into the main trunc
//
// Revision 1.3.2.1  2002/07/04 05:00:14  frank
// Formatting.
//
// Revision 1.1  2002/06/21 19:03:20  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxRibbon.h,v 1.3 2008/04/03 22:40:06 gdavidson Exp $
// $Id: vxRibbon.h,v 1.3 2008/04/03 22:40:06 gdavidson Exp $=======
