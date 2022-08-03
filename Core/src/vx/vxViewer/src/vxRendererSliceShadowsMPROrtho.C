// $Id: vxRendererSliceShadowsMPROrtho.C,v 1.6 2007/03/02 21:19:53 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRendererSliceShadowsMPROrtho.h"
#include "vxEnvironmentList.h"
#include "vxEnvironment.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxViewerTypeMPRCurved.h"
#include "vxViewerTypeMPRCurvedLuminal.h"
#include "vxRibbon.h"
#include "Intersection.h"
#include "Line.h"
#include "Plane.h"

// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Constructor
 *
 * @param environment the environment
 */
vxRendererSliceShadowsMPROrtho::vxRendererSliceShadowsMPROrtho( vxEnvironment & environment ) : vxRendererSliceShadows( environment )
{
} // vxRendererSliceShadowsMPROrtho()


/**
 * Renders the slice shadows
 */
void vxRendererSliceShadowsMPROrtho::Render()
{
  // early return if no rendering necessary
  if ( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_NONE )
    return;

  if( GetEnvironment().GetRenderingMode()->GetSliceShadowMode() == vxRenderingMode::SHADOWS_ACTIVE )
  {
    const vxEnvironment *  pActiveEnvironment = vxEnvironment::GetActiveEnvironment();
    if( pActiveEnvironment && pActiveEnvironment != &GetEnvironment() )
    {
      vxViewerType * pViewerType = pActiveEnvironment->GetViewerType();
      if( dynamic_cast< vxViewerTypeMPROrtho* >( pViewerType ) || dynamic_cast< vxViewerTypeMPROblique* >( pViewerType ) )
      {    
        RenderPlane( *pActiveEnvironment );
      }
      else if( dynamic_cast< vxViewerTypeMPRCurved* >( pViewerType ) || dynamic_cast< vxViewerTypeMPRCurvedLuminal* >( pViewerType ) )
      {
        RenderCurvedPlane( *pActiveEnvironment );
      }
    }
  }
  else
  {
    vxEnvironmentList * pEnvironmentList( reinterpret_cast< vxEnvironmentList * >( GetEnvironment().GetElement( vxIDs::EnvironmentList )));
    for( vxEnvironmentList::EnvironmentList::iterator iter = pEnvironmentList->GetEnvironmentList().begin(); 
         iter != pEnvironmentList->GetEnvironmentList().end(); ++iter )
    {
      vxEnvironment * pOtherEnvironment( * iter );
      
      vxViewerType * pViewerType = pOtherEnvironment->GetViewerType();
      
      if( dynamic_cast< vxViewerTypeMPROrtho* >( pViewerType ) || dynamic_cast< vxViewerTypeMPROblique* >( pViewerType ) )
      {    
        RenderPlane( *pOtherEnvironment );
      }
      else if( dynamic_cast< vxViewerTypeMPRCurved* >( pViewerType ) || dynamic_cast< vxViewerTypeMPRCurvedLuminal* >( pViewerType ) )
      {
        RenderCurvedPlane( *pOtherEnvironment );
      }
    }
  }
} // Render()


/**
 * Renders the projection of an oblique or ortho plane.
 *
 * @param   otherEnvironment       other environment
 */
void vxRendererSliceShadowsMPROrtho::RenderPlane( const vxEnvironment & otherEnvironment )
{
  vxUtils::SliceShadowIntersections intersections;
  if( vxUtils::GetIntersectionsWorld( &otherEnvironment, & GetEnvironment(), intersections ))
  {
    // draw line between points
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glEnable( GL_LINE_SMOOTH );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

    const PixelRGBA< uint1 > shadowColor = GetSliceShadowColor( otherEnvironment );

    if( GetEnvironment().GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace )
    {
      Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

      vxViewerType * pViewerType = GetEnvironment().GetViewerType();

      Point2D< int2 > firstlineStart;
      pViewerType->ConvertWorldToScreen( intersections.m_firstline.first, firstlineStart );
      
      Point2D< int2 > firstlineEnd;
      pViewerType->ConvertWorldToScreen( intersections.m_firstline.second, firstlineEnd );
      
      Point2D< int2 > secondlineStart;
      pViewerType->ConvertWorldToScreen( intersections.m_secondline.first, secondlineStart );
      
      Point2D< int2 > secondlineEnd;
      pViewerType->ConvertWorldToScreen( intersections.m_secondline.second, secondlineEnd );

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();


      glColor4ub( shadowColor.R(), shadowColor.G(), shadowColor.B(), shadowColor.A() * 0.30F );
      glBegin( GL_QUADS  );
        glVertex2i( firstlineStart.m_x,   firstlineStart.m_y  );
        glVertex2i( firstlineEnd.m_x,     firstlineEnd.m_y    );
        glVertex2i( secondlineEnd.m_x,    secondlineEnd.m_y   );
        glVertex2i( secondlineStart.m_x,  secondlineStart.m_y );
      glEnd();


      glColor4ub( shadowColor.R(), shadowColor.G(), shadowColor.B(), shadowColor.A() * 0.60F );
      glBegin( GL_LINES);
        glVertex2i( firstlineStart.m_x,   firstlineStart.m_y  );
        glVertex2i( firstlineEnd.m_x,     firstlineEnd.m_y    );
        glVertex2i( secondlineStart.m_x,  secondlineStart.m_y );
        glVertex2i( secondlineEnd.m_x,    secondlineEnd.m_y   );
      glEnd();

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();  
    }
    else
    {
      glColor4ub( shadowColor.R(), shadowColor.G(), shadowColor.B(), shadowColor.A() * 0.30F );
      glBegin( GL_QUADS  );

        glVertex3f( intersections.m_firstline.first.m_x,  intersections.m_firstline.first.m_y,  intersections.m_firstline.first.m_z );
        glVertex3f( intersections.m_firstline.second.m_x, intersections.m_firstline.second.m_y, intersections.m_firstline.second.m_z );
        glVertex3f( intersections.m_secondline.second.m_x, intersections.m_secondline.second.m_y, intersections.m_secondline.second.m_z );
        glVertex3f( intersections.m_secondline.first.m_x,  intersections.m_secondline.first.m_y,  intersections.m_secondline.first.m_z );

      glEnd();


      glColor4ub( shadowColor.R(), shadowColor.G(), shadowColor.B(), shadowColor.A() * 0.60F );
      glBegin( GL_LINES );
        glVertex3f( intersections.m_firstline.first.m_x,  intersections.m_firstline.first.m_y,  intersections.m_firstline.first.m_z );
        glVertex3f( intersections.m_firstline.second.m_x, intersections.m_firstline.second.m_y, intersections.m_firstline.second.m_z );
        glVertex3f( intersections.m_secondline.first.m_x,  intersections.m_secondline.first.m_y,  intersections.m_secondline.first.m_z );
        glVertex3f( intersections.m_secondline.second.m_x, intersections.m_secondline.second.m_y, intersections.m_secondline.second.m_z );
      glEnd();
    }
  }

  glPopAttrib();

} // RenderPlane( otherEnvironment )


