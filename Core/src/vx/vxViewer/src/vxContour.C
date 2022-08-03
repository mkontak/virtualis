// $Id: vxContour.C,v 1.2 2004/03/12 19:49:15 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Manjushree Nulkar [manju@viatronix.com]
//
// Complete history at bottom of file.



// pragma declarations
#pragma warning (disable : 4786)



// include declarations
#include "stdafx.h"
#include "vxContour.h"
#include "vxBlockVolumeIterator.h"
#include "vxIntensityConverter.h"


// define declarations
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor.
 */
vxContour::vxContour()
: m_uOrientation( 2 )
{
} // vxContour()


/**
 * Destructor
 */
vxContour::~vxContour()
{
} // ~vxContour()


/**
 * Retrieve the point
 * @param iIndex index
 * @return Point3Df m_contourPoints[index].
 */
const Point3Df & vxContour::GetPoint( const int4 iIndex ) const
{
	
  if ( iIndex < 0 || iIndex > m_contourPoints.size() - 1 )
    throw ex::IndexOutOfBoundsException( LogRec( "Tried to access a point out of bounds", "vxContour", "GetPoint" ) );

  std::list< Point3Df >::const_iterator iter = m_contourPoints.begin();
  std::advance( iter, iIndex );

	return * iter;

} // GetPoint()

  
/**
 * Set a contour point
 * @param iIndex index
 * @param newPosition the new position
 * @return Point3Df m_contourPoints[index].
 */
void vxContour::SetPoint( const int4 iIndex, const Point3Df & newPosition )
{

  if ( iIndex < 0 || iIndex > m_contourPoints.size() - 1 )
    throw ex::IndexOutOfBoundsException( LogRec( "Tried to access a point out of bounds", "vxContour", "SetPoint" ) );

  std::list< Point3Df >::iterator iter = m_contourPoints.begin();
  std::advance( iter, iIndex );

  * iter = newPosition;

} // SetPoint()


/**
 * Add a point to the list.
 * @param pkt point to be added.
 * @return void.
 */
void vxContour::AddPoint( Point3Df pkt )
{
	m_contourPoints.push_back( pkt );
} // AddPoint()


/**
 * Get number of contour points.
 * @param void
 * @return int4 size of the contour points list.
 */
int4 vxContour::GetPointNumOfContour() const
{
	return m_contourPoints.size();
} // GetPointNumOfContour()


/**
 * Get number of handle points.
 * @param void.
 * @return number of points.
 */
uint2 vxContour::GetNoHandlePoints() const
{
  return m_contourPoints.size();
} // GetNoHandlePoints()


/**
 * Calculate AreaContour annotation.
 * @param vVolumeUnits volume units.
 * @param TopLeft top left position.
 * @param boolMask image.
 * @return bool.
 */
bool vxContour::CalculateArea(const Triple<float4> & vVolumeUnits, Point<int2> & TopLeft, Image<bool>  & boolMask)
{
  bool bResult(true);
  //do the scan conversion of the polygon in m_contourPoints
  //ScanPolygon( m_contourPoints,  tagVol, vVolumeUnits );
  bResult = ScanPolygon( m_contourPoints, vVolumeUnits, TopLeft, boolMask);
  if ( bResult == false )
  {
     boolMask.Resize( Triple<uint4> (1, 1, 0) );
     LogWrn( "unabled to scan polygon", "vxContour", "CalculateArea" );
  }
  return bResult;
} // CalculateArea()


/**
 * Scan polygon
 * @param polygon list of contour points.
 * @param vVolumeUnits volume units.
 * @param TopLeft top left position.
 * @param boolMask image.
 * @return bool.
 */
