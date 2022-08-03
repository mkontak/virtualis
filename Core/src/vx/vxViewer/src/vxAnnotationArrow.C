// $Id: vxAnnotationArrow.C,v 1.10 2006/11/24 19:17:49 romy Exp $
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
#include "vxAnnotationArrow.h"
#include "vxViewerType.h"
#include "vxMouseCursor.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "vxRendererOverlay.h"
#include "Matrix.h"
#include "vxSerializer.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.10 $"

//const float4 vxAnnotationArrow::m_fScreenLength(50.0F);

const float4 vxAnnotationArrow::m_fScreenLength( 30.0F);

/**
 * Default constructor
 */
vxAnnotationArrow::vxAnnotationArrow()
//*******************************************************************
{
  CommonInit();
} // default constructor


/**
 * CommonInit
 */
void vxAnnotationArrow::CommonInit()
//*******************************************************************
{
  SetMouseCursor("CURSOR_ARROW");
  SetImage(ResourceImage::FromResource("ANNOTATION_ARROW"));
} // CommonInit()

/**
 * copy constructor
 */
vxAnnotationArrow::vxAnnotationArrow( const vxAnnotationArrow & other ) : vxAnnotation( other )
{
	//m_fScreenLength = other.m_fScreenLength;

  CommonInit();
}//copy constructor

/**
 * Clone method
 */
vxAnnotationArrow *  vxAnnotationArrow::Clone()const
{
	return new vxAnnotationArrow( *this );
}//Clone



/**
 * Reset
 */
void vxAnnotationArrow::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // Resetn()


/**
 * Create a valid vector that is not 0
 * @param handle0
 * @param handle1
 * @return vector
 */
Vector<float4> vxAnnotationArrow::CreateValidVector(const vxHandle & handle0, 
                                                    const vxHandle & handle1)
//*******************************************************************
{
  Vector<float4> arrow;

  if ((handle0.GetPosition() - handle1.GetPosition()).GetLength() < 0.0001F)
  {
    // this is somewhat a workaround so that rotating the arrow in Render() works
    Normal<float4> view(GetEnvironment()->GetCamera()->GetViewDirection());
    vxCamera camera(*GetEnvironment()->GetCamera());
    if (view.GetMaxComponentCoord() != 1)
    {
      arrow = (Vector<float4>)GetEnvironment()->GetCamera()->GetViewRight();
    }
    else
    {
      arrow = Vector<float4>(-1, 0, 0);
    }
  } // same handle?
  else
  {
    arrow = handle0.GetPosition() - handle1.GetPosition();
  }

  arrow.Normalize();
  
  return arrow;
} // CreateValidVector()


/**
 * Get the position of a given handle
 * @param index of handle
 * @param position of handle
 */
void vxAnnotationArrow::SetHandle(const uint2 uHandleIndex, 
                                  const vxHandle & handle)
//*******************************************************************
{
  if (GetNumHandles() == 2)
  {
    if (uHandleIndex == 0)
    {
      Vector<float4> vector(CreateValidVector(handle, vxAnnotation::GetHandle(1)));
      vector.Normalize();
      vxAnnotation::SetHandle(0, handle);
      vxHandle tmpHandle(vxAnnotation::GetHandle(1));
      tmpHandle.SetPosition(handle.GetPosition() - vector * GetScreenLength());
      vxAnnotation::SetHandle(1, tmpHandle);
    }
    else
    {
      Vector<float4> vector(CreateValidVector(vxAnnotation::GetHandle(0), handle));
      vector.Normalize();
      vxHandle tmpHandle(vxAnnotation::GetHandle(1));
      tmpHandle.SetPosition(vxAnnotation::GetHandle(0).GetPosition() - vector * GetScreenLength());
      vxAnnotation::SetHandle(1, tmpHandle);
    } // handle index not 0?
  } // two handels?
  else
  {
    vxAnnotation::SetHandle(uHandleIndex, handle);
  }
} // SetHandle()


