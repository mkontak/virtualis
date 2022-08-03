// $Id: vxContour.h,v 1.2 2004/03/12 19:52:51 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Manjushree Nulkar (manju@viatronix.com)

#ifndef vxContour_h
#define vxContour_h


// include declarations
#include "Image.h"
#include "IncrementalDensityStatistics.h"
#include "BoundingRect.h"
#include "vxBlockVolume.h"


/**
 * Holds a mouse-drawn 2D contour
 */
class VX_VIEWER_DLL vxContour
{

// Member Functions.
public:
  
  /// Constructor.
  vxContour();
  
  /// Destructor.
  virtual ~vxContour();
  
  /// Calculate the pixels within the lasso..
  bool CalculateArea(const Triple<float4> & vVolumeUnits, Point<int2> & TopLeft, Image<bool> & boolMask);

  /// Loop over the lasso region.
  bool ScanPolygon(std::list< Point3Df > polygon, Triple<float4> vVolumeUnits, Point<int2> & TopLeft, Image<bool> & boolMask); 
                   
  // Calculate statistics of the region.
  bool CalculateStatistics(const Triple<float4> & vVolumeUnits, Point<int2> & TopLeft, Image<bool> & boolMask, vxBlockVolume<uint2> & origVol);

  /// Get the bounding box of the given polygon.
  bool EvalBoundaryBox(std::list< Point3Df > polygon, Point3Df * pTopLeftPoint, Point3Df * pBottomRightPoint);
  
  /// Get the bounding box of the current contour points.
  BoundingRect<float4> GetBoundingRectangle();
  
  /// Fill voxels inside the polygon.
  bool FillPolygon(std::list< Point3Df > polygon, Point3Df minP, Point3Df maxP, Image<bool> & boolImage );

  /// Fill voxels inside the polygon into a full image.
  bool FillPolygon(Image<bool> & boolImage);
  
  /// Move the selected handle point.
  bool MoveSelectedPoint( Point3Df newMeasureEnds);

  /// Retrieve the number of handle locations.
  virtual uint2 GetNoHandlePoints() const;
  
  /// Get the position of the handle point.
  virtual Point<float4> GetHandlePosition(uint2 uIndex) const;

  /// Recompute statistics after deserialization.
  virtual void Update(vxBlockVolume<uint2> & rIntensityVolume, vxBlockVolume<uint2> & rLabelVolume);

  /// Get the size of area in square mm.
  const float8 & GetArea() const;

  /// Get one point from the contour list
  const Point3Df & GetPoint( const int4 iIndex ) const;

  /// Set a contour point
  void SetPoint( const int4 iIndex, const Point3Df & newPosition );

  /// Add one point to the contour list
  void AddPoint( Point3Df pkt );

  /// the number of points in the contour
  int4 GetPointNumOfContour() const;

  /// Get the plane orientation.
  uint2 GetOrientation();

  /// Set the plane orientation.
  void SetOrientation(uint2 uOrientation);

  /// Get the statistics object conatining the various volume statistics.
  const IncrementalDensityStatistics & GetStats() const;

  /// Get teh histogram.
  std::vector<uint4> GetHistogram() const;

  /// Get number of voxels.
  int4 GetNumberOfVoxels() const;

  /// Set number of voxels covered by the lasso.
  void SetNoOfVoxels(int4 iNoVoxels);

  /// Get the collection of points that make up the contour.
  std::list< Point3Df > & GetContourPtVector() { return m_contourPoints; }


// Data Members
private:

  // The point list of the contour
  std::list< Point3Df > m_contourPoints;
  
  // Slice orientation.
  uint2 m_uOrientation;
  
}; // class vxContour


#endif // vxContour_h


