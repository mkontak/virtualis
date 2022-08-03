// $Id: vxAnnotationPoint.C,v 1.9 2006/11/24 19:17:49 romy Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// includes
#include "stdafx.h"
#include "vxAnnotationPoint.h"
#include "vxViewerType.h"
#include "vxMouseCursor.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "vxSerializer.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Default constructor
 */
vxAnnotationPoint::vxAnnotationPoint()
//*******************************************************************
{
  CommonInit();
	//SetDrawHandles( false );
}


/**
 * Copy Constructor
 */
vxAnnotationPoint::vxAnnotationPoint( const vxAnnotationPoint & other ) : vxAnnotation( other )		
{
	CommonInit();

	m_fRadius = other.m_fRadius;
}//Copy Constructor


/**
 * Clone method
 */
vxAnnotationPoint * vxAnnotationPoint::Clone() const
{
	return new vxAnnotationPoint( * this ); 
} //Clone method


/**
 * CommonInit
 */
void vxAnnotationPoint::CommonInit()
//*******************************************************************
{
  SetRadius(1.0F);
  SetMouseCursor("CURSOR_POINT");
  SetImage(ResourceImage::FromResource("ANNOTATION_POINT"));
} // CommonInit()


/**
 * Reset
 */
void vxAnnotationPoint::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // Reset()


/**
 * render the annotation
 * @param viewer type
 */
void vxAnnotationPoint::Render(vxEnvironment & environment)
//*******************************************************************
{
  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(* this) && GetNumHandles() == 1 && viewerType.IsVisible(* this))
  {
    const uint4 uSphereLongitudes(10);
    const uint4 uSphereLatitudes(4);

    GLUquadricObj * pQuadric = gluNewQuadric();
    gluQuadricOrientation(pQuadric, GLU_OUTSIDE);

    GLfloat mat_specular[] = { .72F, .8F, .93F, 1.0F };

    glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.0, -1);

    // get the color for the sphere
    PixelRGBA<uint1> color;
    if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() == 0)
      color = GetColorSelected();
    else
      color = GetColor();
    glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // transform to the proper position
    Point<float4> handlePos;
    switch (viewerType.GetRenderCoordinateSystem())
    {
    case vxViewerType::eWorldSpace:
      handlePos = GetHandle(0).GetPosition();
      break;
    case vxViewerType::eScreenSpace:
      {
        Point2D<int2> screenPos;
        if (viewerType.ConvertWorldToScreen(GetHandle(0).GetPosition(), screenPos)  == false)
          throw ex::VException(LogRec("Unable to convert world to screen", "vxAnnotationPoint", "Render"));
        handlePos = Point3Df(screenPos.m_x, screenPos.m_y, 0.0F);
      }
      break;
    default:
      throw ex::IllegalArgumentException(LogRec("unknown coordinate system", "vxAnnotationPoint", "Render"));
      break;
    }

    glTranslatef(handlePos[0], handlePos[1], handlePos[2]);
    gluQuadricDrawStyle(pQuadric, (GLenum)GLU_FILL);
    gluQuadricOrientation(pQuadric, (GLenum)GLU_OUTSIDE);
    gluQuadricNormals(pQuadric, (GLenum)GLU_SMOOTH);
    
    // render the sphere with the proper size
    switch (viewerType.GetRenderCoordinateSystem())
    {
    case vxViewerType::eWorldSpace:
      gluSphere(pQuadric, GetHandle(0).GetHandleWorldSize()*GetRadius(), uSphereLongitudes, uSphereLatitudes);
      break;
    case vxViewerType::eScreenSpace:
      gluSphere(pQuadric, GetHandle(0).GetHandleScreenSize()*GetRadius(), uSphereLongitudes, uSphereLatitudes);
      break;
    default:
      break;
    }
    glPopMatrix();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);

    gluDeleteQuadric(pQuadric);

    // call parent class
    vxAnnotation::Render(environment);
  } // annotation supported

  return;
} // Render()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationPoint::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  // do local scope stuff
  float4 fRadius(GetRadius());
  vxSerializer::SetAttribute(element, "version", "2.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(fRadius), "radius"), fRadius);
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationPoint::FromXml(vxDOMElement & element)
//*******************************************************************
{
  if (element.IsNull()) 
    return false;

  bool bSuccess(true);

  // call parent class
  bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  // do local scope stuff
  std::string sVersion("0.0");
  bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);
  if (atof(sVersion.c_str()) == 2.0)
  {
    float4 fRadius(2.0F);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(fRadius), "radius"), fRadius);
    SetRadius(fRadius);
  }
  else
  {
    LogDbg("Unknown version number for XML", "vxAnnotationPoint", "FromXml()");
  }

  return bSuccess;
} // FromXml()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationPoint.C,v $
// Revision 1.9  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.8  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.7  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.6  2006/08/03 21:33:30  romy
// removed the AnnotationAdjust tool
//
// Revision 1.5  2006/08/02 21:39:55  romy
// adjusted point radius
//
// Revision 1.4  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/08 14:10:03  frank
// michael fixed the problem with opacity not being obeyed in some cases
//
// Revision 1.2  2005/02/08 00:01:34  michael
// Fixed the drawing to account for the actual set opacity (enable blending)
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.26  2004/01/23 03:51:32  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.25  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.24  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.23  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.22  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.21  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.20  2003/03/17 18:34:39  michael
// set radius consistently to 2
//
// Revision 1.19  2003/03/07 18:18:23  frank
// simplified logic
//
// Revision 1.18  2003/03/05 12:51:39  frank
// Extended interface to include the ability to render in either screen or world space.
//
// Revision 1.17  2003/03/04 01:43:06  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.16  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.15  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.14  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.13  2003/02/24 14:58:05  frank
// Added local coordinate transformation for non-OpenGL projections
//
// Revision 1.12  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.11  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.10  2003/01/19 21:07:08  michael
// added box annotation
//
// Revision 1.9  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.8  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.7  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.6  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.5  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.4  2003/01/13 23:20:33  michael
// made first annotation to work in new structure (vxAnnotations)
//
// Revision 1.3  2003/01/13 14:43:35  geconomos
// Did the following:
//   (1) Added mouse cursor handling.
//   (2) Set the active annotation in AddAnnotation().
//
// Revision 1.2  2003/01/09 14:55:44  michael
// some add-ons to the new annotation structure
//
// Revision 1.1  2003/01/06 17:12:18  michael
// Added initial version of annotations
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationPoint.C,v 1.9 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationPoint.C,v 1.9 2006/11/24 19:17:49 romy Exp $
