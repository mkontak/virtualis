// $Id: vxRendererOverlayMPROrtho.C,v 1.13 2007/01/16 17:02:32 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererOverlayMPROrtho.h"
#include "vxUtils.h"
#include "vxWindowLevel.h"
#include "vxBlockVolume.h"
#include "vxEnvironment.h"
#include "GLFontGeorge.h"
#include "vxBlockVolumeIterator.h"
#include "vxCropBox.h"
#include "vxViewerTypes.h"
#include "vxOverlayStrings.h"
#include "vxRenderingMode.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxVisualizationPreferences.h"
#include "vxCine.h"
#include "vxDicomRCSutilities.h"

// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.13 $"


/**
 * Constructor
 * @param environment the environment
 */
vxRendererOverlayMPROrtho::vxRendererOverlayMPROrtho(vxEnvironment & environment)
:vxRendererOverlayMPR(environment)
//******************************************************************* 
{
} // vxRendererOverlayMPROrtho()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererOverlayMPROrtho::IsRedrawNecessary()
//******************************************************************* 
{
  bool bRedrawNecessary(vxRendererOverlayMPR::IsRedrawNecessary());

  //bool bChangedZoomFactor(GetEnvironment().IsModified(vxEnvironment::ZOOM_FACTOR));

  //bRedrawNecessary |= bChangedZoomFactor;

  return bRedrawNecessary;
} // IsRedrawNecessary()


/**
 * Render 2D overlay.
 */  
void vxRendererOverlayMPROrtho::Render()
//******************************************************************* 
{
  // see if we are in an ortho MPR view
  if (NULL == dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()))
  {
    throw ex::UnsupportedDataTypeException(LogRec("wrong viewer type in environment!!!", "vxRendererOverlayMPROrtho", "Render"));
  }

  // call base class for basic information to be displayed
  vxRendererOverlayMPR::Render();

  // exit if there is no volume
  if ( GetEnvironment().GetIntensityVolume() == NULL ) return;
  if ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim().GetMaxComponent() == 0 ) return;

  // display additional information
  const Triple<int4> dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  if ( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) )
  {
    pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( GetFieldOfViewString (), vxOverlayStrings::vxOverlayElement::UpperRight ) );

    // Zoom text - only show if different from 100%, top right
    uint2 uAxisX( GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord() );
    uint2 uAxisY( GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord() );
    float4 fWidthX( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[ uAxisX ] );
    float4 fHeightY( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[ uAxisY ] );
    float4 fWidth( GetEnvironment().GetZoomFactor()->GetSize() );
    float4 fWidthMax( max( fWidthX, fHeightY ) );
    int1 sTmpStr[201];

    if ( fWidth != fWidthMax )
    {
      sprintf(sTmpStr, "Zoom: %i%%%%", static_cast< int4 >( 100.0F * fWidthMax / fWidth ) );
      pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sTmpStr, vxOverlayStrings::vxOverlayElement::UpperRight ) );

			//If zoom factor is greater than 250 start displaying HU Values
			if ( ( 100.0F * fWidthMax / fWidth ) > 700 &&  vxVisualizationPreferences::GetShowHU() )
				AddHUOverlayElements();
    }

    if ( GetEnvironment().GetCine()->GetCineMode() != vxCine::IDLE )
    {
      sprintf( sTmpStr, "Cine %.1ffps", 1.0F / m_cineTimer.ElapsedTime() );
      pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( sTmpStr, vxOverlayStrings::vxOverlayElement::UpperRight ) );
    }

		pOverlayStrings->AddElement(vxOverlayStrings::vxOverlayElement(GetAxialSliceDicomPositionString(), vxOverlayStrings::vxOverlayElement::UpperRight));

		/// render the unit length text information on top of the 2D ruler at the left edge of the view window
		RenderRulerUnitLength();

  } // if

  m_cineTimer.Reset();

} // Render()


/*
 *Adds the HU overlay string element
 */
