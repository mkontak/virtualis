// $Id: vxRendererMesh.C,v 1.7 2008/05/05 19:17:33 cqian Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

/**
 * Renders a 3D colored mesh.
 */


// includes
#include "stdafx.h"
#include "vxRendererMesh.h"
#include "vxEnvironment.h"
#include "vxViewerType3D.h"
#include "vxRendererUtils.h"
#include "vxElementString.h"


/**
 * Default constructor
 *
 * @param environment the environment
 */
vxRendererMesh::vxRendererMesh( vxEnvironment & environment )
: vxRenderer( environment )
{
} // vxRendererMesh(environment)


/**
 * Destructor
 */
vxRendererMesh::~vxRendererMesh()
{
} // ~vxRendererMesh()


/** 
 * Has something changed that makes rendering necessary?
 * @return whether rendering is necessary
 */
bool vxRendererMesh::IsRedrawNecessary()
{

  bool bRedrawNecessary( false );

  vxMesh * pMesh = dynamic_cast< vxMesh * >( GetEnvironment().GetElement( vxIDs::Mesh ) );
  if ( pMesh != NULL )
  {
    bRedrawNecessary |= GetEnvironment().IsModified( vxIDs::Mesh );
    bRedrawNecessary |= GetEnvironment().IsModified( vxEnvironment::WIN_RECT );
    bRedrawNecessary |= GetEnvironment().IsModified( vxEnvironment::CAMERA );
  }

  return bRedrawNecessary;

} // IsRedrawNecessary()


/**
 * Render the vessel using OpenGL
 */
void vxRendererMesh::Render()
{
  ////////// Testing only - remove for production use!
  //static Triplei savedVolDim = GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim();
  //if ( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim() != savedVolDim || GetEnvironment().GetElement( vxIDs::Mesh ) == NULL )
  //  GetEnvironment().AddElement( vxIDs::Mesh, GetSampleMesh() );
  ////////// Testing only - remove for production use!

  // make sure this is a 3D view
  if ( ! dynamic_cast< vxViewerType3D * >( GetEnvironment().GetViewerType() ) )
    return;

  // make sure we have something to render
  vxMesh * pMesh = dynamic_cast< vxMesh * >( GetEnvironment().GetElement( vxIDs::Mesh ) );
  if ( pMesh == NULL || pMesh->GetMesh().size() < 2)
    return;

  // get the center to put the mesh
  Triplef volumeCenter = GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() / 2;

  // save the OpenGL state
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  {
    // setup OpenGL for lighting
    glLineWidth(1.0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    GLfloat mat_specular[] = { .72F, .8F, .93F, 1.0F };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


     // possibly create a display list
    if ( GetEnvironment().IsModified( vxIDs::Mesh ) || ! m_displayList.GetValid() )
    {
      m_displayList.BeginDefinition();

      // first, loop over the strips
      for ( int4 stripIndex = 0; stripIndex < static_cast< int4 >( pMesh->GetMesh().size() ) - 1; stripIndex++ )
      {
        // get this column and the next
        std::vector< vxColoredVertex > column0 = pMesh->GetMesh()[ stripIndex + 0 ];
        std::vector< vxColoredVertex > column1 = pMesh->GetMesh()[ stripIndex + 1 ];
    
        glBegin( GL_QUAD_STRIP );
        for ( int4 quad = 0; quad < column0.size(); quad++ )
        {
          // first point in this row
          vxColoredVertex left = column0[ quad ];
          glColor4ub( left.GetColor().R(), left.GetColor().G(), left.GetColor().B(), left.GetColor().A() );
          glNormal3f( left.GetNormal().X(), left.GetNormal().Y(), left.GetNormal().Z() );
          vxRendererUtils::GlVertex( left.GetPosition() ); 
          // second point in this row
          vxColoredVertex right = column1[ quad ];
          glColor4ub( right.GetColor().R(), right.GetColor().G(), right.GetColor().B(), right.GetColor().A() );
          glNormal3f( right.GetNormal().X(), right.GetNormal().Y(), right.GetNormal().Z() );
          vxRendererUtils::GlVertex( right.GetPosition() ); 
        } // foreach quad vertex in strip
        glEnd();
      } // foreach strip
    
      m_displayList.EndDefinition();
    } // if mesh has changed or display list is uninitialized

    // just call the display list, it's ready for use
    else
    {
      m_displayList.CallList();
    }

  } // glPushAttrib()
  glPopAttrib();
	
  // put the string title
  vxShareableObject<vxOverlayStrings> * pOverlayStrings = dynamic_cast<vxShareableObject<vxOverlayStrings> *>( GetEnvironment().GetElement( vxIDs::OverlayStrings ) );
  vxElementString * pElementString = dynamic_cast<vxElementString *>( GetEnvironment().GetElement( vxIDs::TitleString ) );
  pOverlayStrings->AddElement( vxOverlayStrings::vxOverlayElement( pElementString->GetValue(), vxOverlayStrings::vxOverlayElement::UpperMiddle ) );

} // Render()


/**
 * Creates a sample mesh
 */
vxMesh * vxRendererMesh::GetSampleMesh() const
{

  static vxMesh mesh;

  mesh.GetMesh().clear();

  Triplef volumeCenter = GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim() / 2;
  for ( int phi=0; phi<365; phi+=5 )
  {
    std::vector< vxColoredVertex > oneColumn;
    for ( int theta=0; theta<180; theta+=5 )
    {
      vxColoredVertex vertex;
      vertex.SetPosition( Point3Df(
        cos( phi * M_PI / 180 ) * sin( theta * M_PI / 180 ),
        sin( phi * M_PI / 180 ) * sin( theta * M_PI / 180 ),
        cos( theta * M_PI / 180 ) ) );
      vertex.SetNormal( static_cast< Normal3Df >( vertex.GetPosition() ) );
      vertex.SetPosition( vertex.GetPosition() );//.GetScaledAnIso( volumeCenter ) + static_cast< Vector3Df >( volumeCenter ) );
      vertex.SetColor( PixelRGBA< uint1 >( 255 * sin( 2 * phi * M_PI / 180 ), 255 * cos( 2 * theta * M_PI / 180 ), 255, 128 ) );
      oneColumn.push_back( vertex );
    }
    mesh.GetMesh().push_back( oneColumn );
  }

  return & mesh;

} // GetPath()


// undefines
#undef FILE_REVISION


// $Log: vxRendererMesh.C,v $
// Revision 1.7  2008/05/05 19:17:33  cqian
// add the titleString
//
// Revision 1.6  2008/04/02 16:01:24  cqian
// check in
//
// Revision 1.5  2008/03/20 16:52:47  cqian
// chang the rendering object
//
// Revision 1.4  2007/08/09 16:59:54  frank
// moved display list class to vxViewer
//
// Revision 1.3  2007/07/23 14:37:09  geconomos
// corrected underflow error while rendering mesh with zero strips
//
// Revision 1.2  2007/07/20 19:57:47  cqian
// check in for testing
//
// Revision 1.1  2007/07/14 01:23:12  frank
// Added mesh renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRendererMesh.C,v 1.7 2008/05/05 19:17:33 cqian Exp $
// $Id: vxRendererMesh.C,v 1.7 2008/05/05 19:17:33 cqian Exp $
