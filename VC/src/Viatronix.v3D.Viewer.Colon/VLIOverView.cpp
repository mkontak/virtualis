//////////////////////////////////////////////////////////////////////
// $Id: VLIOverView.cpp,v 1.2 2007/03/07 22:34:05 jmeade Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VLIOverView.cpp,v 1.2 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : VLIOverview.h
// Description : The same functionality of Overview but with the support of VolumePro 
// Created: Nov 14, 2000
// Author : Wei Li (liwei@viatronix.net).
//

#include "stdafx.h"
#include "VLIOverView.h"

using namespace ViewModes;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/**
 * constructor
 *
 * @param hWndParent     parent window
 * @param orient         study orientation
 * @param bBufferImage   whether to buffer draw images
 **/
VLIOverView::VLIOverView(HWND hWndParent, const StudyOrientations::StudyOrientationEnum &orient, const bool &bBufferImage)
	:OverView(hWndParent, orient, bBufferImage)
{
	m_bVolumeRendering = false;
	InitVLIOptions();
	m_left = -500;
	m_right = 500;
	m_top = -500;
	m_bottom = 500;
	m_near = 1.0;
	m_far = 9600;

	m_bcR = 0.1F;
	m_bcG = 0.1F;
	m_bcB = 0.1F;
	m_bcA = 0.0F;

  ViatronStudy::OverViewMode += TRANSLUCENT;
} // 

VLIOverView::~VLIOverView()
{
	if (m_vliContext) m_vliContext->Release();
	glDeleteTextures (1, &m_textureObjectID);
}

static void handle_opengl_error (bool* error, int* num)
{
	*num = glGetError();
	*error = ((*num) != GL_NO_ERROR);
}

