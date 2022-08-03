// $Id: vxVessel.h,v 1.5 2006/12/14 22:00:41 romy Exp $
//
// Copyright (c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

#pragma once

// Includes
#include <vector>
#include <string>
#include "vxElement.h"
#include "Serialization.h"
#include "vxVesselSection.h"
#include "vxComponentData.h"


/**
 * Maintains a single vessel and all its details.  Essentially
 * a vector of sections and some tools to extract information from it.
 */
class VX_VIEWER_DLL vxVessel : public std::vector< vxVesselSection >, public vxElement, public vxComponentData
{
// typedefs, enumerations, and inner classes
public:

  /**
   * A class used to delineate the ends of a measurement or subsection of the vessel
   */
  class MeasurePair : public std::pair< float4, float4 >
  {
  // member functions
  public:

    /// constructor from a std::pair
    MeasurePair( std::pair< float4, float4 > & value ) { first = value.first; second = value.second; }

    /// constructor from two values
    MeasurePair( const float4 firstValue=0, const float4 secondValue=0 ) { first = firstValue; second = secondValue; }

    /// return the minimum value
    const float4 & GetMinimum() const { return Min( first, second ); }

    /// return the maximum value
    const float4 & GetMaximum() const { return Max( first, second ); }

  }; // class MeasurePair

// vxComponentData implementation
public:

  /// return the component data name
  inline const std::string GetName() const { return "vxVessel"; }

  /// if allowed to perform a grow operation
  inline virtual bool IsGrowSupported() { return false; }

  /// if allowed to perform a shrink operation
  inline virtual bool IsShrinkSupported() { return false; }

  /// perform a grow on the component data (will be called after the component grows)
  inline virtual void GrowComponentData() {}

  /// perform a shrink on the component data (will be called after the component shrinks)
  inline virtual void ShrinkComponentData() {}

// vxISerialize functions
public:

  /// generate Xml node
  virtual void ToXml( vxDOMElement & element ) const;

  /// initialize from Xml node
  virtual bool FromXml( vxDOMElement & element );

  /// name for Xml node
  virtual const std::string GetElementName() const { return GetName(); }

// member functions
public:

  //
  // Construction
  //

  /// constructor
  vxVessel();

  /// destructor
  virtual ~vxVessel();

  ///Clones an instance
  vxVessel * Clone() const;

  //
  // Input / Output
  //

  /// load a centerline from a text file
  bool LoadCenterlineFromTextFile( const std::string & sFileName );

  /// save a centerline to a text file
  bool SaveCenterlineToTextFile( const std::string & sFileName );

  /// retrieves the number of bytes of the serialized data.
  uint4 GetSerializedDataSize() const;

  /// reads in all the data from serialized form.
  void ReadSerializedData( int1 ** hiBuffer, const int1 * piBufferEnd );

  /// equality operator
  bool operator ==( const vxVessel & other ) const;

  /// inequality operator
  bool operator !=( const vxVessel & other ) const;

  /// assignment  operator
  vxVessel & operator=( const vxVessel & rhs );


  /// copy constructor
  vxVessel( const vxVessel & other );


  //
  // Get / Set Parameters
  //

  /// set the label of the vessel
  inline void SetLabel( const uint2 & uLabel ) { m_uLabel = uLabel; }

  /// set the label of the vessel
  inline const uint2 & GetLabel() const { return m_uLabel; }

  /// set the interval between cross-sections in mm ( < 0 indicates arbitrary intervals)
  inline void SetSectionInterval( const float4 fSectionIntervalMM ) { m_fSectionIntervalMM = fSectionIntervalMM;  }

  /// get the interval between cross-sections in mm ( < 0 indicates arbitrary intervals)
  inline const float4 GetSectionInterval() const { return m_fSectionIntervalMM;  }

  /// set the stenosis valid state
  inline void SetStenosisValid( const bool & bStenosisValid ) { m_bStenosisValid = bStenosisValid; }

  /// get the stenosis valid state
  inline const bool & GetStenosisValid() const { return m_bStenosisValid; }

  /// Returns a pair of indices which indicate the measurement ends
  const MeasurePair & GetMeasurementDomain() const { return m_uMeasurementPair; }

  /// Sets a pair of indices which indicate the measurement ends
  void SetMeasurementDomain( const MeasurePair & measurePair ) { m_uMeasurementPair = measurePair; }

  /// get the range of diameters of all the sections in mm.
  ValueRange<float4> GetDiameterRange( const MeasurePair & measurePair ) const;

  /// get the range of lumen areas of all the sections between the measure in square mm.
  ValueRange<float4> GetLumenAreaRange( const MeasurePair & measurePair ) const;

  /// get the range of wall areas of all the sections between the measure in square mm.
  ValueRange<float4> GetWallAreaRange( const MeasurePair & measurePair ) const;

  /// get the length from measurement start to end in mm.
  float4 GetLength( const MeasurePair & measurePair ) const;

  /// get the volume from measurement start to end in mm^3.
  float4 GetVolume( const MeasurePair & measurePair ) const;