void vxRendererOverlayMPROrtho::AddHUOverlayElement( Vector3Df voxelPos )
{
	//Use this copy to find the World position.
	Vector3Df currentVoxelPos = voxelPos;

	//Find the screen position
	Point3Df worldPos( currentVoxelPos.ScaleAnIso( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() ) );
	Point2D<int2> screenPos;
	GetEnvironment().GetViewerType()->ConvertWorldToScreen( worldPos, screenPos );

	//add the element to the Overlay strings
	vxOverlayStrings::vxOverlayElement element( GetVoxelString(voxelPos),  vxOverlayStrings::vxOverlayElement::Random );
	element.SetRandomScreenPosition( screenPos );
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
	pOverlayStrings->AddElement( element );

}//AddHUOverlayElement( Vector3Df currentVolPos )

/*
 * Adds the HU overlay string elements
 * Based on the current voxel pos, this method searches the neighbour's HU also
 */
void vxRendererOverlayMPROrtho::AddHUOverlayElements()
{
	int iAxisIndex= -1;
	if ( GetEnvironment().GetActiveEnvironment()  != NULL && GetEnvironment().GetActiveEnvironment()->GetPlaneOrtho() != NULL )
		iAxisIndex = (GetEnvironment().GetActiveEnvironment() ->GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

	if ( iAxisIndex < 0 || iAxisIndex > 2 ) 
		return;

	Vector3Df currentVoxelPos( * GetEnvironment().GetCurrentVoxelPos() );
	AddHUOverlayElement( currentVoxelPos );
	
	//Add the HU overlay element for the neighbour voxels
	vxBlockVolumeIteratorNeighborsConsts::NeigborhoodsEnum searchPlane;
	
	if ( iAxisIndex ==  2)//Axial
		searchPlane = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xy;
	else if ( iAxisIndex ==  1) //coronal
		searchPlane = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08xz;
	else if ( iAxisIndex ==  0)//sagittal
		searchPlane = vxBlockVolumeIteratorNeighborsConsts::e2Dneighbors08yz;

	vxBlockVolumeIteratorNeighbors<uint2> iterNeighbour( * GetEnvironment().GetIntensityVolume(), currentVoxelPos, searchPlane  );
  // set neighborhood at point in region and loop around neighborhood and add HUs for them
  for (iterNeighbour.CenterAt(currentVoxelPos); !iterNeighbour.IsAtEnd(); ++iterNeighbour)
  {
    if (iterNeighbour.IsInsideVolume())
    {
			AddHUOverlayElement( iterNeighbour.GetPos() );
    }
	}
}//AddHUOverlayElements



/**
 * Return the slice type text string
 * @return the slice type text string
 */
std::string vxRendererOverlayMPROrtho::GetSliceTypeString()
//*******************************************************************
{
  static char sOrientationText[3][10] = { "Sagittal\0", "Coronal\0", "Axial\0" };
  const int uAxisIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());

  return sOrientationText[uAxisIndex];
} // GetSliceTypeString()


/**
 * Return the current slice number as a string
 * @param environment the environment
 * @return the current slice number as a string
 */
