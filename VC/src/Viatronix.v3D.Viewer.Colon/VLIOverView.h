//////////////////////////////////////////////////////////////////////
// $Id: VLIOverView.h,v 1.2 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : Overview.h
// Description : a class to display the overview colon and support segments sorting 
// OverView.h: interface for the COverView class.
//		This class 1> gives user a overview of the colon;
//				       2> supports interactive segments sorting 
//		Interactive controls include : rotate/translate/scale
// Created: Nov 14, 2000
// Author : Wei Li (liwei@viatronix.net).
//
// Complete History at bottom of file.
// 

#if !defined(AFX_VLIOVERVIEW_H__3DF9ABE5_B979_11D4_9066_0050DA90D583__INCLUDED_)
#define AFX_VLIOVERVIEW_H__3DF9ABE5_B979_11D4_9066_0050DA90D583__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OverView.h"

#include "vli.h"

class VLIOverView : public OverView
{
//Member Functions
public:
	/// Constructor
  VLIOverView(HWND hWndParent, const StudyOrientations::StudyOrientationEnum &orient, const bool &bBufferImage = false);

	/// Destructor
	virtual ~VLIOverView();

	/// Initialization functions
	void InitVLIOptions();
  /// init open gl
	bool InitializeOpenGL();

	/// Overloaded virtual functions defined for the parent class
  /// opengl sizing
	virtual void GLResize(GLsizei width, GLsizei height);
  /// cast a ray in the volume
	virtual bool RayCastVolume(float zoomFactor, float fov, bool transparanent);

  /// render the volume
	bool RenderVolume(VtxViewModes viewModes);

	/// Draw the frame of the volume
	void DrawCube (float inMin[3], float inMax[3]);
protected:
private:

// Data Members
public:
  /// context
	VLIContext* m_vliContext;

  /// object id
	GLuint  m_textureObjectID;
  /// previous width
	int4 m_previousBasePlaneWidth;
  /// previous height
	int4 m_previousBasePlaneHeight;

  /// true when opengl initialized
	bool m_bOpenGLInitialized;

  /// types of rendering
	enum VOLUME_RENDERING_MODE { VRM_RAY_CASTING, VRM_SPLATTING, VRM_VOLUME_PRO};
  /// what type of rendering is taking place
	VOLUME_RENDERING_MODE m_eVolumeRenderingMode;

protected:
private:
	/// Variable for error handling
	bool m_bRenderError;
  /// Variable for error handling
	VLIStatus m_renderStatus;
  /// Variable for error handling
	bool m_bOpenglError;
  /// Variable for error handling
	int4 m_iOpenGLErrorCode;

	/// View volume parameters
	GLdouble m_left, m_top, m_right, m_bottom, m_near, m_far;
}; // VLIOverView

#endif // !defined(AFX_VLIOVERVIEW_H__3DF9ABE5_B979_11D4_9066_0050DA90D583__INCLUDED_)

// 
// $Log: VLIOverView.h,v $
// Revision 1.2  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:24   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:40   ingmar
// Initial revision.
// Revision 1.14  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.13  2001/03/21 21:46:45  liwei
// Obsolete functions removed
//
// Revision 1.12  2001/03/21 15:28:18  liwei
// Remove the RenderObject from VLIOverView. VolumePro is invoked by a virtual member function: RayCastVolume
//
// Revision 1.11  2001/03/12 20:18:39  liwei
// VLIOverview modified to be compatible with changes in other files
//
// Revision 1.10  2001/02/23 20:11:32  jmeade
// Removed most unnecessary 'C' before class names (everything's a class!)
//
// Revision 1.9  2001/02/22 01:36:19  jeff
// variable naming to follow coding conventions
//
// Revision 1.8  2001/02/21 20:33:42  jeff
// COverview constructor now takes a StudyOrientation as a parameter (Study parameters unused/obsolete)
//
// Revision 1.7  2001/01/24 21:53:14  liwei
// Conflic resolved
//
// Revision 1.6  2000/12/20 19:17:05  jeff
// Default parameters for study and bBufferImage
// Changed order of parameters in constructor, on the notion that
//  m_bBufferImage is the most likely default-value parameter
//
// Revision 1.5  2000/11/22 20:36:19  liwei
// no message
//
// Revision 1.4  2000/11/17 17:18:57  liwei
// Blending order in VLIOverview changed. Using 'B' to toggle the background and foreground
//
// Revision 1.3  2000/11/17 05:55:22  liwei
// Bugs fixed for VolumePro
//
// Revision 1.2  2000/11/15 19:12:30  liwei
// Adding features to read transfer functions from file
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VLIOverView.h,v 1.2 2007/03/07 22:34:05 jmeade Exp $
// $Id: VLIOverView.h,v 1.2 2007/03/07 22:34:05 jmeade Exp $