  /// get the percent stenosis as determined by the start and end of measurement
  float4 GetPercentStenosis() const;

  /// get the section indices of the min and max diameters
  MeasurePair GetDiameterRangeIndices( const MeasurePair & measurePair ) const;

  /// get the section indices of the min and max areas
  MeasurePair GetAreaRangeIndices( const MeasurePair & measurePair ) const;

  /// Returns a pair of indices which indicates the entire vessel
  MeasurePair GetEntireDomain() { return MeasurePair( 0, size()-1 ); }

  /// resample the centerline position at a fraction section index
  Point3Df GetResampledPosition( const float4 fLocation ) const;

  /// get the resampling index at the location of specific length
  float4 GetFractionalIndex( const float4 fDesiredLengthMM ) const;

  /// resample the centerline tangent direction at a fraction section index
  Normal3Df GetResampledTangent( const float4 fLocation ) const;

  /// resample the centerline up direction at a fraction section index
  Normal3Df GetResampledUpDirection( const float4 fLocation ) const;

  /// set the calcification threshold of current lumen contour
  void SetCalcificationThresholdHU( const int4 iCalcificationThresholdHU ) { m_iCalcificationThresholdHU = iCalcificationThresholdHU; }

  /// get the calcification threshold of current lumen contour
  const int4 & GetCalcificationThresholdHU() const { return m_iCalcificationThresholdHU;  }

  //
  // Main Functions
  //

  /// recalculate the < min, max > diameter points from the contour.
  void RecalculateDiameterPoints();

  /// recalculate the area from the contour.
  void RecalculateArea();

  /// smooth orientations along the length so as to minimize distortion in luminal view
  void SmoothOrientations();
  
  /// computes the toruosity (in degrees/mm) at given location by looking ahead and behind <radius> millimeters.
  float4 ComputeTortuosity( const MeasurePair & calipers ) const;

  /// get the toruosity (in degrees/mm) at given location 
  float4 GetTortuosity( uint4 uLocation ) const;

  /// compute the maximum tortuosity and it's location
  float4 ComputeMaximumTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const;

  /// compute the maximum tortuosity and it's location
  float4 ComputeMinimumTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const;
  
  /// compute the maximum tortuosity and it's location
  float4 ComputeAverageTortuosity( uint4 & uLocation, const MeasurePair & measureDomain ) const;

  /// find the measurement pair surrounding this location with the given arc length
  MeasurePair GetEnclosingMeasurePair( const uint4 uLocation, const float4 fArcLengthMM ) const;

  /// compute the volume between sections along the vessel. This should be called when anytime the contour is changed.
  void RecalculateVolumes();

  /// compute the accumulated length between sections along the vessel. This should be called when anytime the centerline is changed.
  void RecalculateLengths();

  /// resample the lumen area value at a location along the vessel
  float4 ResampleLumenAreaSquareMM( const float4 fLocation ) const;

  /// resample the wall area value at a location along the vessel
  float4 ResampleWallAreaSquareMM( const float4 fLocation ) const;

  /// resample the accumulated length at a location along the vessel
  float4 ResampleAccumulatedLengthMM( const float4 fLocation ) const;

  /// resample the accumulated volume at a location along the vessel
  float4 ResampleAccumulatedVolumeCubicMM( const float4 fLocation ) const;

  /// resample the minimum and maximum diameters.
  ValueRange<float4> ResampleMinMaxDiameter( const float4 fLocation ) const;

  /// resample the tortuosity 
  float4 ResampleTortuosityDegree2MM( const float4 fLocation ) const;

  /// estimate the tangent vectors for the entire vessel from scratch
  void EstimateTangents( const float4 fArcLengthMM );

  //
  // Utility Functions
  //

  /// calculate the outside angle of 3 points.
  static float CalculateOutsideAngle( const Point3D<float4> & p0, const Point3D<float4> & p1, const Point3D<float4> & p2,
                                      const Vector3D<float4> & t0, const Vector3D<float4> & t2 );

  /// calculate the inside angle of 3 points.
  float4 CalculateInsideAngle( const Point3D<float4> & p0, const Point3D<float4> & p1, const Point3D<float4> & p2 );

private:

  /// compute the volume between two sections
  float4 ComputeInterSectionVolume( const uint4 uUpperSectionIndex ) const;


// Data Members
private:

  /// the < start, end > indices of measurement
  MeasurePair m_uMeasurementPair;

  /// whether the stenosis measurement has been manipulated by the user
  bool m_bStenosisValid;

  /// a delimiter for debugging I/O
  const static std::string m_sDelimiter;

  /// label of the vessel
  uint2 m_uLabel;

  /// interval (arc_length) to sample the vxCrossSecions
  float4 m_fSectionIntervalMM;

  /// calcification threshold in HU
  int4 m_iCalcificationThresholdHU;

  /// file version
  static const float4 m_fVersion;

}; // Class vxVessel