std::string vxRendererOverlayMPROrtho::GetSliceNumberString()
{

  const bool bMIPRendering( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP );
  const bool bSlidingThinSlab( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB );
  const bool bXRay( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::XRAY);

  const Vector3D<int4> dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  Triple<int4> volumeDim( Triple<int4>( dim.X(), dim.Y(), dim.Z() ) );
  vxPlaneOrtho<float4> * pSlice( GetEnvironment().GetPlaneOrtho() );
  uint1 uAxisIndex( pSlice->GetNormal().GetMaxAbsComponentCoord() );
  uint2 uNumberOfSlices( volumeDim[ uAxisIndex ] );
  float4 fInterSliceDistance( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits()[ uAxisIndex ] );
  uint2 uSliceIndex = uint2( 0.5F + GetEnvironment().GetPlaneOrtho()->GetDistance() / fInterSliceDistance );
  uint2 uCurrentSlice( vxUtils::CalculateOffsetDistance( uSliceIndex, uNumberOfSlices, GetEnvironment().GetOffset(), GetEnvironment().GetMaximumOffset() ) );
  int1 sTmpStr[ 201 ];

  if ( bSlidingThinSlab == false && bMIPRendering == false && bXRay == false )
  {

    // we need to invert the slice number since 1 should be at the top (head)
    uint2 uFinalSliceNumber( ( uAxisIndex == 4 ) ? ( ( uNumberOfSlices == 0 ) ? 0 : uNumberOfSlices - uCurrentSlice ) : uCurrentSlice + 1 );
    string sNumber( ToAscii( uFinalSliceNumber ) );
    if ( uFinalSliceNumber < 1 || uFinalSliceNumber > uNumberOfSlices ) sNumber = "?";
    sprintf( sTmpStr, "Image: %s/%i", sNumber.c_str(), uNumberOfSlices );

  }
  else
  {

    // compute the two slice positions
    const pair< uint4, uint4 > thinSlab( vxUtils::GetSlabPair( GetEnvironment() ) );

    // we need to invert the slice number since 1 should be at the top (head)
    uint2 uSlice1(  thinSlab.first  );
    uint2 uSlice2(  thinSlab.second );

    // always sort min to max
    if ( uSlice2 < uSlice1 ) Swap( uSlice1, uSlice2 );
    ++uSlice1;
    ++uSlice2;
    string sSlice1( ToAscii( uSlice1 ) );
    string sSlice2( ToAscii( uSlice2 ) );
    if ( ( uSlice1 < 1 || uSlice1 > uNumberOfSlices ) && ( uSlice2 < 1 || uSlice2 > uNumberOfSlices ) )
      sSlice1 = sSlice2 = "?";

    // render the string
    string sImageType( bMIPRendering ? "MIP" : ( bXRay ? "x-ray" : "VR" ) );
    const float4 fThicknessMM( GetEnvironment().GetRenderingMode()->GetSlabThickness() );
    sprintf( sTmpStr, "%.0fmm %s: %s-%s/%i", fThicknessMM, sImageType.c_str(), sSlice1.c_str(), sSlice2.c_str(), uNumberOfSlices );

  }

  return sTmpStr;

} // GetSliceNumberString()


/**
 *  Get the axial slice dicom z position string
 *
 * @return "NOTAPPLICABLE" if the current view mode is MIP, thin slab, or x-ray; "DICOM Coordinates: " otherwise
 */
std::string vxRendererOverlayMPROrtho::GetAxialSliceDicomPositionString()
{
	std::string sTmp("");

  const bool bMIPRendering( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::MIP );
  const bool bSlidingThinSlab( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB );
  const bool bXRay( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::XRAY);

	if( ! (bMIPRendering || bSlidingThinSlab || bXRay) ) 
	{ 
	  const Vector3Di posVolume( * GetEnvironment().GetCurrentVoxelPos() );

    vxBlockVolumeIterator<uint2> iter( * static_cast< vxBlockVolume<uint2> * >( GetEnvironment().GetIntensityVolume() ) );
    iter.SetPos( posVolume );
		if( ! iter.IsInsideVolume() ) return sTmp;

		const Triple<float4> vfUnits( static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits() );
		
		vxDicomRCSutilities dcmrcs( static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader() );
		
		if( !dcmrcs.CalulateOriginRCS() ) return sTmp;

		Vector3Df vfWorldCoordinate = dcmrcs.ConvertVolCoordinate2DcmRCS( posVolume );

    char sTmpStr[201];

		sprintf (sTmpStr, "Pos(mm): (%5.1f, %5.1f, %5.1f)", vfWorldCoordinate.m_x, vfWorldCoordinate.m_y, vfWorldCoordinate.m_z );

    return sTmpStr;
	} // if it is normal 2D rendering mode

	return sTmp;
} // GetAxialSliceDicomPositionString()


/**
 * Determine if the current slice (or slab) intersects the crop box and display message if not
 */
