// $Id: vxVesselSection.h,v 1.3 2004/03/30 21:51:28 frank Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * A section of a tubular vessel object containing a local coordinate
 * system plus the vessel wall contour.
 */


#ifndef vxVesselSection_h
#define vxVesselSection_h


// Includes
#include <vector>
#include <utility>
#include "Point.h"
#include "Normal.h"
#include "RaySegment.h"


// Forward declarations
template <class T> class ValueRange;


class VX_VIEWER_DLL vxVesselSection
{
// Typedefs
public:

  /// A pair of point pairs.  Used to define two line segments to indicate min/max diameter, for example.
  typedef std::pair< RaySegment, RaySegment > RaySegmentPair;

  /// The section contour
  typedef std::vector< Point3Df > SectionContour;

// Member Functions
public:

  //
  // Construction, etc.
  //

  /// Constructor
  vxVesselSection();

  /// Equality operator
  bool operator ==(const vxVesselSection & other) const;

  /// Inequality operator
  bool operator !=(const vxVesselSection & other) const;

  /// Comparison operator
  bool operator <(const vxVesselSection & other ) const;

  //
  // Gets and Sets
  //

  /// Get the center position of the section in world coordinates
  inline const Point3Df & GetPosition() const { return m_position; }

  /// Set the center position of the section in world coordinates
  inline void SetPosition(const Point3Df & position) { m_position = position; }

  /// Get the up direction of the section in world coordinates
  inline const Normal3Df & GetUpDirection() const { return m_up; }

  /// Set the up direction of the section in world coordinates
  inline void SetUpDirection(const Normal3Df & upDirection) { m_up = upDirection; }

  /// Get the tangent direction of the section in world coordinates
  inline const Normal3Df & GetTanDirection() const { return m_tangent; }

  /// Set the tangent direction of the section in world coordinates
  inline void SetTanDirection(const Normal3Df & tanDirection) { m_tangent = tanDirection; }

  /// Get the lumen contour points of the section in world coordinates
  inline const SectionContour & GetLumenContour() const { return m_lumenContour; }

  /// Set the lumen contour points of the section in world coordinates
  void SetLumenContour(const SectionContour & contour) { m_lumenContour = contour; }

  /// Get the wall contour points of the section in world coordinates
  inline const SectionContour & GetWallContour() const { return m_wallContour; }

  /// Set the wall contour points of the section in world coordinates
  void SetWallContour(const SectionContour & contour) { m_wallContour = contour; }

  /// Get the lumen area of the section in square millimeters
  inline const float4 & GetLumenArea() const { return m_fLumenArea; }

  /// Set the area of the section in square millimeters
  inline void SetLumenArea(const float4 & fArea) { m_fLumenArea = fArea; }

  /// Get the wall area of the section in square millimeters
  inline const float4 & GetWallArea() const { return m_fWallArea; }

  /// Set the wall area of the section in square millimeters
  inline void SetWallArea( float4 fWallArea ) { m_fWallArea = fWallArea; }

  /// Get the minimum and maximum diameter of the section in millimeters
  ValueRange< float4 > GetDiameterRange() const;

  /// Get the < min, max > diameter points
  RaySegmentPair GetDiameterPoints() const;

  /// Set the < min, max > diameter points
  void SetDiameterPoints( const RaySegmentPair & pairs );

  /// Gets the mean diameter by derivation from the area
  float4 GetMeanDiameter() const;

  /// Gets the volume from the start of the vessel to this section in cubic mm
  const float4 & GetAccumulatedVolumeCubicMM() const { return m_fAccumulatedVolumeCubicMM; }

  /// Sets the volume from the start of the vessel to this section in cubic mm
  void SetAccumulatedVolumeCubicMM( const float4 fAccumulatedVolumeCubicMM ) { m_fAccumulatedVolumeCubicMM = fAccumulatedVolumeCubicMM; }

  /// Gets the length from the start of the vessel in mm
  const float4 & GetAccumulatedLengthMM() const { return m_fAccumulatedLengthMM; }

  /// Sets the length from the start of the vessel in mm
  void SetAccumulatedLengthMM( const float4 fAccumulatedLengthMM ) { m_fAccumulatedLengthMM = fAccumulatedLengthMM; }

  /// Get the curvature by degree / mm
  const float4 GetCurvatureDegree2MM() const { return m_fCurvatureDegree2MM; }
  
  /// Set the curvature by degree / mm
  void SetCurvatureDegree2MM( const float4 fCurvatureDegree2MM ) { m_fCurvatureDegree2MM = fCurvatureDegree2MM; }

  //
  // Main Functions
  //

  /// Recalculate the < min, max > diameter points from the contour.
  void RecalculateDiameterPoints();

  /// Recalculate the area based on the contour points.
  void RecalculateArea();

  //
  // I / O
  //