bool vxContour::ScanPolygon( std::list< Point3Df > polygon, Triple<float4> vVolumeUnits, Point<int2> & TopLeft, Image<bool> & boolMask )
{
	Point3Df pkt1, pkt2, topLeftPkt, bottomRightPkt;
  
  // ensure there is enough data to process
	if ( polygon.size() < 4 )
    return false;
  
  // ensure that the last point is duplicated
  if ( polygon.front() != polygon.back() )
  {
    polygon.push_back( polygon.front() );
  }
  if ( polygon.front() != polygon.back() )
    return false;
	
	std::list< Point3Df > volPolygon; 
  std::list< Point3Df >::iterator iter = polygon.begin();
	switch (m_uOrientation)
	{
	case 0: // X Axis
      for ( ; iter != polygon.end(); ++ iter )
      {
        pkt1 = * iter;
        pkt2.m_x = pkt1.m_z / vVolumeUnits.m_z;
        pkt2.m_y = pkt1.m_y / vVolumeUnits.m_y;
        pkt2.m_z = pkt1.m_x / vVolumeUnits.m_x;
        volPolygon.push_back( pkt2 );
      }
      break;

	case 1: //Y Axis
      for ( ; iter != polygon.end(); ++ iter )
      {
        pkt1 = * iter;
        pkt2.m_x = pkt1.m_x / vVolumeUnits.m_x;
        pkt2.m_y = pkt1.m_z / vVolumeUnits.m_z;
        pkt2.m_z = pkt1.m_y / vVolumeUnits.m_y;
        volPolygon.push_back( pkt2 );
      }
      break;

	case 2: // Z Axis
      for ( ; iter != polygon.end(); ++ iter )
      {
        pkt1 = * iter;
        pkt2.m_x = pkt1.m_x / vVolumeUnits.m_x;
        pkt2.m_y = pkt1.m_y / vVolumeUnits.m_y;
        pkt2.m_z = pkt1.m_z / vVolumeUnits.m_z;
        volPolygon.push_back( pkt2 );
      }
      break;

	case -1: // screen -- do not convert them
      for ( ; iter != polygon.end(); ++ iter )
      {
        pkt1 = * iter;
        volPolygon.push_back( pkt1 );
      }
      break;
		
	default: // should not happen
		return false;
	}

	EvalBoundaryBox( volPolygon, &topLeftPkt, &bottomRightPkt );

	int4 iBufSizeX = (int4)(bottomRightPkt.m_x) - (int4)(topLeftPkt.m_x) + 2;
	int4 iBufSizeY = (int4)(bottomRightPkt.m_y) - (int4)(topLeftPkt.m_y) + 2;
	boolMask.Resize(Triple<uint4>(iBufSizeX, iBufSizeY, 0));

	FillPolygon( volPolygon, topLeftPkt, bottomRightPkt, boolMask );
	
	pkt1 = volPolygon.front();
	
	TopLeft.m_x = (int2)(topLeftPkt.m_x);
	TopLeft.m_y = (int2)(topLeftPkt.m_y);
	TopLeft.m_z = (int2)(pkt1.m_z+0.5);
	
	return true;
} // ScanPolygon()


/**
 * Get bounding box.
 * @param polygon list of contour points.
 * @param topLeft top left point.
 * @param btmRight bottom right point.
 * @return bool.
 */
bool vxContour::EvalBoundaryBox( std::list< Point3Df > polygon, Point3Df * pTopLeftPoint, Point3Df * pBottomRightPoint ) 
{
	if ( polygon.size() < 4 ) return false;

	Point3Df maxPkt = polygon.front();
	Point3Df minPkt = polygon.front();
  std::list< Point3Df >::iterator iter = polygon.begin();
  for ( ++ iter; iter != polygon.end(); ++ iter )
  {
		Point3Df pkt ( * iter );

		if ( pkt.m_x > maxPkt.m_x ) maxPkt.m_x = pkt.m_x;
		if ( pkt.m_x < minPkt.m_x ) minPkt.m_x = pkt.m_x;

		if ( pkt.m_y > maxPkt.m_y ) maxPkt.m_y = pkt.m_y;
		if ( pkt.m_y < minPkt.m_y ) minPkt.m_y = pkt.m_y;
	}

	*pTopLeftPoint = minPkt;
	*pBottomRightPoint = maxPkt;

  return true;

} // EvalBoundaryBox()


/**
 * Get the bounding box of the current contour points.
 * @param void.
 * @return bounding rectangle.
 */
BoundingRect<float4> vxContour::GetBoundingRectangle()
{
	if ( m_contourPoints.empty() )
  {
    throw ex::VException(LogRec("Not enough points in the boundary to calculate a bounding box", "vxContour", "GetBoundingRectangle"));
  }

	Point3Df fMaxPoint = m_contourPoints.front();
	Point3Df fMinPoint = m_contourPoints.front();
  for ( std::list< Point3Df >::iterator iter = m_contourPoints.begin(); iter != m_contourPoints.end(); ++iter )
  {
    Point3Df fContourPoint( * iter );

    if ( fContourPoint.m_x > fMaxPoint.m_x ) fMaxPoint.m_x = fContourPoint.m_x;
		if ( fContourPoint.m_y > fMaxPoint.m_y ) fMaxPoint.m_y = fContourPoint.m_y;

		if ( fContourPoint.m_x < fMinPoint.m_x ) fMinPoint.m_x = fContourPoint.m_x;
		if ( fContourPoint.m_y < fMinPoint.m_y ) fMinPoint.m_y = fContourPoint.m_y;
  }

	return BoundingRect<float4>(fMinPoint.m_x, fMinPoint.m_y, fMaxPoint.m_x, fMaxPoint.m_y);
} // GetBoundingRectangle()