// $Log: vxContour.h,v $
// Revision 1.2  2004/03/12 19:52:51  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.40  2004/01/19 20:23:58  frank
// removed some old typedefs
//
// Revision 1.39  2003/06/18 15:19:50  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.38  2003/05/16 14:56:29  dongqing
// code review
//
// Revision 1.37  2003/05/16 11:44:59  michael
// code review
//
// Revision 1.36  2002/12/23 22:51:19  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 1.35  2002/09/09 21:04:54  dmitry
// Cleaned up code and changed vectors to lists.
//
// Revision 1.34  2002/07/16 23:25:23  geconomos
// no message
//
// Revision 1.33  2002/07/10 14:11:19  geconomos
// exported classes.
//
// Revision 1.32  2002/06/20 15:43:17  manju
// Added method to retrieve the vector of contour points.
//
// Revision 1.31  2002/06/04 19:11:46  manju
// Coding standard
//
// Revision 1.30  2002/05/29 19:05:37  manju
// Coding standard.
//
// Revision 1.29  2002/05/10 15:12:15  manju
// Removed measurement status as argument to CalculateArea.
//
// Revision 1.28  2002/05/09 18:52:37  manju
// Changed labelvolume from int2 to uint2 in Update method.
//
// Revision 1.27  2002/05/07 18:27:31  frank
// Removed unneccessary includes.
//
// Revision 1.26  2002/05/06 14:00:50  manju
// Added method to get orientation of plane.
// Removed modality as an argument to CalculateStatistics method.
//
// Revision 1.25  2002/04/25 16:21:35  manju
// Implemented the GetNoHandlePoints method.
//
// Revision 1.24  2002/04/24 21:57:45  manju
// Using block volume in Update method.
//
// Revision 1.23  2002/04/23 14:54:39  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.22  2002/04/22 18:06:33  manju
// Changed Update method to take in reference of density volume.
//
// Revision 1.21  2002/04/20 18:29:13  michael
// cosmetics
//
// Revision 1.20  2002/04/18 23:05:45  frank
// Added function to fill in the polygon for the entire slice.
//
// Revision 1.19  2002/04/18 20:19:46  frank
// Added sculpting manipulator and renderer.
//
// Revision 1.18  2002/04/18 14:29:17  manju
// Added a method Update to compute statistics after deserialization
// of the annotation objects.
//
// Revision 1.17  2002/04/02 22:18:49  manju
// initializing the status flag to false.
//
// Revision 1.16  2002/03/27 22:35:03  manju
// Removed MoveHandlePoints method.
//
// Revision 1.15  2002/03/27 18:13:58  manju
// Added methods to get number of handle points and to get position of
// a handle point.
//
// Revision 1.14  2002/03/25 16:21:06  michael
// removed VolumeSetExt dependencies from annotations, only CardiacPlaque remains
//
// Revision 1.13  2002/03/24 16:36:05  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.12  2002/03/21 18:35:10  manju
// cleaned the #includes and added forward declarations where necessary
//
// Revision 1.11  2002/01/31 23:53:19  wenli
// Lasso TopLeft position Point<uint2> has been changed to Point<int2>.
//
// Revision 1.10  2002/01/07 16:11:03  manju
// Coding Standards.
//
// Revision 1.9  2002/01/04 17:55:03  wenli
// 2D lasso and 3D lasso are differentiated by a flag
//
// Revision 1.8  2002/01/03 16:24:45  manju
// Changed class names and added to a new project 51_annotation.
// MeasurementList -> HandlesSet
// Measurement -> Annotation
// Some function names have also been changed to be consistent.
//
// Revision 1.7  2001/12/11 15:56:38  manju
// Coding standards.
//
// Revision 1.6  2001/11/22 00:09:43  manju
// Taking into account CT and other types of modalities for statistics calculation.
//
// Revision 1.5  2001/11/17 00:02:14  manju
// Got back the statistics flag for lasso.
//
// Revision 1.4  2001/11/16 23:36:41  manju
// Added number of voxels.
//
// Revision 1.3  2001/11/16 18:18:19  manju
// Calculating statistics of the region.
//
// Revision 1.2  2001/11/14 19:21:08  wenli
// Area Measurement
//
// Revision 1.3  2001/01/13 07:55:48  manju
// Modified Volume Measurement.
//
// Revision 1.2  2001/10/31 22:40:43  manju
// Modified volume measurement.
//
// Revision 1.1  2001/10/30 00:33:59  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxContour.h,v 1.2 2004/03/12 19:52:51 frank Exp $
// $Id: vxContour.h,v 1.2 2004/03/12 19:52:51 frank Exp $