  /// Retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// Writes all the data in serialized form
  void WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd) const;

  /// Reads in all the data from serialized form.
  void ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// Retrieves the number of bytes of a contour
  static uint4 GetSerializedContourSize( const std::vector<Point3Df> & contour );

  /// Writes all the data of a contour
  static void WriteSerializedContour( int1 ** hiBuffer, const int1 * piBufferEnd, const std::vector<Point3Df> & contour );

  /// Reads in a contour from serialized form
  static std::vector<Point3Df> ReadSerializedContour( int1 ** hiBuffer, const int1 * piBufferEnd );

  //
  // Utility Functions
  //

  /// Calculate the mean diameter given an area.
  static float4 CalculateMeanDiameter( const float4 fArea );

  /// Calculate the percent stenosis given a normal and a stenotic measure
  static float4 CalculatePercentStenosis( const float4 fNormalMeasure, const float4 fStenoticMeasure );


private:

  /// Calculate the min diameter points of the contour.
  RaySegment CalculateMinDiameterPoints() const;

  /// Calculate the max diameter points of the contour.
  RaySegment CalculateMaxDiameterPoints() const;

  /// Returns the index of the other point which forms a segment that is most perpendicular to the given direction.
  uint4 GetMostPerpendicularSegmentIndex( const Normal3Df & majorDirection, const uint4 uFirstIndex ) const;


// Data Members
private:

  /// The center position of the section in world coordinates.
  Point3Df m_position;

  /// The plane normal of the section in world coordinates.
  Normal3Df m_tangent;

  /// The up direction of the section in world coordinates.
  Normal3Df m_up;

  /// The lumen contour points of the section in world coordinates.
  SectionContour m_lumenContour;

  /// The wall contour points of the section in world coordinates.
  SectionContour m_wallContour;

  /// The area of the lumen section in square millimeters.
  float4 m_fLumenArea;

  /// The area of the wall section in square millimeters.
  float4 m_fWallArea;

  /// The minimum diameter of the section in world coordinates.
  RaySegment m_diameterMin;

  /// The maximum diameter of the section in world coordinates.
  RaySegment m_diameterMax;

  /// The volume from the start of the vessel to this section in cubic mm.
  float4 m_fAccumulatedVolumeCubicMM;

  /// The length from the start of the vessel to this section in mm.
  float4 m_fAccumulatedLengthMM;

  /// The length from the start of the vessel to this section in mm.
  float4 m_fCurvatureDegree2MM;

  /// The file version
  static const float4 m_fVersion;

  /// A delimiter for debugging I/O
  const static std::string m_sSectionDelimiter;

  /// A delimiter for debugging contour I/O
  const static std::string m_sContourDelimiter;

}; // class vxVesselSection


#endif // vxVesselSection_h


// $Log: vxVesselSection.h,v $
// Revision 1.3  2004/03/30 21:51:28  frank
// addeds contour serialization
//
// Revision 1.2  2004/03/23 21:13:24  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/08/13 11:26:35  frank
// Added comparison operator
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.6  2003/06/10 14:17:24  wenli
// add GetCurvatureDegree2MM() and SetCurvatureDegree2MM()
//
// Revision 1.5  2003/05/28 17:45:36  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.4  2003/05/27 14:03:36  frank
// Made the cross-section area more specific to lumen
//
// Revision 1.3  2003/05/14 13:48:00  frank
// Added the wall contour
//
// Revision 1.2  2003/05/14 12:02:35  frank
// Adding a new contour to the vessel section
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.13  2003/04/11 18:37:38  frank
// Added a typedef
//
// Revision 1.12  2003/03/12 14:44:40  frank
// Changed the length to accumulated length and volume to accumulated volume
//
// Revision 1.11  2003/02/27 18:46:46  frank
// Calculated the area using the contour points.
//
// Revision 1.10  2003/02/18 21:48:21  frank
// Used Point3Df/Normal3Df typedefs
//
// Revision 1.9  2003/01/22 22:21:05  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.8  2003/01/16 21:27:13  frank
// code review
//
// Revision 1.7  2003/01/14 16:16:28  dongqing
// Code review
//
// Revision 1.6  2002/11/27 16:54:35  frank
// Added resample functions to improve the quality of graphs.
//
// Revision 1.5  2002/11/20 17:08:27  frank
// Added volume
//
// Revision 1.4  2002/11/15 02:07:21  frank
// Changed "normal" to "tangent" throughout
//
// Revision 1.3  2002/11/13 01:06:36  frank
// Implemented min/mean/max diameter estimation from the contour and the area, respectively.
//
// Revision 1.2  2002/11/12 21:15:30  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.1  2002/10/31 12:46:53  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.6  2002/10/30 00:02:18  frank
// Added SetDiamterPoints
//
// Revision 1.5  2002/10/29 16:49:59  frank
// revamped measurement
//
// Revision 1.4  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.3  2002/08/22 03:33:12  frank
// Removed warning.
//
// Revision 1.2  2002/08/16 21:06:55  ana
// Test function.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVesselSection.h,v 1.3 2004/03/30 21:51:28 frank Exp $
// $Id: vxVesselSection.h,v 1.3 2004/03/30 21:51:28 frank Exp $
