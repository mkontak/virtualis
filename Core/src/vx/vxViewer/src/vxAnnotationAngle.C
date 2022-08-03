// $Id: vxAnnotationAngle.C,v 1.10 2006/11/24 19:17:49 romy Exp $
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
#include "vxAnnotationAngle.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "vxSerializer.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.10 $"


/**
* Default constructor
*/
vxAnnotationAngle::vxAnnotationAngle()
//*******************************************************************
{
	CommonInit();
	//SetDrawHandles( false );
}


/**
* CommonInit
*/
void vxAnnotationAngle::CommonInit()
//*******************************************************************
{
	m_fArcRadius = 50;
	SetMouseCursor("CURSOR_ANGLE");
	SetImage(ResourceImage::FromResource("ANNOTATION_ANGLE"));
} // CommonInit()

/**
 * Copy constructor
 */
vxAnnotationAngle::vxAnnotationAngle( const vxAnnotationAngle & other ) : vxAnnotation( other )
{
	CommonInit();
	m_fArcRadius = other.m_fArcRadius;
}//Copy constructor

/**
 * Clone method
 */
vxAnnotationAngle * vxAnnotationAngle::Clone() const
{
	return new vxAnnotationAngle( *this );
}//Clone


/**
* Reset
*/
void vxAnnotationAngle::Reset()
//*******************************************************************
{
	// call base class
	vxAnnotation::Reset();

	// initialize all the local stuff
	CommonInit();
} // Reset()


/**
* Transform the handle point into the proper coordinate system
* @param iHandleIndex the index of the handle you wish to transform
* @param environment  the environment
* @return the transformed position
*/
Point3Df vxAnnotationAngle::GetTransformedHandle(const unsigned int iHandleIndex,
																								 vxEnvironment & environment) const
																								 //*******************************************************************
{
	// if we are drawing in world coordinates, just return the world space handle location
	Point3Df transformedPoint(GetHandle(iHandleIndex).GetPosition());
	Point2D<int2> screenPos;

	if (environment.GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace &&
		environment.GetViewerType()->ConvertWorldToScreen(GetHandle(iHandleIndex).GetPosition(), screenPos) == true)
	{
		transformedPoint = Point3Df(screenPos.m_x, screenPos.m_y, 0.0F);
	}

	return transformedPoint;
} // GetTransformedHandle()


/**
* Render the annotation
* @param environment
*/
void vxAnnotationAngle::Render(vxEnvironment & environment)
//*******************************************************************
{
	// call parent class
	//vxAnnotation::Render(environment);

	vxViewerType & viewerType(*environment.GetViewerType());

	if (viewerType.IsAnnotationSupported(*this))
	{
		if (viewerType.IsVisible(*this))
		{
			for (int i(1); i<GetNumHandles(); i++)
			{
				// get the color for the sphere
				PixelRGBA<uint1> color;
				if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() == i-1 
					|| GetSelectedHandleIndex() == i)
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
				Point3Df point1(GetTransformedHandle(i-1, environment));
				Point3Df point2(GetTransformedHandle(i  , environment));
				glVertex3f(point1.m_x, point1.m_y, point1.m_z);
				glVertex3f(point2.m_x, point2.m_y, point2.m_z);
				glEnd();

				// draw shadow for the line
				glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);   
				glLineWidth( m_fLineWidth );
				glBegin(GL_LINES);
				glVertex3f(point1.m_x, point1.m_y, point1.m_z);
				glVertex3f(point2.m_x, point2.m_y, point2.m_z);
				glEnd();
			} // for all handles

			if (GetNumHandles() == 3)
			{
				RenderArc(environment);
			}

			// call parent class
			vxAnnotation::Render(environment);
		} // annotation visible
	} // annotation supported

	return;
} // Render()


/**
* Write object to XML node
* @param node
*/
void vxAnnotationAngle::ToXml(vxDOMElement & element) const
//*******************************************************************
{
	// call parent class
	vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));

	// do local scope stuff
	float4 fArcRadius(GetArcRadius());
	vxSerializer::SetAttribute(element, "version", "2.0");
	vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(fArcRadius), "arc radius"), fArcRadius);
} // ToXml()


/**
* Initialize from XML node
* @param viewer type
* @return successful?
*/
bool vxAnnotationAngle::FromXml(vxDOMElement & element)
//*******************************************************************
{
	if (element.IsNull()) return false;

	bool bSuccess(true);

	// call parent class
	bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

	// do local scope stuff
	std::string sVersion("0.0");
	bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);
	if (atof(sVersion.c_str()) == 2.0)
	{
		float4 fArcRadius(5.0F);
		bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(fArcRadius), "arc radius"), fArcRadius);
		SetArcRadius(fArcRadius);
	}
	else
	{
		LogDbg("Unknown version number for XML", "vxAnnotationAngle", "FromXml()");
	}

	return bSuccess;
} // FromXml()


