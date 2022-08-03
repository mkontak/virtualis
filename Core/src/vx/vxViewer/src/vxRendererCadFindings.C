// $Id: vxRendererCadFindings.C,v 1.3 2007/04/17 20:06:02 romy Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


// includes
#include "stdafx.h"
#include "vxCadFindings.h"
#include "vxEnvironment.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxUtils.h"
#include "vxRendererCadFindings.h"


// defines
#define FILE_REVISION "$Revision: 1.3 $"


/**
* Constructor
* @param environment the environement
*/ 
vxRendererCadFindings::vxRendererCadFindings( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererCadFindings()


/**
* Render the Cad Finding areas
*/
void vxRendererCadFindings::Render()
{  
  //works for MPR ortho & Oblique views
  if((( dynamic_cast< vxViewerTypeMPROrtho * >( GetEnvironment().GetViewerType())   == NULL )  &&
    (   dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType()) == NULL )) ||
    (   GetEnvironment().GetCadFindings() == NULL ) || ( GetEnvironment().GetCadFindings()->GetSelectedIndex() < 0  ))  
    return;

  try
  {
    const Triplef fUnits( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    const float4 distance( GetEnvironment().GetPlaneOrtho()->GetDistance()) ;
    uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );

    //Display all the findings which falls in the same slice.
    for ( int4 index=0; index < GetEnvironment().GetCadFindings()->GetCount(); index++ )
    {
      //Draw the cad findings.
      const vxCAD::vxCadFinding & finding = GetEnvironment().GetCadFindings()->GetFinding( index );
      Point<float4> cadCentroid           = Point3Df( finding.GetCentroidMM().GetVector() );

      //If the units are in voxels, scale it to Voxel Units
      if ( finding.GetCentroidMM().GetUnits() == vxCAD::vxCadVector::VOXELS )
        cadCentroid.ScaleAnIso( fUnits );

      //Show the slices which falls inside a bounding box 
      Box<int4>  boundingBox = GetBoundingBox( cadCentroid );
      float minDistance = boundingBox.GetMinPoint()[uOrientation];
      float maxDistance = boundingBox.GetMaxPoint()[uOrientation];

      if ( distance >= minDistance &&  distance <= maxDistance  )
      {
        //Draw Circles if they are in the same slice && if the finding is visible
        //For an Oblique plane, Set the Orienation to axial 
        if ( dynamic_cast< vxViewerTypeMPROblique * >( GetEnvironment().GetViewerType())  != NULL ) 
          uOrientation = vxUtils::AXIAL;

        //Draw the circles , if selected index, draw in a different color
        if ( GetEnvironment().GetCadFindings()->GetSelectedIndex()  == index )
          DrawCircle( cadCentroid, PixelRGBA<uint1>( 255,0,0,255 ), uOrientation );
        else
          DrawCircle( cadCentroid , PixelRGBA<uint1>( 255,255,0,255 ), uOrientation );

        //Display the Index Number on top of the Circle.
        Point2D<int2> screenCoord( 0, 0 );
        GetEnvironment().GetViewerType()->ConvertWorldToScreen( cadCentroid, screenCoord );
        vxOverlayStrings::vxOverlayElement overlayElement( ToAscii( index + 1 ), vxOverlayStrings::vxOverlayElement::Random );
        overlayElement.SetRandomScreenPosition( Point2D<float4>( screenCoord.m_x + 5, screenCoord.m_y + 5 ) );
        (dynamic_cast<vxShareableObject<vxOverlayStrings> *>((GetEnvironment().GetElement(vxIDs::OverlayStrings))))->AddElement( overlayElement );

      }//if they are in the same slice
    } // for each finding
  } // try
  catch (...)
  {
    throw ex::VException( LogRec( "failed to render", "vxRendererCadFindings", "Render" ) );
  }
} // Render()


/*
 * Gets the bounding box around a centroid
 * @param centroid const Point3D<float4> &
 * @returns Box<int4>
 */
Box<int4> vxRendererCadFindings::GetBoundingBox( const Point3D<float4> & centroid ) const
{
  float4 fBoxRadius = 2.0F;
  
  Vector3Df radiusVector = Vector3Df( fBoxRadius, fBoxRadius, fBoxRadius );
  Point3Di pointUL( centroid - radiusVector );
  Point3Di pointLR( centroid + radiusVector );
  Box<int4> bounds( pointUL.m_x, pointUL.m_y, pointUL.m_z, pointLR.m_x, pointLR.m_y, pointLR.m_z );

  return bounds;
}//GetBoundingBox( const Point3D<float4> & centroid ) const


/*
* Draws a circle on the plane
* @param fPosition Point3Df 
* @param color PixelRGBA<uint1> 
* @param uOrientation uint4  
*/
void vxRendererCadFindings::DrawCircle( Point3Df & fPosition,  PixelRGBA<uint1> & color , uint4 uOrientation )const
{
  float4 radius = 8.0F;
  GLdouble X, Y, Z;

  for (int j(0); j<2; j++)
  {
    glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);
    glBegin(GL_LINE_STRIP);			

    for (int angle(0); angle<=360; angle++)
    {		
      if ( uOrientation == vxUtils::AXIAL )
      {
        X = fPosition.m_x + (radius*(float)sin((double)angle));
        Y = fPosition.m_y + (radius*(float)cos((double)angle));
        Z = fPosition.m_z + (radius*(float)sin((double)angle));;
      }
      else if ( uOrientation == vxUtils::CORONAL )
      {
        X = fPosition.m_x + (radius*(float)cos((double)angle));
        Y = fPosition.m_y + (radius*(float)sin((double)angle));
        Z = fPosition.m_z + (radius*(float)sin((double)angle));;
      }
      else if ( uOrientation == vxUtils::SAGITTAL )
      {
        X = fPosition.m_x + (radius*(float)sin((double)angle));
        Y = fPosition.m_y + (radius*(float)sin((double)angle));
        Z = fPosition.m_z + (radius*(float)cos((double)angle));;
      }

      glVertex3d( X, Y, Z);
    }// for (int angle(0); angle<=360; angle++)
    glEnd();
  } 
} //DrawCircle( Point3Df & fPosition,  PixelRGBA<uint1> & color , uint4 uOrientation )const




//undefines
#undef FILE_REVISION


// $Log: vxRendererCadFindings.C,v $
// Revision 1.3  2007/04/17 20:06:02  romy
// added code to display the cad findings in nearby planes also.
//
// Revision 1.2  2007/04/12 19:19:42  romy
// Added code to display the Findings in Oblique slice
//
// Revision 1.1  2007/04/11 20:50:27  romy
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererCadFindings.C,v 1.3 2007/04/17 20:06:02 romy Exp $
// $Id: vxRendererCadFindings.C,v 1.3 2007/04/17 20:06:02 romy Exp $