/**
 * FillPolygon
 * @param polygon list of contour points.
 * @param minP
 * @param maxP
 * @param boolImage mask indicating if the point is inside or outside the contour.
 * @return bool.
 */
bool vxContour::FillPolygon( std::list< Point3Df > polygon, Point3Df minP, Point3Df maxP, Image<bool> & boolImage )
{
  // validate status
  if ( polygon.size() < 4 || ( polygon.front() != polygon.back() ) ) return false;

  boolImage.SetClearColor( false );
  boolImage.Clear();

	int4 iBufX = (int4)(maxP.m_x) - (int4)(minP.m_x) + 2;
	int4 iBufY = (int4)(maxP.m_y) - (int4)(minP.m_y) + 2;
		
	// to fill the voxels inside a polygon
	// it is voxel accurate -- using integer
	std::list< Point3Df >::iterator iter = polygon.begin();
  Point<int4> pointA ( (* iter).X() - minP.X(), (* iter).Y() - minP.Y(), 0); 
  for ( ++ iter; iter != polygon.end(); ++ iter )
  {
		Point<int4> pointB ( (* iter).X() - minP.X(), (* iter).Y() - minP.Y(), 0);
		Point<int4> delta ( pointB - pointA );

    // omit the horizontal line
		if ( delta.Y() != 0 ) 
    {
			delta.m_x = pointB.m_x - pointA.m_x;
			float8 fStep = (float8) delta.X() / delta.Y();
	    
      int4 iYMin, iYMax;
      float8 fXLeft;

      // if B is above A 
			if ( delta.Y() > 0 )
			{
				fXLeft = pointA.m_x;
				iYMin = pointA.m_y;
				iYMax = pointB.m_y;
			}
			else
			{
				fXLeft = pointB.m_x;
				iYMin = pointB.m_y;
				iYMax = pointA.m_y;
			}
			
      // fill in polygon
			for ( int y = iYMin + 1; y < iYMax + 1; ++ y )
			{
				fXLeft += fStep;
				for ( int x = (int4) fXLeft + 1; x < iBufX; ++ x )
				{
	        Point2D<uint2> pkt ( x, y);
					bool flag = boolImage.GetPixel( pkt );
					boolImage.SetPixel( pkt, !flag );
				}		
			}
		} // if not horizontal
		pointA = pointB;		
	} // iterate through polygon

	return true;
} // FillPolygon()


/**
 * Fill voxels inside the polygon into a full image.
 * @param boolImage mask
 * @return bool
 */
bool vxContour::FillPolygon( Image<bool> & boolImage )
{
  Point<float4> minPoint(0.0F, 0.0F, 0.0F);
  Point<float4> maxPoint(boolImage.GetDim().m_x-2, boolImage.GetDim().m_y-2, 0.0F);
  return FillPolygon( m_contourPoints, minPoint, maxPoint, boolImage );
} // FillPolygon()

  
/**
 * Move selcted point.
 * @param newMeasureEnds point to move to  in world coordinates.
 * @return bool.
 */
bool vxContour::MoveSelectedPoint( Point3Df newMeasureEnds )
{
  return true;
} //MoveSelectedPoints()


/**
 * Retrieve the position of a handle point.
 * @param uIndex index of handle point.
 * @return position.
 */
Point<float4> vxContour::GetHandlePosition(uint2 uIndex) const
{
  std::list< Point3Df >::const_iterator iter = m_contourPoints.begin();
  std::advance( iter, uIndex );
  return * iter;
}//GetHandlePosition().


/**
 * Compute statistics after deserialization of annotations.
 * @param origVol density volume.
 * @param labelVol label volume.
 * @return void.
 */
void vxContour::Update(vxBlockVolume<uint2> & rIntensityVolume, vxBlockVolume<uint2> & rLabelVolume)
{
  Point<int2> iMaskTopLeft; 
  Image<bool> bMask(false);
  const Vector3D<float4> units(rIntensityVolume.GetHeader().GetVoxelUnits());
  const Triple<float4> volUnits(units.X(), units.Y(), units.Z());
  CalculateStatistics(volUnits, iMaskTopLeft, bMask, rIntensityVolume);
} // Update()