void VLIOverView::GLResize(GLsizei width, GLsizei height)
{
	if (height==0)	height = 1;
	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat fAspect = (GLfloat) width / (GLfloat)height;
	glOrtho(m_left, m_right, m_top, m_bottom, m_near, m_far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  m_uViewWth = width;
  m_uViewHgt = height;
}

bool VLIOverView::RenderVolume (VtxViewModes viewModes)
{
  if(!(viewModes & VOLUME_RENDERING)) 
		return false;
	
	VLIStatus status;
	VLIVolume *volume = GetStudy()->m_vliVolume;
	
	if(!volume)
		return false;

	if(!m_bOpenGLInitialized){
		InitializeOpenGL();
		m_bOpenGLInitialized = true;
	}

	m_vliContext->SetLookupTable(GetStudy()->m_vliLutTTP);
	
	unsigned int sizeX, sizeY, sizeZ;

	volume->GetSize(sizeX, sizeY, sizeZ);

	double glmm[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, glmm); 
	VLIMatrix mm;
	mm.Assign(glmm);

	VLIVector3D x, y, z;

	m_vliContext->GetCamera().SetViewMatrix(mm);

	int buffer = 0, baseWidth, baseHeight, iWidth, iHeight;
	VLIPixel *pixels;
	VLIVector3D hex[6];
	VLIVector2D tex[6];


	// This is applies only for X-Y Supersampling. By setting the accumalation mode
	// to kBlendBasePlane it keeps the baseplane smaller and should generally speed
	// things up.
	//
	m_vliContext->SetBasePlaneAccumulation (VLIContext::kBlendBasePlane);

	m_render_error = false;
	m_opengl_error = false;

	status = m_vliContext->RenderBasePlane (volume, buffer);

	//------------------------------------------------------------------------
	// If the render failed with MaxAngleDeviation, try perturbing the view
	//------------------------------------------------------------------------
	if( status == kVLIErrMaxAngleDeviation ) {

		#define  MIN_PERTURB  (5.0 * 0.0174533)  // 5 degrees
		int         perturbAttempt=0;
		bool        iGiveUp = false ;
		VLIMatrix   desiredView  = ( m_vliContext->GetCamera() ).GetViewMatrix();
		VLIVector3D axisX( 1, 0, 0 );
		VLIVector3D axisY( 0, 1, 0 );
		double      rotX,rotY;

		do {

			switch( perturbAttempt ){
			  case 0: rotX =  MIN_PERTURB;  rotY =            0;  break;
			  case 1: rotX = -MIN_PERTURB;  rotY =            0;  break;
			  case 2: rotX =            0;  rotY =  MIN_PERTURB;  break;
			  case 3: rotX =            0;  rotY = -MIN_PERTURB;  break;
			  case 4: rotX =  MIN_PERTURB;  rotY =  MIN_PERTURB;  break;
			  case 5: rotX =  MIN_PERTURB;  rotY = -MIN_PERTURB;  break;
			  case 6: rotX = -MIN_PERTURB;  rotY =  MIN_PERTURB;  break;
			  case 7: rotX = -MIN_PERTURB;  rotY = -MIN_PERTURB;  break;
			  default: iGiveUp = true ;  break;
			}
			perturbAttempt++;

			VLIMatrix   newView = desiredView ;

			if( rotX )  newView *= VLIMatrix::Rotate( rotX, axisX ) ;
			if( rotY )  newView *= VLIMatrix::Rotate( rotY, axisY ) ;

			if( !iGiveUp ) {
			  ( m_vliContext->GetCamera() ).SetViewMatrix( newView );

			  status = m_vliContext->RenderBasePlane( volume, buffer );
			}

		}while( !iGiveUp && (status==kVLIErrMaxAngleDeviation)  );


	}//if( kVLIErrMaxAngleDeviation )


	if ((status != kVLIOK) && (status != kVLIMultiPass)) 
        {
		m_render_error = true;
		m_render_status = status;
	}
        
  // this is where the 7th point will live
  VLIVector3D seventhPoint;

	if (! m_render_error) {
		status = m_vliContext->FetchBasePlane (buffer, baseWidth, baseHeight,
			                               iWidth, iHeight, pixels,
										 hex, tex);
    // Calculate 7th point here using Andy Vespers method.
    // since the hex is a parallelpiped and the points we
    // are getting back from FetchBasePlane are in world coords,
    // we can use vector math to compute the 7th point. 
		seventhPoint = hex[2] + hex[0] - hex[1];
	} 
//			else status = kVLIOK;

	if(status != kVLIOK){
		m_render_error = true;
		m_render_status = status;
	}
	else	{
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
		glBindTexture(GL_TEXTURE_2D, m_textureObjectID);

// Because the OpenGL context is shared by several views.
// The texture parameters may have been changed by during consecutive calls
// Respecify the texture size if it changes
// if (m_previousBasePlaneWidth  != baseWidth ||
//     m_previousBasePlaneHeight != baseHeight)
		{
			m_previousBasePlaneWidth  = baseWidth;
			m_previousBasePlaneHeight = baseHeight;

			glBindTexture(GL_TEXTURE_2D, m_textureObjectID);


			glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, baseWidth, baseHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			handle_opengl_error (&m_opengl_error, &m_opengl_error_code);

			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}				

		if (! m_opengl_error) {
			glPixelStorei(GL_UNPACK_ROW_LENGTH, baseWidth);
			handle_opengl_error (&m_opengl_error, &m_opengl_error_code);
		}

		if (! m_opengl_error) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight,GL_RGBA, GL_UNSIGNED_BYTE,pixels);
			handle_opengl_error (&m_opengl_error, &m_opengl_error_code);
		}

		if (! m_opengl_error){
			// draw the volume using the 7th point.
                // Rather than draw a hex, we draw a triangle
                // fan based at the 7th point.
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();             
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();

			glBegin (GL_TRIANGLE_FAN);

      //draw the seventh
			glTexCoord2d( tex[4].X(), tex[2].Y());
      glVertex3d( seventhPoint.X(), seventhPoint.Y(), seventhPoint.Z());
					  
			for (int i = 0; i < 6; i++) {
				glTexCoord2d(tex[i].X(), tex[i].Y()); 
				glVertex3d(hex[i].X(),hex[i].Y(), hex[i].Z());
			}
                   
      glTexCoord2d(tex[0].X(), tex[0].Y()); 
			glVertex3d(hex[0].X(),hex[0].Y(), hex[0].Z()); 

			glEnd ();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glFlush ();
		}
		if (! m_render_error)		
			m_vliContext->ReleaseBasePlane (buffer);

		glDisable(GL_TEXTURE_2D);
	}

	return true;
}