/**
 * Render the annotation
 * @param viewer type
 */
void vxAnnotationArrow::Render(vxEnvironment & environment)
//*******************************************************************
{
  SetEnvironment(&environment);

  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(*this))
  {
    if (GetNumHandles() == 2)
    {
      if (viewerType.IsVisible(*this))
      {
        Point3Df vPoints[2];
        vPoints[0] = GetHandle(0).GetPosition();
        vPoints[1] = GetHandle(1).GetPosition();

        if ((vPoints[0] - vPoints[1]).GetLength() < 0.0001F)
        {
          return;
        }

        const uint4 uSphereLongitudes(10);
        const uint4 uSphereLatitudes(4);

        //glPushAttrib();

        GLUquadricObj * pQuadric = gluNewQuadric();
        gluQuadricOrientation(pQuadric, GLU_OUTSIDE);

        GLfloat mat_specular[] = { .72F, .8F, .93F, 1.0F };

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glShadeModel(GL_SMOOTH);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.0, -1);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // get the color for the sphere
        PixelRGBA<uint1> color(GetColor());
        if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() == 0 || GetSelectedHandleIndex() == 1)
        {
          color = GetColorSelected();
        }
        glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

        // get the handle points
        Point<float4> handlePos[2];
        switch (viewerType.GetRenderCoordinateSystem())
        {
        case vxViewerType::eWorldSpace:
          handlePos[0] = vPoints[0];
          handlePos[1] = vPoints[1];
          break;
        case vxViewerType::eScreenSpace:
          for (int i=0; i<2; ++i)
          {
            Point2D<int2> screenPos;
            viewerType.ConvertWorldToScreen(vPoints[i], screenPos);
            handlePos[i].m_x = screenPos.m_x;
            handlePos[i].m_y = screenPos.m_y;
            handlePos[i].m_z = 0;         
          }
          break;
        default:
          throw ex::IllegalArgumentException(LogRec("bad render coordinate system", "vxAnnotationArrow", "Render"));
          break;
        }

        // determine the proportions
        Vector<float4> arrowDirection(handlePos[1] - handlePos[0]);
        const float4 fLength(arrowDirection.GetLength());
        arrowDirection.Normalize();       

        const float4 fBigRadiusToLength(0.3F);
        const float4 fSmallRadiusToLength(0.15F);
        const float4 fBigRadius(fLength * fBigRadiusToLength);
        const float4 fSmallRadius(fLength * fSmallRadiusToLength);
        const float4 fProportionHead(0.5F);
        const float4 fLengthHead(fLength * fProportionHead);
        const float4 fLengthTail(fLength * (1.0F - fProportionHead));
        const unsigned int iSlices(32); // accuracy of cylinder approximation

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        {
          glTranslatef(handlePos[0][0], handlePos[0][1], handlePos[0][2]);

          Normal<float4> normal(1.0F, 0.0F, 0.0F);
          const Normal<float4> Xnormal(1.0F, 0.0F, 0.0F);
          const Normal<float4> Ynormal(0.0F, 1.0F, 0.0F);
          const Normal<float4> Znormal(0.0F, 0.0F, 1.0F);
          Vector<float4> arrowXZ(arrowDirection[0], 0.0F, arrowDirection[2]);
          Matrix<float4> matrix;

          if (arrowXZ.GetLength() > 0.0F)
          {
            normal = Ynormal;
            matrix.Identity();
            arrowXZ.Normalize();
            const float fAngleXZandZ(acos(arrowXZ.Dot(Znormal)) * 180/M_PI);
            float4 fSignY = ((arrowXZ[0] < 0) ? -1 : 1);
            glRotatef(fSignY * fAngleXZandZ, normal[0], normal[1], normal[2]);
            matrix.Rotate(normal, fSignY * fAngleXZandZ * M_PI/180.0);
            arrowDirection = matrix * arrowDirection;
          }

          matrix.Identity();

          normal = Xnormal;
          const float fAngleYZandZ(acos(arrowDirection.Dot(Znormal)) * 180/M_PI);
          float4 fSignZ = ((arrowDirection[1] < 0) ? -1 : 1);
          glRotatef(-fSignZ * fAngleYZandZ, normal[0], normal[1], normal[2]);
          matrix.Rotate(normal, fSignZ * fAngleYZandZ * M_PI/180.0);
          arrowDirection = matrix * arrowDirection;

          gluQuadricDrawStyle(pQuadric, (GLenum)GLU_FILL);
          gluQuadricOrientation(pQuadric, (GLenum)GLU_OUTSIDE);
          gluQuadricNormals(pQuadric, (GLenum)GLU_SMOOTH);
          gluCylinder(pQuadric, 0.0F, fBigRadius, fLengthHead, iSlices, 1);

          // make cone base
          glPushMatrix();
          {
            glTranslatef(0.0F, 0.0F, fLength - fLengthHead);
            gluDisk(pQuadric, 0, fBigRadius, iSlices, 1);
          }
          glPopMatrix();

          // make cylinder
          glPushMatrix();
          {
            glTranslatef(0.0F, 0.0F, fLengthHead);
            gluCylinder(pQuadric, fSmallRadius, fSmallRadius, fLengthTail, iSlices, 1);
          }
          glPopMatrix();

          // make end cap
          glPushMatrix();
          {
            glTranslatef(0.0F, 0.0F, fLength);
            gluDisk(pQuadric, 0, fSmallRadius, iSlices, 1);
          }
          glPopMatrix();
        }
        glPopMatrix();

        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_DEPTH_TEST);

        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);

        gluDeleteQuadric(pQuadric);

        //glPopAttrib();

        // call parent class
        vxAnnotation::Render(environment);
      } // annotation visible?
    } // two handles?
  } // annotation supported?


  return;
} // Render()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationArrow::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  // do local scope stuff
  vxSerializer::SetAttribute(element, "version", "2.0");

} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationArrow::FromXml(vxDOMElement & element)
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
    //nothing stored in XML now 
  }
  else
  {
    LogDbg("Unknown version number for XML", "vxAnnotationArrow", "FromXml()");
  }

  return bSuccess;
} // FromXml()