/**
 * Calculate avg, max and standard deviation HU of the region.
 * @param vVolumeUnits volume units
 * @param TopLeft 
 * @param boolImage 
 * @param origVol the actual volume
 * @return bool.
 */
bool vxContour::CalculateStatistics(const Triple<float4> & vVolumeUnits, Point<int2> & TopLeft, Image<bool> & boolMask, vxBlockVolume<uint2> & origVol) 
{
  // the last point is duplicated
  if ( m_contourPoints.size() < 4 || ( m_contourPoints.front() != m_contourPoints.back() ) ) return false;
	
	std::list< Point3Df > volPolygon; 
  for( std::list< Point3Df >::iterator iter = m_contourPoints.begin(); iter != m_contourPoints.end(); ++ iter )
	{
    Point3Df point1, point2;
    point1 = * iter;

    switch (m_uOrientation)
    {
    case 0: // X Axis
      point2.m_x = point1.m_y / vVolumeUnits.m_y;
      point2.m_y = point1.m_z / vVolumeUnits.m_z;
      point2.m_z = point1.m_x / vVolumeUnits.m_x;
      break;

    case 1: //Y Axis
      point2.m_x = point1.m_z / vVolumeUnits.m_z;
      point2.m_y = point1.m_x / vVolumeUnits.m_x;
      point2.m_z = point1.m_y / vVolumeUnits.m_y;
      break;

    case 2: // Z Axis
      point2.m_x = point1.m_x / vVolumeUnits.m_x;
      point2.m_y = point1.m_y / vVolumeUnits.m_y;
      point2.m_z = point1.m_z / vVolumeUnits.m_z;
      break;
    }

    volPolygon.push_back( point2 );
	}

  Point3Df topLeftPkt, bottomRightPkt;
	EvalBoundaryBox( volPolygon, &topLeftPkt, &bottomRightPkt );
	
	int4 iBufSizeX = (int4)( bottomRightPkt.m_x - topLeftPkt.m_x + 2 );
	int4 iBufSizeY = (int4)( bottomRightPkt.m_y - topLeftPkt.m_y + 2 );
	boolMask.Resize( Triple<uint4>( iBufSizeX, iBufSizeY, 0 ) );

	FillPolygon( volPolygon, topLeftPkt, bottomRightPkt, boolMask );
	
	TopLeft.m_x = (int2) topLeftPkt.m_y;
	TopLeft.m_y = (int2) topLeftPkt.m_x;
	TopLeft.m_z = (int2)( volPolygon.front().m_z + 0.5 );
	
  Point<int4> volCoord, maskPoint; 
	maskPoint.m_z = TopLeft.m_z;
  
  const uint4  uBins( 30 );
  const float4 fMinimum( 0 );
  const float4 fMaximum( 1000 );

  return true;
	
} // CalculateStatistics()


/**
 * Set the plane orientation.
 * @param uOrientation 0, 1 or 2 for sagittal, coronal and axial.
 */
void vxContour::SetOrientation(uint2 uOrientation)
{
  m_uOrientation = uOrientation;
} // SetOrientation()


/**
 * Get the orientation of the plane
 * @return axial, sagittal or coronal.
 */
uint2 vxContour::GetOrientation()
{
  return m_uOrientation;
} // GetOrientation()



#undef FILE_REVISION


