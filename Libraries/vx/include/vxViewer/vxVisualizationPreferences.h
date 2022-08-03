// $Id: vxVisualizationPreferences.h,v 1.12 2007/06/29 20:20:23 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson (gdavidson@viatronix.com)


// pragmas
#pragma once


class VX_VIEWER_DLL vxVisualizationPreferences
{
// functions
public:

  // gets if the statistics should be shown
  static bool GetShowStatistics() { return m_bShowStatistics; }

  // gets if the statistics should be shown
  static void SetShowStatistics( bool bShowStatistics ) { m_bShowStatistics = bShowStatistics; }

  // gets if high quality is enabled
  static bool GetHighQualityEnabled() { return m_bHighQualityEnabled; }

  // sets if high quality is enabled
  static void SetHighQualityEnabled( bool bHighQualityEnabled ) { m_bHighQualityEnabled = bHighQualityEnabled; }

  // gets the maximum number of render threads
  static int4 GetMaxRenderThreads() { return m_iMaxRenderThreads; }

  // sets the maximum number of render threads
  static void SetMaxRenderThreads( int4 iMaxRenderThreads ) { m_iMaxRenderThreads = iMaxRenderThreads; }

	/// Gets the Show HU Indicator
	static bool GetShowHU() { return m_bShowHU; }

	/// Sets the Show HU Indicator
	static void SetShowHU( bool bShowHU ) { m_bShowHU = bShowHU; }

	/// Gets the Show Coordinates Indicator
	static bool GetShowCoordinates() { return m_bShowCoordinates; }

	/// Sets the Show Coordinates Indicator
	static void SetShowCoordinates( bool bShowCoordinates ) { m_bShowCoordinates = bShowCoordinates; }

  /// get the ability to rotate the curved MPR view
  static bool GetRotatingCurvedMPR() { return m_bRotateCurvedMPR; }

  /// set the ability to rotate the curved MPR view
  static void SetRotatingCurvedMPR( bool bRotateCurvedMPR ) { m_bRotateCurvedMPR = bRotateCurvedMPR; }

  // gets the minimum frame rate for 3D rendering
  static float4 GetFrameRateMin3D() { return m_fFrameRateMin3D; }

  // sets the minimum frame rate for 3D rendering
  static void SetFrameRateMin3D( float4 fFrameRate ) { m_fFrameRateMin3D = fFrameRate; }

  // gets the maximum frame rate for 3D rendering
  static float4 GetFrameRateMax3D() { return m_fFrameRateMax3D; }

  // sets the maximum frame rate for 3D rendering
  static void SetFrameRateMax3D( float4 fFrameRate ) { m_fFrameRateMax3D = fFrameRate; }

  // gets the minimum frame rate for oblique rendering
  static float4 GetFrameRateMinOblique() { return m_fFrameRateMinOblique; }

  // sets the minimum frame rate for oblique rendering
  static void SetFrameRateMinOblique( float4 fFrameRate ) { m_fFrameRateMinOblique = fFrameRate; }

  // gets the maximum frame rate for oblique rendering
  static float4 GetFrameRateMaxOblique() { return m_fFrameRateMaxOblique; }

  // sets the maximum frame rate for oblique rendering
  static void SetFrameRateMaxOblique( float4 fFrameRate ) { m_fFrameRateMaxOblique = fFrameRate; }

  // gets the minimum render size for 3D rendering
  static int4 GetRenderSizeMin3D() { return m_iRenderSizeMin3D; }

  // sets the minimum render size for 3D rendering
  static void SetRenderSizeMin3D( int4 iRenderSize ) { m_iRenderSizeMin3D = iRenderSize; }

  // gets the maximum render size for 3D rendering
  static int4 GetRenderSizeMax3D() { return m_iRenderSizeMax3D; }

  // sets the maximum render size for 3D rendering
  static void SetRenderSizeMax3D( int4 iRenderSize ) { m_iRenderSizeMax3D = iRenderSize; }

  // gets the minimum render size for oblique rendering
  static int4 GetRenderSizeMinOblique() { return m_iRenderSizeMinOblique; }

  // sets the minimum render size for oblique rendering
  static void SetRenderSizeMinOblique( int4 iRenderSize ) { m_iRenderSizeMinOblique = iRenderSize; }

  // gets the maximum render size for oblique rendering
  static int4 GetRenderSizeMaxOblique() { return m_iRenderSizeMaxOblique; }

  // sets the maximum render size for oblique rendering
  static void SetRenderSizeMaxOblique( int4 iRenderSize ) { m_iRenderSizeMaxOblique = iRenderSize; }

  // gets the initial render size for 3D rendering
  static int4 GetRenderSizeInitial3D() { return m_iRenderSizeInitial3D; }

  // sets the initial render size for 3D rendering
  static void SetRenderSizeInitial3D( int4 iRenderSize ) { m_iRenderSizeInitial3D = iRenderSize; }

  // gets the initial render size for oblique rendering
  static int4 GetRenderSizeInitialOblique() { return m_iRenderSizeInitialOblique; }