void vxRendererOverlayMPROrtho::ExplainMissingImage()
{

  bool bOutsideVolume( false );
  const Triple<int4> dim( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() );
  const Triple<float4> units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
  vxPlaneOrtho<float4> * pPlane = GetEnvironment().GetPlaneOrtho();
  uint1 uAxisIndex(pPlane->GetNormal().GetMaxAbsComponentCoord()); // 0, 1, 2.
  uint2 uNumberOfSlices( dim[ uAxisIndex ] );
  float4 fPlaneDistanceWorldCoordinates( GetEnvironment().GetPlaneOrtho()->GetDistance() );
  float4 fPlaneDistanceVolumeCoordinates( fPlaneDistanceWorldCoordinates / units[ uAxisIndex ] );
  uint4 uCurrentSlice( vxUtils::CalculateOffsetDistance( fPlaneDistanceVolumeCoordinates, uNumberOfSlices, GetEnvironment().GetOffset(), GetEnvironment().GetMaximumOffset() ) );
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(  GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  if ( uNumberOfSlices > 1 )
  {
    std::pair< float4, float4> cropBoundsVolumeCoordinates( 0, uNumberOfSlices-1 );
    vxCropBox * pCropBox = GetEnvironment().GetCropBox();
    if( GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) )
    {
      cropBoundsVolumeCoordinates = make_pair( pCropBox->GetBox().GetMinPoint()[ uAxisIndex ] / units[ uAxisIndex ],
                                               pCropBox->GetBox().GetMaxPoint()[ uAxisIndex ] / units[ uAxisIndex ] );
    }
    
    if ( GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() == vxRenderingMode::DEFAULT )         
    {

      // single slice rendering
      if ( ( uCurrentSlice < cropBoundsVolumeCoordinates.first ) ||
           ( uCurrentSlice > cropBoundsVolumeCoordinates.second ) )
      {
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "The current slice is outside", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "the bounds of the crop box." , vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
      }
      else if ( ( uCurrentSlice < 0 ) || ( int4( uCurrentSlice ) > uNumberOfSlices-1 ) )
      {
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "The current slice is outside", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "the bounds of the volume."   , vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
      }
    }
    else
    {

      // slab rendering
      const float fThicknessMM(GetEnvironment().GetRenderingMode()->GetSlabThickness());
      const int4 iNumSlices(GetEnvironment().GetRenderingMode()->GetSlabThickness() / units[uAxisIndex]);
      const pair< uint4, uint4 > thinSlab( vxUtils::GetSlabPair( dim, units, iNumSlices, uAxisIndex, fPlaneDistanceVolumeCoordinates ) );
      if ( ( thinSlab.second < cropBoundsVolumeCoordinates.first ) || ( thinSlab.first > cropBoundsVolumeCoordinates.second ) )
      {
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "The current slab is outside", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "the bounds of the crop box.", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
      }
      else if ( ( thinSlab.first > uNumberOfSlices-1 ) || ( thinSlab.second > uNumberOfSlices-1 ) )
      {
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "The current slab is outside", vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
        pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( "the bounds of the volume."  , vxOverlayStrings::vxOverlayElement::MiddleMiddle, Triple<float4>( 1, 1, 0 ) ) );
      }

    }
  } // if more than one slice

} // ExplainMissingImage()


/**
 *  display the ruler unit length information on top of the 2D ruler
 */