/**
 * Get the position of a given handle
 * @param index of handle
 * @return position of handle
 */
vxHandle vxAnnotationArrow::GetHandle(const uint2 uHandleIndex) const
//*******************************************************************
{
  vxHandle handle(vxAnnotation::GetHandle(uHandleIndex));

  if (uHandleIndex == 1) // handle 1
  {
    // since the arrow needs to be draw with constant screen space size, we need to
    // adjust its tail handle in order to account for the zoom factor and setting of
    // each viewer.
    if (GetEnvironment() == NULL || GetEnvironment()->GetViewerType() == NULL) 
    {
      throw ex::DataCorruptionException(LogRec(" GetEnvironment() return Null pointer ", "vxAnnotationArrow", "GetHandle"));
    }

    // find the world distance of a screen pixel distance
    ///////////////////////////////////////////////////////
    Point3Df worldPos0, worldPos1;
    
    Point2D<int2> screenCenter(GetEnvironment()->GetWinRect().GetReferencePoint().first + 0.5 * GetEnvironment()->GetWinRect().GetViewportSize().first,
                               GetEnvironment()->GetWinRect().GetReferencePoint().second + 0.5 * GetEnvironment()->GetWinRect().GetViewportSize().second);
    Point2D<int2> screenCenterNeighbour(1 + screenCenter.X(), screenCenter.Y());

    if (GetEnvironment()->GetViewerType()->ConvertScreenToWorld(screenCenter, worldPos0) == false)
    {
      LogDbg("Weired, screen center should have a corresponding world position", "vxAnnotationArrow", "GetHandle()");
      return handle;
    }

    if (GetEnvironment()->GetViewerType()->ConvertScreenToWorld(screenCenterNeighbour, worldPos1) == false)
    {
      LogDbg("Weired, screen center neighbour should have a corresponding world position", "vxAnnotationArrow", "GetHandle()");
      return handle;
    }

    float4 fWorldUnitDist(worldPos0.GetEuclideanDistance(worldPos1));

    Point3Df handle0WorldPos(vxHandlesObj::GetHandle(0).GetPosition());
    Point3Df handle1WorldPos(vxHandlesObj::GetHandle(1).GetPosition());
    Vector3Df arrowWorldVector(handle1WorldPos.X()-handle0WorldPos.X(),
                               handle1WorldPos.Y()-handle0WorldPos.Y(),
                               handle1WorldPos.Z()-handle0WorldPos.Z());
    Point<float4> handle1WorldPosCorrected(handle1WorldPos);

    if (arrowWorldVector.GetLength() > 0.0001F)
    {
      arrowWorldVector.Normalize();
      const float4 fArrowScreenLength(GetScreenLength());
      handle1WorldPosCorrected = handle0WorldPos + fArrowScreenLength * fWorldUnitDist * arrowWorldVector;
    }
    
    handle.SetPosition(handle1WorldPosCorrected);
  }

  return handle;
} // GetHandle()