//-----------------------------------------------------------------------------
// This functions draws a cube defined by the min and max values
// Also the 6faces of the cube are drawn explicitly to make sure 
// we have the ability to change the colors of the faces later on
//
void VLIOverView::DrawCube (float inMin[3], float inMax[3])
{

	glColor3f (0.0f, 0.8f, 0.2f);

	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	
	glEnable (GL_LINE_SMOOTH);

	// XMin face
	//
//	glColor3f (1.0f, 0.0f, 0.0f);
	glBegin (GL_POLYGON);
		glVertex3f (inMin[0]/*-0.5*/, inMin[1], inMin[2]);
		glVertex3f (inMin[0], inMax[1], inMin[2]);
		glVertex3f (inMin[0], inMax[1], inMax[2]);
		glVertex3f (inMin[0], inMin[1], inMax[2]);
	glEnd ();

	// XMax face
	//
	glBegin (GL_POLYGON);
		glVertex3f (inMax[0]/*+0.5*/, inMin[1], inMin[2]);
		glVertex3f (inMax[0], inMax[1], inMin[2]);
		glVertex3f (inMax[0], inMax[1], inMax[2]);
		glVertex3f (inMax[0], inMin[1], inMax[2]);
	glEnd ();	
	
	
	// YMin face
	//
//	glColor3f (0.0f, 1.0f, 0.0f);
	glBegin (GL_POLYGON);
		glVertex3f (inMin[0], inMin[1]/*-0.5*/, inMin[2]);
		glVertex3f (inMax[0], inMin[1], inMin[2]);
		glVertex3f (inMax[0], inMin[1], inMax[2]);
		glVertex3f (inMin[0], inMin[1], inMax[2]);
	glEnd ();


	// YMin face
	//
	glBegin (GL_POLYGON);
		glVertex3f (inMin[0], inMax[1]/*+0.5*/, inMin[2]);
		glVertex3f (inMax[0], inMax[1], inMin[2]);
		glVertex3f (inMax[0], inMax[1], inMax[2]);
		glVertex3f (inMin[0], inMax[1], inMax[2]);
	glEnd ();	
	

	// ZMin face
	//
//	glColor3f (0.0f, 0.0f, 1.0f);
	glBegin (GL_POLYGON);
		glVertex3f (inMin[0], inMin[1], inMin[2]/*-0.5*/);
		glVertex3f (inMax[0], inMin[1], inMin[2]);
		glVertex3f (inMax[0], inMax[1], inMin[2]);
		glVertex3f (inMin[0], inMax[1], inMin[2]);
	glEnd ();	
		

	// ZMax face
	//
	glBegin (GL_POLYGON);
		glVertex3f (inMin[0], inMin[1], inMax[2]/*+0.5*/);
		glVertex3f (inMax[0], inMin[1], inMax[2]);
		glVertex3f (inMax[0], inMax[1], inMax[2]);
		glVertex3f (inMin[0], inMax[1], inMax[2]);
	glEnd ();


	// Reset the Polygonmode
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glDisable (GL_CLIP_PLANE0);

}