/**
 * Renders the projection of a curved plane.
 *
 * @param   otherEnvironment       other environment
 */
void vxRendererSliceShadowsMPROrtho::RenderCurvedPlane( const vxEnvironment & otherEnvironment )
{
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( otherEnvironment.GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
    return;

  vxPlaneOblique<float4>* pPlane = NULL;

  if( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType() ) )
    pPlane = GetEnvironment().GetPlaneOrtho();

  else if( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType() ) )
    pPlane = GetEnvironment().GetPlaneOblique();
  
  else
    throw ex::UnsupportedDataTypeException( LogRec( "Inavlid viewer type", "vxRendererSliceShadowsMPROrtho", "RenderCurvedPlane" ));

  Point3Df pointOnPlane;
  if( !pPlane->GetIntersection( Point3Df( 0, 0, 0 ), Normal<float4>( 1, 1, 1 ), pointOnPlane ) )
    return;

  Normal< float4 > planeNormal = pPlane->GetNormal();

  // setup the drawing
  glPushAttrib( GL_ALL_ATTRIB_BITS );
  glEnable( GL_LINE_SMOOTH );
  glLineWidth( 1.0F );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  
  const PixelRGBA< uint1 > shadowColor = GetSliceShadowColor( otherEnvironment );
  glColor4ub( shadowColor.R(), shadowColor.G(), shadowColor.B(), shadowColor.A() );


  glDisable( GL_DEPTH );


  bool bScreenSpace = GetEnvironment().GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;
  if( bScreenSpace )
  {
    Rect<float4> viewPortRect(0.0F, 0.0F, GetEnvironment().GetWinRect().GetViewportSize().first, GetEnvironment().GetWinRect().GetViewportSize().second);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0F, viewPortRect.Width(), 0.0F, viewPortRect.Height());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  /// loop over curved plane scanlines
  glBegin( GL_LINE_STRIP );
  const std::vector< vxRibbon::ScanlineData > & scanlines = pRibbon->GetScanlineData();
  for ( int4 i=0; i<scanlines.size(); ++i )
  {

    // check for a valid scanline
    const vxRibbon::ScanlineData & scanline = scanlines[ i ];
    if ( scanline.m_fSectionIndex >= 0 )
    {

      // get intersection with the plane
      Point3Df intersectionPoint;
      if ( Intersection::ComputeIntersection( Line( scanline.m_leftPoint, scanline.m_rightPoint ),
                                              Plane( pointOnPlane, planeNormal ),
                                              intersectionPoint ) )
      {
        if( bScreenSpace )
        {
          Point2D< int2 > pos;
          GetEnvironment().GetViewerType()->ConvertWorldToScreen( intersectionPoint, pos );
          glVertex2i( pos.m_x, pos.m_y );
        }
        else
        {
          glVertex3f( intersectionPoint.m_x, intersectionPoint.m_y, intersectionPoint.m_z );
        }
        

      } // if an intersection with the plane
      
    } // if a valid scanline

  } // loop over curved plane scanlines
  glEnd();

  if( bScreenSpace )
  {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();  
  }
  glPopAttrib();

} // RenderCurvedPlane( const vxEnvironment & otherEnvironment )


// $Log: vxRendererSliceShadowsMPROrtho.C,v $
// Revision 1.6  2007/03/02 21:19:53  geconomos
// cleanup
//
// Revision 1.5  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.4  2006/11/14 16:28:37  geconomos
// updated to check the the current viewer isn't the active viewer
//
// Revision 1.3  2006/10/23 01:38:54  geconomos
// code cleanup
//
// Revision 1.2  2006/10/05 17:03:04  geconomos
// added display of thin slab
//
// Revision 1.1  2006/10/03 20:04:16  geconomos
// continued reworking of slice shadows
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererSliceShadowsMPROrtho.C,v 1.6 2007/03/02 21:19:53 geconomos Exp $
// $Id: vxRendererSliceShadowsMPROrtho.C,v 1.6 2007/03/02 21:19:53 geconomos Exp $