#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationArrow.C,v $
// Revision 1.10  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.9  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.8  2006/08/08 13:38:05  romy
// adjusted arrow annotation size
//
// Revision 1.7  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.6  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/08/01 19:11:39  frank
// changed function name according to coding standards
//
// Revision 1.5  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.4  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/08 14:10:03  frank
// michael fixed the problem with opacity not being obeyed in some cases
//
// Revision 1.3  2005/02/08 00:01:34  michael
// Fixed the drawing to account for the actual set opacity (enable blending)
//
// Revision 1.2  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.36  2004/02/07 01:12:24  michael
// fixed issue #3721
//
// Revision 1.35  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.34  2004/01/21 16:33:15  michael
// function coments adjustments
//
// Revision 1.33  2004/01/14 18:45:05  wenli
// remove the screen mode detective
//
// Revision 1.32  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.31  2004/01/07 16:51:37  wenli
// Issue # 3570 : Adjusting an arrow annotation from the Curved MPR is increasing the annotation size
//
// Revision 1.30  2003/12/17 15:40:16  michael
// enforced variable naming following our coding standard
//
// Revision 1.29  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.28  2003/06/12 20:12:42  wenli
// update the handle based on the viewtype
//
// Revision 1.27  2003/05/23 15:45:33  michael
// removed debug code
//
// Revision 1.26  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.25  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.24  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.23  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.22  2003/03/17 18:56:50  michael
// previous checkin fiexed issue #2917
//
// Revision 1.21  2003/03/17 18:34:51  michael
// reduced arrow size
//
// Revision 1.20  2003/03/04 03:26:31  michael
// fixed arrow to never have zero length during creation
//
// Revision 1.19  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.18  2003/03/01 22:19:03  michael
// fixed arrow selection on second handle
//
// Revision 1.17  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.16  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.15  2003/02/27 17:45:49  michael
// fixed arrow annotation
//
// Revision 1.14  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.13  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.12  2003/01/21 23:04:11  michael
// improved the equal test so that it makes sense!
//
// Revision 1.10  2003/01/21 22:41:13  michael
// added test to see if there are twodifferent points (not equal)
//
// Revision 1.9  2003/01/21 20:41:30  michael
// fixed angle annotation, improved arrow annotaion, adjusted lines to 3 pixel
// width and consistently using glColor4f() instead of glColor3f()
//
// Revision 1.8  2003/01/20 20:41:04  michael
// added restrictions to the two handles (SetHandle())
//
// Revision 1.7  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.6  2003/01/19 21:07:09  michael
// added box annotation
//
// Revision 1.5  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.4  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.3  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.2  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.1  2003/01/14 15:16:05  michael
// added inital arrow annotation files
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationArrow.C,v 1.10 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationArrow.C,v 1.10 2006/11/24 19:17:49 romy Exp $