bool VLIOverView::InitializeOpenGL()
{
	// Generate the Texture object
	//
	if (m_textureObjectID == 0)
	{
		m_previousBasePlaneWidth  = 0;
		m_previousBasePlaneHeight = 0;

		glGenTextures (1, &m_textureObjectID);
		glBindTexture(GL_TEXTURE_2D, m_textureObjectID);


		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	return TRUE;
}

void VLIOverView::InitVLIOptions()
{
	m_vliContext = VLIContext::Create();

	VLIStatus status;

	status = m_vliContext->SetGradientOpacityModulation (VLItrue);
	status = m_vliContext->SetSuperSamplingFactor(1, 1, 0.5);

	// Make it zeros to begin with and then generate one 
	//
	m_textureObjectID = 0;

	m_previousBasePlaneWidth  = 0;
	m_previousBasePlaneHeight = 0;

	m_bOpenGLInitialized = false;
}

bool VLIOverView::RayCastVolume(float zoomFactor, float fov, bool transparent)
{
	if(!transparent)
		return false;
	return RenderVolume(VOLUME_RENDERING);
}

// $Log: VLIOverView.cpp,v $
// Revision 1.2  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// Revision 1.30  2001/03/21 21:46:45  liwei
// Obsolete functions removed
//
// Revision 1.29  2001/03/21 15:28:18  liwei
// Remove the RenderObject from VLIOverView. VolumePro is invoked by a virtual member function: RayCastVolume
//
// Revision 1.28  2001/03/19 19:44:01  liwei
// To acomodate the change of the data member accessbility in Overview
//
// Revision 1.27  2001/03/19 18:19:55  binli
// no to access protect/private variables now.
//
// Revision 1.26  2001/03/19 17:26:50  liwei
// To acomodate the change of the data member naming in Study and Overview
//
// Revision 1.25  2001/03/12 20:18:38  liwei
// VLIOverview modified to be compatible with changes in other files
//
// Revision 1.24  2001/02/23 20:11:32  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.23  2001/02/22 01:36:19  jeff
// variable naming to follow coding conventions
//
// Revision 1.22  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.21  2001/01/29 19:47:07  liwei
// CVS test
//
// Revision 1.20  2001/01/29 19:45:55  liwei
// CVS test
//
// Revision 1.19  2001/01/26 19:09:17  geconomos
// Worked on reloading NonDistended areas
//
// Revision 1.18  2001/01/19 21:07:10  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.17  2001/01/18 17:46:08  binli
// removed 'trackball' defined in the class
//
// Revision 1.16  2000/12/28 20:00:16  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.15  2000/12/20 19:17:04  jeff
// Default parameters for study and bBufferImage
// Changed order of parameters in constructor, on the notion that
//  m_bBufferImage is the most likely default-value parameter
//
// Revision 1.14  2000/12/05 17:06:41  liwei
// Code cleaned
//
// Revision 1.13  2000/11/22 20:56:31  liwei
// Use black background
//
// Revision 1.12  2000/11/22 20:49:46  liwei
// The default view mode changed to TRANSLUCENT
//
// Revision 1.11  2000/11/22 20:36:19  liwei
// no message
//
// Revision 1.10  2000/11/20 20:50:41  liwei
// Always set texture parameters before drawing, since the OpenGL context is shared, it may have been changed by others
//
// Revision 1.9  2000/11/17 22:34:14  liwei
// no message
//
// Revision 1.8  2000/11/17 22:01:12  liwei
// The Grabimage() called appropriately in Overview
//
// Revision 1.7  2000/11/17 21:27:27  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.6  2000/11/17 17:18:57  liwei
// Blending order in VLIOverview changed. Using 'B' to toggle the background and foreground
//
// Revision 1.5  2000/11/17 16:36:23  liwei
// The picking problem in VLIOverview solved
//
// Revision 1.4  2000/11/17 05:55:22  liwei
// Bugs fixed for VolumePro
//
// Revision 1.3  2000/11/15 23:32:14  liwei
// Minor changes for VolumePro
//
// Revision 1.2  2000/11/15 19:12:30  liwei
// Adding features to read transfer functions from file
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VLIOverView.cpp,v 1.2 2007/03/07 22:34:05 jmeade Exp $
// $Id: VLIOverView.cpp,v 1.2 2007/03/07 22:34:05 jmeade Exp $
