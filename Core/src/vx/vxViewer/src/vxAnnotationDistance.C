// $Id: vxAnnotationDistance.C,v 1.8 2006/11/24 19:17:49 romy Exp $
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
#include "vxAnnotationDistance.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "utilities.h"
#include "vxEnvironment.h"
#include "vxSerializer.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Default constructor
 */
vxAnnotationDistance::vxAnnotationDistance()
//*******************************************************************
{
  CommonInit();
	//SetDrawHandles( false );
}


/**
 * Copy Constructor
 */
vxAnnotationDistance::vxAnnotationDistance( const vxAnnotationDistance & other ) : vxAnnotation( other )		
{
	CommonInit();
}//Copy Constructor


/**
 * Clone method
 */
vxAnnotationDistance * vxAnnotationDistance::Clone() const
{
	return new vxAnnotationDistance( * this ); 
} //Clone method


/**
 * CommonInit
 */
void vxAnnotationDistance::CommonInit()
//*******************************************************************
{
  SetMouseCursor("CURSOR_LINEAR");
  SetImage(ResourceImage::FromResource("ANNOTATION_DISTANCE"));
} // CommonInit()


/**
 * Reset
 */
void vxAnnotationDistance::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // Reset()


/**
 * Render the annotation
 * @param environment
 */
void vxAnnotationDistance::Render(vxEnvironment & environment)
//*******************************************************************
{
  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(*this))
  {
    if (viewerType.IsVisible(*this))
    {
      if (GetNumHandles() == 2)
      {
        // get the color for the sphere
        PixelRGBA<uint1> color;
        if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() == 0 || GetSelectedHandleIndex() == 1)
          color = GetColorSelected();
        else
          color = GetColor();

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        // draw shadow for the line
        glColor4f(0.0F, 0.0F, 0.0F, color.A()/255.0);   
        glLineWidth( m_fLineShadowWidth );
        glBegin(GL_LINES);
        
        // get the handle points
        Point<float4> handlePos[2];
        switch (viewerType.GetRenderCoordinateSystem())
        {
        case vxViewerType::eWorldSpace:
          handlePos[0] = GetHandle(0).GetPosition();
          handlePos[1] = GetHandle(1).GetPosition();
        	break;
        case vxViewerType::eScreenSpace:
          for (int i(0); i<2; ++i)
          {
            Point2D<int2> screenPos;
            viewerType.ConvertWorldToScreen(GetHandle(i).GetPosition(), screenPos);
            handlePos[i].m_x = screenPos.m_x;
            handlePos[i].m_y = screenPos.m_y;
            handlePos[i].m_z = 0;
          }
        	break;
        default:
          throw ex::IllegalArgumentException(LogRec("bad render coordinate system", "vxAnnotationDistance", "Render"));
        	break;
        }
        glVertex3f(handlePos[0][0], handlePos[0][1], handlePos[0][2]);
        glVertex3f(handlePos[1][0], handlePos[1][1], handlePos[1][2]);
        glEnd();

        // draw line
        glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);   
        glLineWidth( m_fLineWidth );
        glBegin(GL_LINES);
        glVertex3f(handlePos[0][0], handlePos[0][1], handlePos[0][2]);
        glVertex3f(handlePos[1][0], handlePos[1][1], handlePos[1][2]);
        glEnd();
      } // two handles?

      // call parent class
      vxAnnotation::Render(environment);
    } // annotation visible?
  } // annotation supported?

  return;
} // Render()


/**
 * Update the annotation text
 * @param environment
 */
void vxAnnotationDistance::Update(vxEnvironment & environment)
//*******************************************************************
{
  if (GetNumHandles() == 2)
  {
    SetText(ToAscii(GetHandle(0).GetPosition().GetEuclideanDistance(GetHandle(1).GetPosition())) + " mm");
  }

  return;
} // Update()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationDistance::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationDistance::FromXml(vxDOMElement & element)
//*******************************************************************
{
  bool bSuccess(true);

  // call parent class
  bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  return bSuccess;
} // FromXml()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationDistance.C,v $
// Revision 1.8  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.7  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.6  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.5  2006/08/03 21:33:30  romy
// removed the AnnotationAdjust tool
//
// Revision 1.4  2006/08/02 21:37:02  romy
// adjusted the line & line shadow width
//
// Revision 1.3  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.2.1  2005/08/01 19:11:39  frank
// changed function name according to coding standards
//
// Revision 1.2  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.28  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.27  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.26  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.25  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.24  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.23  2003/04/02 21:01:13  michael
// fixed issue #2581
//
// Revision 1.22  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.21  2003/03/21 17:46:05  michael
// fixed drawing in saggital and coronal (issue #2937)
//
// Revision 1.20  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.19  2003/03/06 16:01:42  frank
// Permitted annotations to be used within a plugin
//
// Revision 1.18  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.17  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.16  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.15  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.14  2003/02/24 14:58:57  frank
// Added local coordinate transformation for non-OpenGL projections
//
// Revision 1.13  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.12  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 1.11  2003/01/21 20:41:30  michael
// fixed angle annotation, improved arrow annotaion, adjusted lines to 3 pixel
// width and consistently using glColor4f() instead of glColor3f()
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
// Revision 1.4  2003/01/13 14:43:47  geconomos
// Set the active annotation in AddAnnotation().
//
// Revision 1.3  2003/01/10 17:14:46  michael
// fixed (compile) bug
//
// Revision 1.2  2003/01/10 14:20:58  michael
// fixed compilation
//
// Revision 1.1  2003/01/10 13:39:40  michael
// added initial version of distance annotation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationDistance.C,v 1.8 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationDistance.C,v 1.8 2006/11/24 19:17:49 romy Exp $