// $Log: vxVessel.h,v $
// Revision 1.5  2006/12/14 22:00:41  romy
// added Clone interface to Component Data
//
// Revision 1.4  2006/09/21 14:55:14  geconomos
// added copy constructor and assignment operator
//
// Revision 1.3  2005/06/07 17:59:18  vxconfig
// removed name
// moved serialization to class
//
// Revision 1.2  2004/03/23 21:13:03  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/01/22 14:32:22  wenli
// no message
//
// Revision 1.4  2003/08/20 21:19:14  dongqing
// add function GetFractionalIndex()
//
// Revision 1.3  2003/08/18 14:24:28  michael
// fixed debug crash: index out of bounce
//
// Revision 1.2  2003/08/13 11:26:54  frank
// Removed pointers to sections
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.6  2003/06/19 20:05:38  wenli
// Set up Calcification Threshold
//
// Revision 1.5  2003/06/10 14:22:35  wenli
// Get Tortousity from centerlinerefiner
//
// Revision 1.4  2003/06/09 14:30:46  frank
// Prevented stenosis text from dislaying unless the stenosis line is manipulated
//
// Revision 1.3  2003/05/28 17:45:36  frank
// Displayed the vessel wall area on the graph.
//
// Revision 1.2  2003/05/27 14:03:36  frank
// Made the cross-section area more specific to lumen
//
// Revision 1.1  2003/05/08 19:59:28  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.19  2003/04/11 18:37:20  frank
// Extended the vessel to permit non-uniform section intervals
//
// Revision 1.18  2003/04/08 15:16:45  frank
// Properly serialized the section interval
//
// Revision 1.17  2003/04/02 19:48:58  wenli
// fInterval is added
//
// Revision 1.16  2003/03/13 14:09:43  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.15  2003/03/12 14:46:25  frank
// Changed the length to accumulated length and volume to accumulated volume, added resampled for these values, and allowed fractional length measurement between endpoints.
//
// Revision 1.14  2003/02/28 21:43:49  frank
// Added volume output to text file.
//
// Revision 1.13  2003/02/27 18:46:46  frank
// Calculated the area using the contour points.
//
// Revision 1.12  2003/02/18 21:47:47  frank
// Added function to estimate the tangents along the vessel
//
// Revision 1.11  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.10  2003/02/05 23:20:59  wenli
// Load and Save vxVessel to TXT file
//
// Revision 1.9.2.1  2003/02/12 23:11:08  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.9  2003/01/14 16:16:41  dongqing
// Code review
//
// Revision 1.8  2002/11/27 16:54:09  frank
// Added resample functions to improve the quality of graphs.
//
// Revision 1.7  2002/11/22 03:34:20  frank
// Added angle utility functions
//
// Revision 1.6  2002/11/21 19:31:38  frank
// Revised tortuosity computation
//
// Revision 1.5  2002/11/20 17:09:38  frank
// Added volume computation.
//
// Revision 1.4  2002/11/19 20:14:33  frank
// Computed tortuosity
//
// Revision 1.3  2002/11/15 02:08:08  frank
// Added SmoothOrientations()
//
// Revision 1.2  2002/11/13 00:00:37  frank
// Added function to recalculate the min/max diameter.
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.19  2002/10/30 23:12:27  frank
// Fixed measurement indices
//
// Revision 1.18  2002/10/30 00:02:41  frank
// Renamed a variable
//
// Revision 1.17  2002/10/29 18:27:11  frank
// went from std::pair to MeasurePair
//
// Revision 1.16  2002/10/29 16:49:59  frank
// revamped measurement
//
// Revision 1.15  2002/10/25 16:35:41  frank
// Serialized/Deserialized the vessel length, name, version, and percent stenosis.
//
// Revision 1.14  2002/10/25 16:07:03  frank
// Got percent stenosis to be live.
//
// Revision 1.13  2002/10/25 02:29:43  frank
// Added GetPercentStenosis()
//
// Revision 1.12  2002/10/24 23:04:10  frank
// Added vessel name
//
// Revision 1.11  2002/09/20 22:18:26  frank
// Worked on reading/writing.
//
// Revision 1.10  2002/09/20 18:51:21  wenli
// v3D vessel component list
//
// Revision 1.9  2002/08/22 21:24:34  ana
// Vessel area range over the measured area or over the whole vessel.
//
// Revision 1.8  2002/08/22 03:33:28  frank
// Added length function.
//
// Revision 1.7  2002/08/22 02:59:20  frank
// Working on visualization.
//
// Revision 1.6  2002/08/16 21:06:56  ana
// Test function.
//
// Revision 1.5  2002/08/15 20:39:02  ana
// Read and Write from/to file implemented.
//
// Revision 1.4  2002/08/14 22:45:26  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/14 16:06:59  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/12 15:52:58  wenli
// initial version - Vascular
//
// Revision 1.1  2002/07/10 16:22:01  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxVessel.h,v 1.5 2006/12/14 22:00:41 romy Exp $
// $Id: vxVessel.h,v 1.5 2006/12/14 22:00:41 romy Exp $