void vxRendererOverlayMPROrtho::RenderRulerUnitLength()
{ 
	//Create a ruler object. 
	std::pair<uint2, uint2> size = GetEnvironment().GetWinRect().GetViewportSize();

	// Create a ruler.
	vxRuler2D ruler(GetEnvironment().GetViewerType(), size, true);

	// Screen positions for the ruler's begin and end.
	Point2D<int2> screenPosUpper, screenPosLower;

	// Get the screen coordinates for the OpenGL world coordinates.
	GetEnvironment().GetViewerType()->ConvertWorldToScreenUsingOpenGL(ruler.GetStartPoint(), screenPosUpper);
	GetEnvironment().GetViewerType()->ConvertWorldToScreenUsingOpenGL(ruler.GetEndPoint(), screenPosLower);

	// v3D world coordinates for the ruler begin and end points.
	Point3Df startWorld, endWorld;

	//// Convert the screen positions into world positions.
	GetEnvironment().GetViewerType()->ConvertScreenToWorld(screenPosUpper, startWorld);
	GetEnvironment().GetViewerType()->ConvertScreenToWorld(screenPosLower, endWorld);

	// Divide the euclidean distance by the number of subdivisions to give the unit distance.
	float8 unitDistance = startWorld.GetEuclideanDistance(endWorld) / 8.0f;

	// Create overlay for the "Units".
	vxOverlayStrings::vxOverlayElement overlayUnits("Unit", vxOverlayStrings::vxOverlayElement::Random);
	overlayUnits.SetRandomScreenPosition(Point2D<float4>(screenPosLower.X() + 15, screenPosLower.Y() + 20));
	overlayUnits.SetColor(Triple<float4>(1, 1, 1));

	// Create overlay for the Units in mm.
	vxOverlayStrings::vxOverlayElement overlayScale(ToAscii(RoundDecimals(unitDistance, 1)) + "mm", vxOverlayStrings::vxOverlayElement::Random);
	overlayScale.SetRandomScreenPosition(Point2D<float4>(screenPosLower.X() + 15, screenPosLower.Y() + 4));
	overlayScale.SetColor(Triple<float4>(1, 1, 1));

	// Add the overlays to the list of overlay strings for display.
	vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >(GetEnvironment().GetElement(vxIDs::OverlayStrings));
	pOverlayStrings->AddElement(overlayUnits);
	pOverlayStrings->AddElement(overlayScale);

} // RenderRulerUnitLength()


#undef FILE_REVISION


