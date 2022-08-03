`// $Id: vxRendererVessel3D.C,v 1.7 2007/08/09 19:37:56 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Renders the vessel centerline and outline in 3D.
 */

// includes
#include "StdAfx.h"
#include "vxRendererVessel3D.h"
#include "vxEnvironment.h"
#include "vxVesselSection.h"
#include "vxRendererSoftwareMPROblique.h"
#include "vxViewerTypeVesselDetailEndoluminal.h"
#include "vxViewerTypeMPRCurved.h"
#include "ValueRange.h"
#include "vxElementFloat4.h"
#include "vxRendererUtils.h"
#include "vxRibbon.h"
#include "Line.h"
#include "Plane.h"
#include "RaySegment.h"
#include "vxSeedpoints.h"
#include "vxOverlayStrings.h"
#include "vxVesselUtilities.h"
#include "vxVesselPreferences.h"
#include "vxVessel.h"


// Defines
#define FILE_REVISION "$Revision: 1.7 $"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererVessel3D::vxRendererVessel3D( vxEnvironment & environment )
: vxRenderer( environment )
{
} // Constructor


/**
 * Destructor
 */
vxRendererVessel3D::~vxRendererVessel3D()
{
} // Destructor


/**
 * Has something changed that makes redrawing necessary?
 *
 * @return redraw necessary?
 */
bool vxRendererVessel3D::IsRedrawNecessary()
{
  return true;

  const bool bPreferencesChanged( GetEnvironment().IsModified( vxIDs::VesselPreferences ) );
  const bool bVesselChanged( GetEnvironment().IsModified( vxIDs::VesselArray ) );
  const bool bSeedpointsChanged( GetEnvironment().IsModified( vxIDs::Seedpoints ) );

  return bPreferencesChanged || bVesselChanged || bSeedpointsChanged;

} // IsRedrawNecessary()


/**
 * Render the vessel using OpenGL
 */
void vxRendererVessel3D::Render()
{
  // get the rendering preferences
  vxVesselPreferences * pPreferences = NULL;
  try
  {
    pPreferences = dynamic_cast< vxVesselPreferences * >(GetEnvironment().GetElement( vxIDs::VesselPreferences ));
  }
  catch ( ... )
  {
    LogWrn( "problem in dynamic cast", "vxRendererVessel3D", "Render" );
    return;
  }
  if( pPreferences == NULL )
  {
    LogDbg( "Preferences pointer is NULL", "vxRendererVessel3D", "Render" );
    return;
  }

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  // possibly scale for detail volume
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  TransformDetail();

  // set up antialiasing
  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glLineWidth( 1.0F );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

  // render outline
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel != NULL && pPreferences->GetDisplayOutline() )
    RenderContours();

  // render centerline and glyphs
  if( pVessel != NULL && pPreferences->GetDisplayCenterline() )
  {
    RenderCenterline();
    RenderAreaGlyphs();
    RenderDiameterGlyphs();
  }

  // render the seed points
  RenderSeedpoints();

	//display the Arrow if the Slice shadow mode is off. Else use the one from Sliceshadow mode.
	//Why there is an arrow in slice shadow mode ?????
	if ( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_NONE	)
	{
		//Do not show the arrow in Endoluminal Views. That's looks so nasty
		const vxViewerType * pViewerType = dynamic_cast<vxViewerTypeVesselDetailEndoluminal*> ( GetEnvironment().GetViewerType() );
		if ( pViewerType == NULL )
			RendererArrow();
	}

  // restore the OpenGL state
  glPopMatrix();
  glPopAttrib();

} // Render()


/**
 * Transform OpenGL for the detail volume
 */
void vxRendererVessel3D::TransformDetail() const
{
  Point<float4> centerPoint;
  if( GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar( "VolumeCenter", centerPoint ) )
  {
    // get the location of (0,0,0) in the original volume
    Vector<float4> worldDim( GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() );
    Point<float4> volMinPoint( centerPoint - worldDim / 2.0F );

    glTranslatef( -volMinPoint.m_x, -volMinPoint.m_y, -volMinPoint.m_z );
  }
} // TransformDetail()


/**
 * Render the arrow 
 */
void vxRendererVessel3D::RendererArrow() const
{
	// render outline
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( pVessel == NULL  || GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) == NULL )
		return;

	const float4 fVesselSectionIndex = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) )->GetValue();
  if ( fVesselSectionIndex >= 0.0F && fVesselSectionIndex <= pVessel->size() - 1 )
  {
    // compute the vessel radius at the specified vessel  sectionj
		const float fVesselRadius = pVessel->GetDiameterRange( vxVessel::MeasurePair( fVesselSectionIndex, fVesselSectionIndex ) ).GetMaximum() / 2.0F;

    Normal< float4 >  cameraDirection = GetEnvironment().GetCamera()->GetViewDirection();
    Point<float4> refercentPnt( pVessel->GetResampledPosition( fVesselSectionIndex ) + fVesselRadius * cameraDirection );
    Normal3Df viewDirection( pVessel->GetResampledTangent( fVesselSectionIndex ) );

    const float4 fArrowLength( 10.0F );
    const float4 fArrowLengthOnScreen( 8.0F );
    Point<float4> arrowBgn( refercentPnt );
    Point<float4> arrowEnd( refercentPnt - fArrowLength*viewDirection );

    const vxViewerType * pViewerType = GetEnvironment().GetViewerType();
    if( pViewerType->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace )
    {
      Point2D<int2> screenPos;

      pViewerType->ConvertWorldToScreen(arrowBgn, screenPos);
      arrowBgn.m_x = screenPos.m_x;
      arrowBgn.m_y = screenPos.m_y;
      arrowBgn.m_z = 0;

      pViewerType->ConvertWorldToScreen(arrowEnd, screenPos);
      arrowEnd.m_x = screenPos.m_x;
      arrowEnd.m_y = screenPos.m_y;
      arrowEnd.m_z = 0;
    }
    Vector3Df arrowDir( arrowEnd - arrowBgn  );
    if( arrowDir.GetLength() < 1.0F )
      arrowDir = Vector3Df( 0.0F, 0.0F, 1.0F );
    arrowDir.Normalize();
    Point3Df arrowHead( arrowBgn - 0.5 * fArrowLengthOnScreen * arrowDir );

    vxRendererUtils::RenderArrow( arrowHead, Normal3Df( arrowDir ), fArrowLengthOnScreen, true, false, PixelRGBA<float4>( 0.0F, 0.0F, 1.0F, 1.0F ) );
 }
}//RendererArrow() const


/**
 * Render the centerline
 */
void vxRendererVessel3D::RenderCenterline() const
{
  glPushAttrib( GL_LINE_BIT );
  glLineWidth( 2.0F );
  glBegin( GL_LINE_STRIP );

  // loop over centerline points
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  std::vector< vxVesselSection >::iterator it( pVessel->begin() );
  for( int4 i(0); it != pVessel->end(); ++it, ++i )
  {
    // render brighter green if we are within the measured section
    if( i >= pVessel->GetMeasurementDomain().GetMinimum() && i <= pVessel->GetMeasurementDomain().GetMaximum() )
      glColor4f( 0.0F, 1.0F, 0.0F, 1.0F );
    else
      glColor4f( 0.0F, 1.0F, 0.0F, 0.5F );
    vxRendererUtils::GlVertex( it->GetPosition() ) ;
  }

  glEnd();
  glPopAttrib();

} // RenderCenterline()


/**
* Render the contours
*/
void vxRendererVessel3D::RenderContours() const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  std::pair< uint4, uint4 > diameterIndices( pVessel->GetDiameterRangeIndices( pVessel->GetMeasurementDomain() ) );

  // loop over centerline points
  for( uint4 i(0); i<pVessel->size(); ++i )
  {

    // select a color
    Quad<float4> baseColor;
    if( i >= pVessel->GetMeasurementDomain().GetMinimum() && i <= pVessel->GetMeasurementDomain().GetMaximum() )
      baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.75F );
    else
      baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.25F );
    RenderContourPoints( i, baseColor );

    // draw the connection to the next section
    if( false )
    {
      if( i >= pVessel->GetMeasurementDomain().GetMinimum() && i+1 <= pVessel->GetMeasurementDomain().GetMaximum() )
        baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.75F );
      else
        baseColor = Quad<float4>( 0.0F, 1.0F, 0.0F, 0.25F );
      RenderContourConnection( i, baseColor );
    }
  }
} // RenderCenterline()


/**
 * Render the connection between two contours
 * @param uSectionIndex the index of the section in the vessel
 * @param baseColor     the color
 */
void vxRendererVessel3D::RenderContourConnection( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( uSectionIndex + 1 < pVessel->size() )
  {
    // loop over both this and the next section contours
    const std::vector< Point<float4> > &     contour = pVessel->at( uSectionIndex     ).GetLumenContour();
    const std::vector< Point<float4> > & nextContour = pVessel->at( uSectionIndex + 1 ).GetLumenContour();
    std::vector< Point<float4> >::const_iterator     contourIt(     contour.begin() );
    std::vector< Point<float4> >::const_iterator nextContourIt( nextContour.begin() );

    // draw
    glBegin( GL_LINES );
    int4 iMinSize( Min( contour.size(), nextContour.size() ) );
    for( int4 i(0); i<iMinSize; i += 3 )
    {
      vxRendererUtils::GlVertex(     contour[i] );
      vxRendererUtils::GlVertex( nextContour[i] );
    }
    glEnd();
  }
} // RenderContourConnection()


/**
 * Render a contour as lines
 * @param uSectionIndex the index of the section in the vessel
 * @param baseColor     the color
 */
void vxRendererVessel3D::RenderContourLines( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( uSectionIndex < pVessel->size() )
  {
    const std::vector< Point<float4> > & contour = pVessel->at( uSectionIndex ).GetLumenContour();
    std::vector< Point<float4> >::const_iterator contourIt( contour.begin() );

    glColor4f( baseColor.m_x, baseColor.m_y, baseColor.m_z, baseColor.m_t );
    glBegin( GL_LINE_LOOP );
    for( contourIt = contour.begin(); contourIt != contour.end(); ++contourIt )
      vxRendererUtils::GlVertex( * contourIt );
    glEnd();
  }
} // RenderContourLines()


/**
 * Render a contour as points
 * @param uSectionIndex the index of the section in the vessel
 * @param baseColor     the color
 */
void vxRendererVessel3D::RenderContourPoints( const uint4 uSectionIndex, const Quad<float4> & baseColor ) const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if( uSectionIndex < pVessel->size() )
  {
    const std::vector< Point<float4> > & contour = pVessel->at( uSectionIndex ).GetLumenContour();
    std::vector< Point<float4> >::const_iterator contourIt( contour.begin() );

    glColor4f( baseColor.m_x, baseColor.m_y, baseColor.m_z, baseColor.m_t );
    glBegin( GL_POINTS );
    for( contourIt = contour.begin(); contourIt != contour.end(); ++contourIt )
      vxRendererUtils::GlVertex( * contourIt );
    glEnd();
  }
} // RenderContourPoints()


/**
 * Render the area glyphs
 */
void vxRendererVessel3D::RenderAreaGlyphs() const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  std::pair< uint4, uint4 > areaIndices( pVessel->GetAreaRangeIndices( pVessel->GetMeasurementDomain() ) );
  
  // render the minimum area
  if( areaIndices.first < pVessel->size() )
    RenderContourLines( areaIndices.first + 0.5F, Quad<float4>(1,0,0,1) );
  
  // render the maximum area
  if( areaIndices.second < pVessel->size() )
    RenderContourLines( areaIndices.second + 0.5F, Quad<float4>(0,1,0,1) );

} // RenderAreaGlyphs()


/**
 * Render the diameter glyphs
 */
void vxRendererVessel3D::RenderDiameterGlyphs() const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  std::pair< uint4, uint4 > diameterIndices( pVessel->GetDiameterRangeIndices( pVessel->GetMeasurementDomain() ) );
  
  // render the minimum diameter
  if( diameterIndices.first < pVessel->size() )
  {
    RaySegment minDiam = pVessel->at( diameterIndices.first + 0.5F ).GetDiameterPoints().first;
    glColor3f( 1.0F, 0.0F, 0.0F );
    RenderDiameterGlyph( minDiam );
  }
  
  // render the maximum diameter
  glColor3f( 0.0F, 1.0F, 0.0F );
  if( diameterIndices.second < pVessel->size() )
  {
    RaySegment maxDiam = pVessel->at( diameterIndices.second + 0.5F ).GetDiameterPoints().second;
    glColor3f( 0.0F, 1.0F, 0.5F );
    RenderDiameterGlyph( maxDiam );
  }

} // RenderDiameterGlyphs()


/**
 * Render one diameter glyph
 * @param segment the diameter line segment to draw
 */
void vxRendererVessel3D::RenderDiameterGlyph( const RaySegment & segment ) const
{
  glBegin( GL_LINES );
  {
    vxRendererUtils::GlVertex( Point3Df( segment.GetStartPoint() ) );
    vxRendererUtils::GlVertex( Point3Df( segment.GetEndPoint  () ) );
  }
  glEnd();

} // RenderDiameterGlyph()


/**
 * Render the maximum tortuosity glyph
 */
void vxRendererVessel3D::RenderTortuosityGlyph() const
{
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel->size() < 1 )
  {
    LogDbg( "Vessel has no data", "vxRendererVessel3D", "RenderTortuosityGlyph" );
    return;
  }

  // get the location of maximum tortuosity
  uint4 uCenterLocation(0);
  pVessel->ComputeMaximumTortuosity( uCenterLocation, pVessel->GetMeasurementDomain() );
  vxVessel::MeasurePair tortuosityLocations( pVessel->GetEnclosingMeasurePair( uCenterLocation, vxVesselPreferences::GetToruosityArcLengthMM() ) );

  // save the rendering state
  glPushAttrib( GL_LINE_BIT );
  glLineWidth( 2.0F );
  glBegin( GL_LINE_STRIP );

  // set the color
  glColor4f( 1.0F, 0.0F, 0.0F, 1.0F );

  // draw the extensions on either side
  const float4 fExtensionLength( vxVesselPreferences::GetToruosityArcLengthMM() );
  const vxVesselSection & section1 = pVessel->at( tortuosityLocations.GetMinimum() );
  const vxVesselSection & section2 = pVessel->at( tortuosityLocations.GetMaximum() );
  const Point<float4> end1( section1.GetPosition() - Vector<float4>( section1.GetTanDirection() ) * fExtensionLength );
  const Point<float4> end2( section2.GetPosition() + Vector<float4>( section2.GetTanDirection() ) * fExtensionLength );

  // loop over centerline points
  for( int4 i(tortuosityLocations.GetMinimum()); i<tortuosityLocations.GetMaximum(); ++i )
    vxRendererUtils::GlVertex( pVessel->at( i ).GetPosition() );

  // restore the render state
  glEnd();
  glPopAttrib();

  // draw an angle glyph
  bool bDrawAngleGlyph( false );
  if ( bDrawAngleGlyph )
  {
    // make a plane from the midpoint
    const Point<float4> centerPoint( pVessel->at( uCenterLocation ).GetPosition() );
    const Vector<float4> v1( end1 - centerPoint );
    const Vector<float4> v2( end2 - centerPoint );
    Plane plane( centerPoint, Normal3Df( v1.Cross( v2 ) ) );

    // project the vectors to the plane
    Vector<float4> vp1( plane.GetProjectionParallelToPlane( v1 ) );
    Vector<float4> vp2( plane.GetProjectionParallelToPlane( v2 ) );

    // show the point of intersection
    Line line1( end1, section1.GetTanDirection() );
    Line line2( end2, - section2.GetTanDirection() );
    Point<float4> isect1( line1.GetIntersection( line2 ) );
    Point<float4> isect2( line2.GetIntersection( line1 ) );
    glBegin( GL_LINES );
    const float4 len( fExtensionLength / 2 );

    glColor4f( 1.0F, 1.0F, 0.0F, 1.0F );
    glVertex3f( isect1.m_x - len, isect1.m_y, isect1.m_z );
    glVertex3f( isect1.m_x + len, isect1.m_y, isect1.m_z );
    glVertex3f( isect1.m_x, isect1.m_y - len, isect1.m_z );
    glVertex3f( isect1.m_x, isect1.m_y + len, isect1.m_z );
    glVertex3f( isect1.m_x, isect1.m_y, isect1.m_z - len );
    glVertex3f( isect1.m_x, isect1.m_y, isect1.m_z + len );

    glColor4f( 0.0F, 1.0F, 1.0F, 1.0F );
    glVertex3f( isect2.m_x - len, isect2.m_y, isect2.m_z );
    glVertex3f( isect2.m_x + len, isect2.m_y, isect2.m_z );
    glVertex3f( isect2.m_x, isect2.m_y - len, isect2.m_z );
    glVertex3f( isect2.m_x, isect2.m_y + len, isect2.m_z );
    glVertex3f( isect2.m_x, isect2.m_y, isect2.m_z - len );
    glVertex3f( isect2.m_x, isect2.m_y, isect2.m_z + len );
    glEnd();
  } // if draw angle glyph

} // RenderTortuosityGlyph()


/**
 * Render the seedpoints
 */
void vxRendererVessel3D::RenderSeedpoints() const
{
  const float4 fRadius( 3.0F );
  const float4 fTrackPointRadius( 0.2F );

  // get the seedpoints
  vxSeedpoints * pSeedpoints = dynamic_cast< vxSeedpoints * >( GetEnvironment().GetElement( vxIDs::Seedpoints ) );
  if ( pSeedpoints == NULL )
    throw ex::DataCorruptionException( LogRec( "missing seedpoints", "vxRendererVessel3D", "RenderSeedpoints" ) );

  // convert the points to world coordinates
  const Triple<float4> & volUnits = GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits();

  // prepare to display seedpoint numbers
  vxOverlayStrings::vxOverlayElement overlayElement( "dummy", vxOverlayStrings::vxOverlayElement::Random, Triple<float4>( 1, 1, 1 ) );
  vxOverlayStrings * pOverlayStrings = dynamic_cast< vxOverlayStrings * >( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );

  // render all centerline points
  for ( int2 s(0); s < pSeedpoints->GetNumClicks(); ++s )
  {
    Point3Df seedPointWorld( pSeedpoints->GetSeedPointAsFloat(s).GetScaledAnIso( volUnits ) );
    glColor3f( 0, 1, 0 );
    RenderSphereWireframe( seedPointWorld, fRadius );

    // project to screen coordinates and display the seedpoint #
    Point2D< int2 > screenPos;
    if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( seedPointWorld, screenPos ) == true )
    {
      overlayElement.SetText( ToAscii( s + 1 ) );
      overlayElement.SetRandomScreenPosition( Point2D< float4 >( screenPos.m_x + 15, screenPos.m_y + 5 ) );
      pOverlayStrings->AddElement( overlayElement );
    } // if can project to screen

    const std::vector< Point3Df > &path0( pSeedpoints->GetTrackPath0(s) );
    const std::vector< Point3Df > &path1( pSeedpoints->GetTrackPath1(s) );
    for ( int4 t(0); t < path0.size(); ++t )
    {
      Point3Df pointWorld( path0[t].GetScaledAnIso( volUnits ) );
      RenderSphereWireframe( pointWorld, fTrackPointRadius );
    }    
    for ( int4 t(0); t < path1.size(); ++t )
    {
      Point3Df pointWorld( path1[t].GetScaledAnIso( volUnits ) );
      RenderSphereWireframe( pointWorld, fTrackPointRadius );
    }

  } // for all seedpoints

  // display the last two ends
  if ( pSeedpoints->GetNumClicks() > 0 )
  {
    int4 iLastSelection(pSeedpoints->GetNumClicks()-1);
    const std::vector< Point3Df > &path0( pSeedpoints->GetTrackPath0(iLastSelection) );
    const std::vector< Point3Df > &path1( pSeedpoints->GetTrackPath1(iLastSelection) );
    int2 iBackSpaceAlt( pSeedpoints->GetBackSpaceAlt() );

    if ( iBackSpaceAlt == 0 && path0.size() > 0 )
    {
      Point3Df startPointWorld( path0[path0.size()-1].GetScaledAnIso( volUnits ) ); 
      Point2D< int2 > screenPos;
      if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( startPointWorld, screenPos ) == true )
      {
        overlayElement.SetColor( Triple<float4>(1, 0, 0 ) );
        overlayElement.SetText( "<<" );
        overlayElement.SetRandomScreenPosition( Point2D< float4 >( screenPos.m_x+10, screenPos.m_y+5 ) );
        pOverlayStrings->AddElement( overlayElement );
      } // if can project to screen
    }
    
    if ( iBackSpaceAlt == 1 && path1.size() > 0 )
    {
      Point3Df endPointWorld( path1[path1.size()-1].GetScaledAnIso( volUnits ) ); 
      Point2D< int2 > screenPos;
      if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( endPointWorld, screenPos ) == true )
      {        
        overlayElement.SetColor( Triple<float4>(1, 0, 0 ) );
        overlayElement.SetText( "<<" );
        overlayElement.SetRandomScreenPosition( Point2D< float4 >( screenPos.m_x+10, screenPos.m_y+5 ) );
        pOverlayStrings->AddElement( overlayElement );
      } // if can project to screen
    }

    overlayElement.SetColor( Triple<float4>(1, 1, 1 ) );
  }


  // render the cursor seedpoint
  if ( pSeedpoints->GetCursorSeedpoint().IsSeedpointValid() )
  {
    const Point3Df pointWorld( pSeedpoints->GetCursorSeedpoint().GetSeedpoint().GetScaledAnIso( volUnits ) );
    glColor3f( 1, 1, 1 );
    RenderSphereWireframe( pointWorld, fRadius * 0.8F );

    // project to screen coordinates and display the seedpoint #
    Point2D< int2 > screenPos;
    if ( GetEnvironment().GetViewerType()->ConvertWorldToScreen( pointWorld, screenPos ) == true )
    {
      overlayElement.SetText( ToAscii( pSeedpoints->GetNumClicks()+1 ) );
      overlayElement.SetRandomScreenPosition( Point2D< float4 >( screenPos.m_x + 15, screenPos.m_y + 5 ) );
      pOverlayStrings->AddElement( overlayElement );
    } // if can project to screen
  } // if IsCursorSeedpointActive()

} // RenderSeedpoints()


/**
 * Local class declaration for the path of a circle.  It precomputes the path
 * in the constructor, then you can render the points using OpenGL in any major plane.
 */
class CirclePath
{
// Member Functions
public:

  /// Constructor
  CirclePath( const uint4 uNumSteps, const float4 fRadius )
  {
    for ( int i=0; i<uNumSteps; ++i )
    {
      const float4 fAngleRadians( M_PI * 2 * float4( i ) / float4( uNumSteps ) );
      m_circlePoints.push_back( Point2D< float4 >( fRadius * cos( fAngleRadians ), fRadius * sin( fAngleRadians ) ) );
    }
  } // CirclePath()

  /// Render the circle in the XY plane
  void RenderXY() const
  {
    glBegin(GL_LINE_LOOP);
    for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
      glVertex3f( it->m_x, it->m_y, 0 );
    glEnd();
  } // RenderXY()

  // Render the circle in the YZ plane
  void RenderYZ() const
  {
    glBegin(GL_LINE_LOOP);
    for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
      glVertex3f( 0, it->m_x, it->m_y );
    glEnd();
  } // RenderYZ()

  // Render the circle in the XZ plane
  void RenderXZ() const
  {
    glBegin(GL_LINE_LOOP);
    for ( std::vector< Point2D< float4 > >::const_iterator it( m_circlePoints.begin() ); it != m_circlePoints.end(); ++it )
      glVertex3f( it->m_x, 0, it->m_y );
    glEnd();
  } // RenderXZ()

// Data Members
private:

  /// The points on the circle
  std::vector< Point2D< float4 > > m_circlePoints;

}; // class CirclePath


/**
 * Render a wireframe sphere
 *
 * @param position the center position in world coordinates
 * @param fRadius  the radius of the sphere
 */
void vxRendererVessel3D::RenderSphereWireframe( const Point3Df & position, const float4 fRadius ) const
{
  // pre-compute a circle path
  CirclePath path( 36, fRadius );

  /// draw it three times for the three major axes
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  {
    glTranslatef( position.m_x, position.m_y, position.m_z );

    // draw in the z=0 plane
    glPushMatrix();
    path.RenderXY();
    glPopMatrix();

    // draw in the x=0 plane
    glPushMatrix();
    path.RenderYZ();
    glPopMatrix();

    // draw in the y=0 plane
    glPushMatrix();
    path.RenderXZ();
    glPopMatrix();
  }
  glPopMatrix();
} // RenderSphereWireframe()


// undefines
#undef FILE_REVISION


// $Log: vxRendererVessel3D.C,v $
// Revision 1.7  2007/08/09 19:37:56  romy
// Rendering or navigation arrow is disabled for Endoluminal views
//
// Revision 1.6  2007/08/04 04:13:55  romy
// oops !! not && bug fix
//
// Revision 1.5  2007/08/04 04:12:00  romy
// added RenderArrow method
//
// Revision 1.4  2007/08/04 03:38:36  romy
// added RenderArrow method
//
// Revision 1.3  2006/09/20 14:33:43  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.2.1  2005/08/08 18:56:22  frank
// spelling
//
// Revision 1.3  2005/06/08 15:02:17  vxconfig
// formatting
//
// Revision 1.2  2005/06/07 18:13:20  vxconfig
// removed the vessel array
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.14  2004/03/02 01:10:30  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.13.2.1  2004/02/25 15:00:25  frank
// formatting
//
// Revision 1.13  2004/01/14 14:18:15  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.12  2004/01/13 19:14:08  frank
// code review
//
// Revision 1.11  2004/01/05 20:23:15  frank
// formatting
//
// Revision 1.10  2003/12/22 20:23:48  frank
// removed dead code
//
// Revision 1.9  2003/12/22 19:44:08  wenli
// current "BackSpace" end is marked with "<<" instead of 'F' and 'B'
//
// Revision 1.8  2003/12/18 17:05:32  frank
// Optimizing redraws
//
// Revision 1.7  2003/12/08 21:57:40  wenli
// current 'BackSpace' end is marked as Red.
//
// Revision 1.6  2003/10/20 19:28:33  frank
// removed tortuosity display
//
// Revision 1.5  2003/10/13 20:21:44  wenli
// vxSeedPoint data structure are changed
//
// Revision 1.4  2003/10/07 21:08:10  wenli
// render the tracking seeds
//
// Revision 1.3  2003/08/14 18:12:15  frank
// Removed old viewer type
//
// Revision 1.2  2003/08/14 18:06:10  frank
// name change in vxBase
//
// Revision 1.1  2003/08/13 16:07:46  frank
// Specialized for vascular
//
// Revision 1.13  2003/08/12 20:20:24  frank
// Moved the vessel data structure and tools to vxBase
//
// Revision 1.12  2003/06/09 13:42:46  frank
// Added individual slice shadow renderer
//
// Revision 1.11  2003/05/29 14:05:29  frank
// Fixed debug messages
//
// Revision 1.10  2003/05/27 13:24:08  frank
// Removed dead includes
//
// Revision 1.9  2003/05/23 15:43:40  frank
// fixed oblique slice shadow
//
// Revision 1.8  2003/05/21 19:13:28  frank
// Fixed oblique plane for new oblique renderer data
//
// Revision 1.7  2003/05/19 18:46:35  frank
// showed slice shadows again
//
// Revision 1.6  2003/05/19 17:24:57  frank
// Removed unused return parameter
//
// Revision 1.5  2003/05/14 17:28:25  michael
// code review
//
// Revision 1.4  2003/05/14 12:02:35  frank
// Adding a new contour to the vessel section
//
// Revision 1.3  2003/05/12 20:07:41  frank
// Moved to alternate oblique viewer
//
// Revision 1.2  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.1  2003/05/08 19:59:27  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.43  2003/04/29 19:42:46  frank
// Cleaned up dead code
//
// Revision 1.42  2003/04/29 15:11:19  frank
// Added seedpoint numbers
//
// Revision 1.41  2003/04/24 19:21:11  frank
// Added back in cursor preview of seedpoint
//
// Revision 1.40  2003/04/22 19:25:46  wenli
// multi-seed points
//
// Revision 1.39  2003/04/15 17:45:42  frank
// Added preview of the vessel underneath the cursor
//
// Revision 1.38  2003/03/28 17:20:39  frank
// Changed exception to warning
//
// Revision 1.37  2003/03/25 17:32:02  frank
// Added virtual function to return class name until dynamic_cast problem across DLLs is resolved.
//
// Revision 1.36  2003/03/25 15:40:50  frank
// Added exception handling
//
// Revision 1.35  2003/03/24 15:13:43  frank
// Refactored code
//
// Revision 1.34  2003/03/21 15:50:13  frank
// Rounded for nearest contour
//
// Revision 1.33  2003/03/20 21:33:03  frank
// Enabled seedpoint antialiasing
//
// Revision 1.32  2003/03/19 18:19:56  frank
// Fixed up a crash
//
// Revision 1.31  2003/03/19 17:59:03  frank
// Converted to world coordinates before rendering
//
// Revision 1.30  2003/03/19 16:27:01  frank
// Moved individual seedpoints to a single class.
//
// Revision 1.29  2003/03/17 18:43:20  frank
// Rendered the seedpoints
//
// Revision 1.28  2003/03/13 20:34:35  frank
// Cleaned up arrow rendering
//
// Revision 1.27  2003/03/13 19:40:23  frank
// Working on camera rendering
//
// Revision 1.26  2003/03/13 14:12:22  frank
// Working on rendering the camera in 3D
//
// Revision 1.25  2003/03/11 18:43:44  geconomos
// Enabled anti-aliased line drawing.
//
// Revision 1.24  2003/02/18 22:32:56  frank
// cosmetics
//
// Revision 1.23  2003/02/18 21:51:43  frank
// Error handling for empty vessels.
//
// Revision 1.22  2003/01/28 15:09:28  frank
// Fixed bowtie problem
//
// Revision 1.21  2003/01/24 20:50:11  frank
// Fixed vessel overlays.
//
// Revision 1.20  2003/01/24 20:19:43  frank
// Overhauled the ribbon to contain more helpful data per scanline.
//
// Revision 1.19  2003/01/24 18:24:07  frank
// fixed debug messages
//
// Revision 1.18  2003/01/21 22:17:20  frank
// code review
//
// Revision 1.17  2003/01/14 16:17:15  dongqing
// Code review
//
// Revision 1.16  2002/12/16 23:55:23  frank
// Changed Quadrilateral to be a vector instead of four individual points.
//
// Revision 1.15  2002/11/25 22:57:06  frank
// Modified line width
//
// Revision 1.14  2002/11/22 01:47:06  frank
// thickened the centerline
//
// Revision 1.13  2002/11/21 19:34:41  frank
// Working on tortuosity glyph
//
// Revision 1.12  2002/11/21 00:51:51  frank
// Let you go to vessel views without a vessel without a crash
//
// Revision 1.11  2002/11/15 00:05:46  frank
// Rendered the curved/luminal shadow
//
// Revision 1.10  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.9  2002/11/13 20:11:55  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.8  2002/11/11 17:17:19  frank
// fixed comments
//
// Revision 1.7  2002/11/08 03:05:51  frank
// Rendered the entire vessel contour as points
//
// Revision 1.5  2002/11/05 19:53:22  frank
// Rendered oblique slice shadow
//
// Revision 1.4  2002/11/04 14:54:10  frank
// Working on blending colors more
//
// Revision 1.3  2002/11/01 19:58:20  frank
// Took out extra glyphs
//
// Revision 1.2  2002/10/31 21:59:25  frank
// Added VascularPreferences
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.4  2002/10/30 23:13:37  frank
// Cleaned up vessel rendering.
//
// Revision 1.3  2002/10/30 00:03:02  frank
// Working on rendering diameters.
//
// Revision 1.2  2002/10/29 18:27:28  frank
// Added the contour rendering
//
// Revision 1.1  2002/10/29 16:54:14  frank
// added vxRendererVessel3D
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererVessel3D.C,v 1.7 2007/08/09 19:37:56 romy Exp $
// $Id: vxRendererVessel3D.C,v 1.7 2007/08/09 19:37:56 romy Exp $