// $Log: vxContour.C,v $
// Revision 1.2  2004/03/12 19:49:15  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.54  2004/01/19 20:23:58  frank
// removed some old typedefs
//
// Revision 1.53  2003/06/18 15:19:50  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.52  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.51  2003/05/27 13:18:05  frank
// Removed dead includes
//
// Revision 1.50  2003/05/16 11:44:59  michael
// code review
//
// Revision 1.49  2003/05/13 17:42:42  frank
// Issue #3215: Contour broken in some slices
//
// Revision 1.48  2003/04/18 15:25:41  frank
// Removed HUconverter
//
// Revision 1.47  2003/01/22 21:39:22  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.46  2002/12/23 22:51:19  ingmar
// removed redundant use of Triple copy constructor
//
// Revision 1.45  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 1.44  2002/09/24 19:16:21  dmitry
// removed logs
//
// Revision 1.43  2002/09/09 21:04:54  dmitry
// Cleaned up code and changed vectors to lists.
//
// Revision 1.42  2002/07/17 20:05:37  ingmar
// functions implemented in the .C file can not be inlined
//
// Revision 1.41  2002/07/16 23:25:23  geconomos
// no message
//
// Revision 1.40  2002/06/04 19:11:46  manju
// Coding standard
//
// Revision 1.39  2002/05/29 19:05:38  manju
// Coding standard.
//
// Revision 1.38  2002/05/22 17:45:20  manju
// Storing the data modality used while populating the annotations table.
//
// Revision 1.37  2002/05/17 00:50:11  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.36  2002/05/10 15:12:15  manju
// Removed measurement status as argument to CalculateArea.
//
// Revision 1.35  2002/05/10 14:04:03  manju
// Lasso statistics now work properly in Sagittal and Coronal views too.
//
// Revision 1.34  2002/05/09 18:52:37  manju
// Changed labelvolume from int2 to uint2 in Update method.
//
// Revision 1.33  2002/05/06 14:01:44  manju
// Removed modality as argument ot CalculateStatistics method.
// Changed names of some variables in this method.
//
// Revision 1.32  2002/05/03 18:27:47  manju
// Calculating area inside the CalculateStatsitics method.
//
// Revision 1.31  2002/05/01 21:38:55  michael
// Adjusted GetDataModality() to GetDataModalityEnum()
//
// Revision 1.30  2002/05/01 20:25:18  manju
// Slight change.
//
// Revision 1.29  2002/04/25 16:21:35  manju
// Implemented the GetNoHandlePoints method.
//
// Revision 1.28  2002/04/25 03:13:53  michael
// Added missing includes (due to include replacement by forward declaration in header file)
//
// Revision 1.27  2002/04/24 21:57:45  manju
// Using block volume in Update method.
//
// Revision 1.26  2002/04/23 14:54:39  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.25  2002/04/22 18:06:33  manju
// Changed Update method to take in reference of density volume.
//
// Revision 1.24  2002/04/18 23:05:45  frank
// Added function to fill in the polygon for the entire slice.
//
// Revision 1.23  2002/04/18 21:41:55  frank
// spelling
//
// Revision 1.22  2002/04/18 20:19:46  frank
// Added sculpting manipulator and renderer.
//
// Revision 1.21  2002/04/18 14:29:17  manju
// Added a method Update to compute statistics after deserialization
// of the annotation objects.
//
// Revision 1.20  2002/04/02 22:18:49  manju
// initializing the status flag to false.
//
// Revision 1.19  2002/03/27 22:35:03  manju
// Removed MoveHandlePoints method.
//
// Revision 1.18  2002/03/27 18:13:58  manju
// Added methods to get number of handle points and to get position of
// a handle point.
//
// Revision 1.17  2002/03/25 16:21:06  michael
// removed VolumeSetExt dependencies from annotations, only CardiacPlaque remains
//
// Revision 1.16  2002/03/21 18:35:10  manju
// cleaned the #includes and added forward declarations where necessary
//
// Revision 1.15  2002/01/31 23:53:19  wenli
// Lasso TopLeft position Point<uint2> has been changed to Point<int2>.
//
// Revision 1.14  2002/01/21 17:38:05  jaddonisio
// Include "Component.h".
//
// Revision 1.13  2002/01/07 16:11:03  manju
// Coding Standards.
//
// Revision 1.12  2002/01/04 17:55:03  wenli
// 2D lasso and 3D lasso are differentiated by a flag
//
// Revision 1.11  2002/01/03 16:24:45  manju
// Changed class names and added to a new project 51_annotation.
// MeasurementList -> HandlesSet
// Measurement -> Annotation
// Some function names have also been changed to be consistent.
//
// Revision 1.10  2001/12/17 17:55:12  wenli
// using ctrl+left mouse to pick plaque in 3D window;
// removed the bugs in MPR plaque selection
//
// Revision 1.9  2001/12/12 22:13:03  wenli
// MIP Selection
//
// Revision 1.8  2001/12/03 19:20:15  manju
// Added comments.
//
// Revision 1.7  2001/11/22 00:09:43  manju
// Taking into account CT and other types of modalities for statistics calculation.
//
// Revision 1.6  2001/11/17 00:02:14  manju
// Got back the statistics flag for lasso.
//
// Revision 1.5  2001/11/16 23:50:57  manju
// Initialized voxel count to 0.
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxContour.C,v 1.2 2004/03/12 19:49:15 frank Exp $
// $Id: vxContour.C,v 1.2 2004/03/12 19:49:15 frank Exp $





