/**
* Draw the arc representing the angle
* @param environment
*/
void vxAnnotationAngle::RenderArc(vxEnvironment & environment) const
//*******************************************************************
{

	// do we really have three different points?
	if (GetTransformedHandle(0, environment) == GetTransformedHandle(1, environment) ||
		GetTransformedHandle(1, environment) == GetTransformedHandle(2, environment) ||
		GetTransformedHandle(0, environment) == GetTransformedHandle(2, environment))
		return;

	// normalize the vectors(line segments).
	Normal<float4> normal1(GetTransformedHandle(0, environment) - GetTransformedHandle(1, environment)); 
	Normal<float4> normal2(GetTransformedHandle(2, environment) - GetTransformedHandle(1, environment));

	Normal<float4> anglePlaneNormal(normal1.Cross(normal2));
	Normal<float4> halfPlaneNormal(normal1.Cross(anglePlaneNormal));
	vxPlaneOblique<float4> halfPlane(halfPlaneNormal, GetTransformedHandle(1, environment));
	float fSign(1);

	if (halfPlane.GetDistance(GetTransformedHandle(2, environment)) < 0)
		fSign = -1;

	float4 fMaxArcRadius(0.5F * Min(Vector<float4>(GetTransformedHandle(0, environment) - GetTransformedHandle(1, environment)).GetLength(), 
		Vector<float4>(GetTransformedHandle(2, environment) - GetTransformedHandle(1, environment)).GetLength()));
	float4 fArcRadius(GetArcRadius());
	if (fArcRadius > fMaxArcRadius)
		fArcRadius = fMaxArcRadius;

	float4 fDegree(acos(normal1.Dot(normal2)) * 180.0F / M_PI);
	const int4 iSteps(fDegree);

	// determine the color
	PixelRGBA<uint1> desiredColor;
	if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() != -1)
		desiredColor = GetColorSelected();
	else
		desiredColor = GetColor();

	PixelRGBA<uint1> color;

	for (int j(0); j<2; ++j)
	{
		if (j == 0)
		{
			color.R() = color.G() = color.B() = 0;
			glLineWidth( m_fLineShadowWidth );
		}
		else
		{
			color = desiredColor;
			glLineWidth( m_fLineWidth );
		}

		glColor4f(color.R()/255.0F, color.G()/255.0F, color.B()/255.0F, color.A()/255.0F);

		const Point3Df handlePoint(GetTransformedHandle(1, environment));
		// perform actual rendering
		glBegin(GL_LINE_STRIP);

		for (int i(0); i<=iSteps; ++i)
		{
			const float4 fAngle(fDegree * i / float4(iSteps));
			float4 fX(fSign * sin(M_PI * fAngle / 180.0F));
			float4 fY(cos(M_PI * fAngle / 180.0F));

			Vector<float4> vector((Vector<float4>(halfPlaneNormal) * fX) + (Vector<float4>(normal1) * fY));
			vector *= fArcRadius;

			glVertex3f(handlePoint[0] + vector.m_x, handlePoint[1] + vector.m_y, handlePoint[2] + vector.m_z);
		}
		glEnd();
	}

} // RenderArc()


/**
* Update the annotation text
* @param environment
*/
void vxAnnotationAngle::Update(vxEnvironment & environment)
//*******************************************************************
{
	if (GetNumHandles() == 3)
	{
		if (GetHandle(0).GetPosition() == GetHandle(1).GetPosition() || GetHandle(1).GetPosition() == GetHandle(2).GetPosition())
		{
			SetText("0.0 degrees");
		}
		else
		{
			const Normal<float4> normal1(GetHandle(0).GetPosition() - GetHandle(1).GetPosition());
			const Normal<float4> normal2(GetHandle(2).GetPosition() - GetHandle(1).GetPosition());
			float4 fDegree(acos(normal1.Dot(normal2))*180.0/M_PI);
			char sText[50];
			sprintf(sText, "%3.1f degrees", fDegree);
			SetText(sText);
		} // all handles different?
	} // three handles?

	return;
} // Update()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationAngle.C,v $
// Revision 1.10  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.9  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.8  2006/08/07 20:26:16  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.7  2006/08/03 21:33:30  romy
// removed the AnnotationAdjust tool
//
// Revision 1.6  2006/08/02 21:34:15  romy
// asjusted the line shadow width
//
// Revision 1.5  2006/08/02 20:47:05  romy
// asjusted the line shadow width
//
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/17 21:50:24  frank
// Issue #3973: fixed text formatting
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
// Revision 1.27  2004/01/21 16:33:15  michael
// function coments adjustments
//
// Revision 1.26  2004/01/14 14:18:16  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.25  2004/01/06 13:56:09  frank
// Fixed the angle display on curved and luminal
//
// Revision 1.24  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.23  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.22  2003/05/08 16:40:33  michael
// more code reviews (75 render Utils)
//
// Revision 1.21  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.20  2003/04/02 21:01:13  michael
// fixed issue #2581
//
// Revision 1.19  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.18  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.17  2003/03/04 01:43:05  michael
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
// Revision 1.13  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.12  2003/01/21 23:08:38  michael
// added fix for Update() if points are not all three different
//
// Revision 1.11  2003/01/21 22:50:14  michael
// cosmetics
//
// Revision 1.10  2003/01/21 22:36:25  michael
// added test to see if there are three different points
//
// Revision 1.9  2003/01/21 20:41:30  michael
// fixed angle annotation, improved arrow annotaion, adjusted lines to 3 pixel
// width and consistently using glColor4f() instead of glColor3f()
//
// Revision 1.8  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.7  2003/01/19 21:07:09  michael
// added box annotation
//
// Revision 1.6  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.5  2003/01/17 17:39:40  michael
// text of annotations can now be selected and moved
//
// Revision 1.4  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.3  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.2  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.1  2003/01/14 23:25:51  michael
// added initial version of angle annotation
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
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationAngle.C,v 1.10 2006/11/24 19:17:49 romy Exp $
// $Id: vxAnnotationAngle.C,v 1.10 2006/11/24 19:17:49 romy Exp $