  // sets the initial render size for oblique rendering
  static void SetRenderSizeInitialOblique( int4 iRenderSize ) { m_iRenderSizeInitialOblique = iRenderSize; }

  // gets if the frame rate for 3D must be maintained
  static bool GetMaintainFrameRate3D() { return m_bMaintainFrameRate3D; }

  // sets if the frame rate for 3D must be maintained
  static void SetMaintainFrameRate3D( bool bMaintainFrameRate ) { m_bMaintainFrameRate3D = bMaintainFrameRate; }

  // gets if the frame rate for oblique must be maintained
  static bool GetMaintainFrameRateOblique() { return m_bMaintainFrameRateOblique; }

  // sets if the frame rate for oblique must be maintained
  static void SetMaintainFrameRateOblique( bool bMaintainFrameRate ) { m_bMaintainFrameRateOblique = bMaintainFrameRate; }

  // gets the tile size for 2D rendering
  static int4 GetTileSize2D() { return m_iTileSize2D; }

  // sets the tile size for 2D rendering
  static void SetTileSize2D( int4 iTileSize2D ) { m_iTileSize2D = iTileSize2D; }

  // gets the tile size for 3D rendering
  static int4 GetTileSize3D() { return m_iTileSize3D; }

  // sets the tile size for 3D rendering
  static void SetTileSize3D( int4 iTileSize3D ) { m_iTileSize3D = iTileSize3D; }

  // gets the tile size for oblique rendering
  static int4 GetTileSizeOblique() { return m_iTileSizeOblique; }

  // sets the tile size for oblique rendering
  static void SetTileSizeOblique( int4 iTileSizeOblique ) { m_iTileSizeOblique = iTileSizeOblique; }

private:

  // default destructor
  vxVisualizationPreferences() {}

// data members
private:
 
  /// is render manager statistics are displayed
  static bool m_bShowStatistics;

  /// is high quality enabled
  static bool m_bHighQualityEnabled;

  /// max render threads
  static int4 m_iMaxRenderThreads;

	///Flag holds the Show HU values
	static bool m_bShowHU;

	///Flag holds the Show coordinate value
	static bool m_bShowCoordinates;

  // should the curved MPR be allowed to rotate
  static bool m_bRotateCurvedMPR;

  // minimum 3D frame rate
  static float4 m_fFrameRateMin3D;

  // maximum 3D frame rate
  static float4 m_fFrameRateMax3D;

  // minimum oblique frame rate
  static float4 m_fFrameRateMinOblique;

  // maximum oblique frame rate
  static float4 m_fFrameRateMaxOblique;

  // minimum 3D render size
  static int4 m_iRenderSizeMin3D;

  // maximum 3D render size
  static int4 m_iRenderSizeMax3D;

  // minimum oblique render size
  static int4 m_iRenderSizeMinOblique;

  // maximum oblique render size
  static int4 m_iRenderSizeMaxOblique;

  // initial 3D render size
  static int4 m_iRenderSizeInitial3D;

  // initial oblique render size
  static int4 m_iRenderSizeInitialOblique;

  // maintain 3D frame rate
  static bool m_bMaintainFrameRate3D;

  // maintain oblique frame rate
  static bool m_bMaintainFrameRateOblique;

  // tile size in pixels for 2d
  static int4 m_iTileSize2D;

  // tile size in pixels for 3d
  static int4 m_iTileSize3D;

  // tile size in pixels for oblique
  static int4 m_iTileSizeOblique;

}; // vxVisualizationPreferences


// $Log: vxVisualizationPreferences.h,v $
// Revision 1.12  2007/06/29 20:20:23  geconomos
// added rendering tile size
//
// Revision 1.11  2007/01/19 15:28:49  geconomos
// reorganized visualization preferences
//
// Revision 1.10  2007/01/17 22:11:36  gdavidson
// Added Get and set for HighQualityEnabled
//
// Revision 1.9  2006/11/24 19:51:24  romy
// added shosHU flag
//
// Revision 1.8  2006/10/31 20:51:30  romy
// added show coordinate vizualization preset
//
// Revision 1.7  2006/09/20 15:04:25  frank
// added preference for displaying rendering manager statistics
//
// Revision 1.6  2006/08/23 17:28:56  geconomos
// corrected compiler error
//
// Revision 1.5  2006/08/23 17:26:48  geconomos
// added "DrawTimingStatistics"
//
// Revision 1.4  2006/08/23 15:53:27  gdavidson
// Added SliceShadowsAllEnabled
//
// Revision 1.3  2006/08/03 14:02:16  geconomos
// added "RenderingManagerEnabled" flag
//
// Revision 1.2  2006/07/19 18:09:11  gdavidson
// Changed the return and parameter types
//
// Revision 1.1  2006/07/19 15:41:54  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVisualizationPreferences.h,v 1.12 2007/06/29 20:20:23 geconomos Exp $
// $Id: vxVisualizationPreferences.h,v 1.12 2007/06/29 20:20:23 geconomos Exp $