// $Log: vxRendererOverlayMPROrtho.C,v $
// Revision 1.13  2007/01/16 17:02:32  gdavidson
// Added vxCine
//
// Revision 1.12  2006/11/24 19:52:25  romy
// added shosHU flag check to display the HU values.
//
// Revision 1.11  2006/11/24 19:17:06  romy
// Modified the Zoom factor condition to display the HU values so that they won't override
//
// Revision 1.10  2006/10/30 17:37:19  romy
// using  a temp variable to hold the original point becasue ScaleAnIso changes the original values.
//
// Revision 1.9  2006/10/27 07:44:54  romy
// HU display for neighbours added for Zoom > 250
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.6.3  2005/08/17 12:46:30  geconomos
// Issue#4350: Fixed axial slice number string for mip, xray, and thin slab
//
// Revision 1.6.6.2  2005/08/15 19:37:49  geconomos
// switched axial start and end distances
//
// Revision 1.6.6.1  2005/07/19 11:49:45  frank
// missing include
//
// Revision 1.6  2004/10/23 10:23:34  frank
// made some functions not static
//
// Revision 1.5  2004/10/05 14:32:42  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.4  2004/08/31 15:17:07  frank
// fixed zoom and field of view
//
// Revision 1.3  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/06 16:27:21  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2004/02/11 14:06:48  frank
// fixed restricted segmentation and slice computations
//
// Revision 1.8  2004/02/09 13:40:15  frank
// fixed the slice numbers in slab views
//
// Revision 1.7  2003/12/17 18:02:56  michael
// accelerating the Redraw() by finding out which renderer requires a redraw
// and also allowing for a better debugging mechanism of that information.
//
// Revision 1.6  2003/12/09 16:08:18  frank
// Fixed cine frames per second
//
// Revision 1.5  2003/11/19 06:16:51  frank
// cleaned up conditionals
//
// Revision 1.4  2003/11/10 13:53:02  frank
// Adjusted the crop box bounds and missing image text to be coordinated
//
// Revision 1.3  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.2  2003/11/05 01:18:48  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.1  2003/11/04 15:46:40  michael
// added new overlay classes that follow the same inheritance hierachy as the
// view types, manipulators, and renderers.
//
// Revision 1.57  2003/09/04 17:29:45  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.56  2003/09/02 17:55:52  michael
// made distance between lines (m_fInterlineSpacing) font size dependent
//
// Revision 1.55  2003/08/22 19:24:01  frank
// Issue #3460: Revised orientation lettering method - unified between 2D and 3D
//
// Revision 1.54  2003/08/13 12:55:44  michael
// accounted for change from OverlayStrings to vxOverlayStrings
//
// Revision 1.53.2.1  2003/08/29 15:20:12  frank
// Fixed orientation lettering in 3D
//
// Revision 1.53  2003/06/18 15:19:58  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.52  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.51  2003/06/03 20:31:53  michael
// added check to see if volume is valid (each dimension > 0)
//
// Revision 1.50  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.49  2003/05/23 18:56:41  frank
// Moved "show patient information" logic out of render function
//
// Revision 1.48  2003/05/22 13:04:29  frank
// code review
//
// Revision 1.47  2003/05/19 14:39:21  frank
// Issue #3249: Slice out of bounds message improperly shown
//
// Revision 1.46  2003/05/17 15:44:50  michael
// reformatting
//
// Revision 1.45  2003/05/05 16:04:35  frank
// Refactored overlay rendering code so it can be easily re-used
//
// Revision 1.44  2003/05/02 19:01:09  frank
// Cleaned up dead code
//
// Revision 1.43  2003/05/01 20:13:00  frank
// Moved to using the more flexible overlay strings
//
// Revision 1.42  2003/05/01 14:35:29  frank
// reflected parameter change in vxRendererOverlay
//
// Revision 1.41  2003/04/18 15:20:46  frank
// Removed HUconverter
//
// Revision 1.40  2003/03/21 15:40:40  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.39  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.38  2003/02/18 16:41:09  frank
// Fixed "out of bounds" on first axial slice
//
// Revision 1.37  2003/02/14 19:00:20  frank
// Fixed overlay for MR datasets
//
// Revision 1.36  2003/01/22 22:02:28  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.35  2003/01/21 22:14:57  frank
// Removed RenderingNecessary flag in overlays
//
// Revision 1.34  2003/01/13 23:25:46  frank
// Removed old parameters, fixed text layout
//
// Revision 1.33  2003/01/08 21:48:27  frank
// Added additional study info to overlays.
//
// Revision 1.32  2002/12/20 16:14:35  frank
// Added x-ray projection
//
// Revision 1.31  2002/11/27 20:59:28  frank
// Fixed warning text when outside crop box or volume.
//
// Revision 1.30  2002/11/22 01:46:24  frank
// Added current HU/Raw display as string function to remove tons of redundant code.
//
// Revision 1.29  2002/11/21 23:30:08  michael
// added overlay information for oblique
//
// Revision 1.28  2002/11/20 17:46:31  michael
// added view dependent stuff to handle VIEW_MPR_OBLIQUE, too
//
// Revision 1.27  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.26  2002/11/08 03:11:49  michael
// fixed issue with slice number display (for single slice rendering)
//
// Revision 1.25  2002/11/02 03:50:06  frank
// working on crop box
//
// Revision 1.24  2002/11/02 03:09:31  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.23  2002/09/30 17:07:46  frank
// Issue #2695: Cleaned up phase number and patient information opn phase renderer overlay display.
//
// Revision 1.22  2002/09/24 19:41:14  frank
// fixed issue #2675
//
// Revision 1.21  2002/09/24 15:32:49  frank
// Added explanation for missing images when the slab or slice is outside of the current crop box.
//
// Revision 1.20  2002/09/18 20:42:10  frank
// Issue #2648: Fixed toggling of certain info with patient info.
//
// Revision 1.19  2002/09/09 20:40:20  frank
// Fixed upper left string display.
//
// Revision 1.18  2002/08/17 12:31:56  frank
// Centralized display of strings in the upper two corners. Now you just add strings to the two lists and they are rendered for you by RenderCornerStrings.  No more hand formatting.
//
// Revision 1.17  2002/06/25 16:06:34  michael
// Extended what is removed when patient information is off
//
// Revision 1.16  2002/06/06 18:01:30  dmitry
// Cine speed (fps) displayed when it is active.
//
// Revision 1.15  2002/06/05 23:14:19  frank
// Changed the FOV to cm.
//
// Revision 1.14  2002/06/04 17:10:39  michael
// added comment
//
// Revision 1.13  2002/06/04 17:06:16  michael
// added size in mm of current viewport
//
// Revision 1.12  2002/05/22 19:59:04  jenny
// fixed orientation lettering and removed a bunch of functions to process the age, patient id, sex and so on, put them in VolumeHeader
//
// Revision 1.11  2002/05/17 00:47:49  jenny
// worked on VolumeHeader Version 4.0
//
// Revision 1.10  2002/05/07 19:57:21  michael
// fixed patient age and sex display in the viewers
//
// Revision 1.9  2002/05/03 17:49:51  jenny
// zoom factor
//
// Revision 1.8  2002/05/01 21:38:55  michael
// Adjusted GetDataModality() to GetDataModalityEnum()
//
// Revision 1.7  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.6  2002/05/01 00:02:21  michael
// Added HUConverter to include list
//
// Revision 1.5  2002/04/29 15:18:40  frank
// HU is back.
//
// Revision 1.4  2002/04/27 04:21:06  frank
// Began centralizing the patient information block.
//
// Revision 1.3  2002/04/26 22:55:05  frank
// Moved HU to right.
//
// Revision 1.2  2002/04/26 13:39:55  michael
// coding guidelines: adjusted some parameter and variable naming
//
// Revision 1.1  2002/04/26 13:10:50  michael
// Added renamed version of the overlay renderers by moving {2,3}D to the end
//
// Revision 1.29  2002/04/25 21:10:20  frank
// Working on MRI value display.
//
// Revision 1.28  2002/04/25 19:54:39  frank
// Reflected change in block iterator.
//
// Revision 1.27  2002/04/25 19:39:58  frank
// Centralized orientation lettering in vxRendererOverlay class.
//
// Revision 1.26  2002/04/25 16:59:41  jenny
// HU display
//
// Revision 1.25  2002/04/24 18:38:11  jenny
// implemented HU value
//
// Revision 1.24  2002/04/24 14:59:43  jenny
// vxBlockVolume stuff
//
// Revision 1.23  2002/04/24 03:03:31  michael
// added test if rendering is necessary (false if no volume available)
//
// Revision 1.22  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.21  2002/04/23 15:39:57  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.20  2002/04/21 22:57:40  michael
// Changed HU converter to return int2 instead of requiring it to be a second parameter
//
// Revision 1.19  2002/04/21 21:58:28  michael
// removed HU conversion from WindowLevel class since it is not a property of this object
//
// Revision 1.18  2002/04/21 20:07:47  frank
// Lined up the text.
//
// Revision 1.17  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.16  2002/04/14 00:18:00  michael
// adjusted method name
//
// Revision 1.15  2002/04/09 21:20:48  dmitry
// Offset calculated for the plane.
//
// Revision 1.14  2002/04/01 23:29:51  michael
// Fixed offsets to match 3D overlay offsets and corrected wrong labels
//
// Revision 1.13  2002/04/01 16:37:09  michael
// push and pop of attributes happens once in Render()
//
// Revision 1.12  2002/03/29 16:06:33  michael
// cosmetics
//
// Revision 1.11  2002/03/28 21:44:35  geconomos
// Disabled depth test  in Render().
//
// Revision 1.10  2002/03/27 16:10:05  michael
// cosmetics
//
// Revision 1.9  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.8  2002/03/25 16:33:46  michael
// adjusted to VolumeData::ModalityEnum
//
// Revision 1.7  2002/03/21 22:00:35  uday
// push-pop edit
//
// Revision 1.6  2002/03/21 20:08:39  jenny
// put comments
//
// Revision 1.5  2002/03/21 19:16:41  michael
// added log history lines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererOverlayMPROrtho.C,v 1.13 2007/01/16 17:02:32 gdavidson Exp $
// $Id: vxRendererOverlayMPROrtho.C,v 1.13 2007/01/16 17:02:32 gdavidson Exp $
