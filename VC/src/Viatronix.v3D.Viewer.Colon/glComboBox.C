// $Id: glComboBox.C,v 1.2 2005/08/16 21:46:44 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: glComboBox.C: implementation of the GLComboBox class
// Author(s): Jeffrey Meade jmeade@viatronix.com

#include "stdafx.h"
#include "glComboBox.h"
#include "Rect.h"

using namespace WinUtils;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLComboBox::GLComboBox()
{
	m_bMouseOver = false;
	m_uMouseFlags = 0;
}

GLComboBox::~GLComboBox()
{
}

void GLComboBox::Create(const CRect &rect)
{
	m_rect = rect;
}

void GLComboBox::OnDraw()
// Call after setting the OpenGL rendering context
{
	HDC hDC = wglGetCurrentDC();
	m_font.Create(hDC, -15, 0, "arial");

	HGLRC hRC = wglGetCurrentContext();

	glPushAttrib(GL_COLOR_BUFFER_BIT);
		CRect r(CPoint(m_rect.left, m_rect.top), CSize(int(m_rect.Width() * 1.25), abs(m_font.GetPixelHeight())));
		r.NormalizeRect();

		if (m_bMouseOver)
		{
			// Draw white background
//			glColor3f(1,1,1);
//			glBegin(GL_QUADS);
//				glVertex2i(r.left, r.bottom);				glVertex2i(r.left, r.top);
//				glVertex2i(r.right, r.top);					glVertex2i(r.right, r.bottom);
//			glEnd();

			// Draw grey border
			r.InflateRect(1,1);
			glColor3f(0.5f,0.5f,0.5f);
			glColor3f(1,1,1);
			glBegin(GL_LINE_LOOP);
				glVertex2i(r.left, r.bottom);				glVertex2i(r.right, r.bottom);
				glVertex2i(r.right, r.top);					glVertex2i(r.left, r.top);
			glEnd();

			// Draw black text
			glColor3f(1,1,1);
			m_font.DrawText(r.left+1, r.top+1, "Test String");
		}
		else
		{
			// Draw white text
			glColor3f(1,1,1);
			m_font.DrawText(r.left, r.top, "Test String");
		}

	glPopAttrib();
}

void GLComboBox::OnMouseMove(const unsigned int, const CPoint & pt)
{
	m_bMouseOver = (m_rect.PtInRect(pt) != FALSE);
}

// Revision History:
// $Log: glComboBox.C,v $
// Revision 1.2  2005/08/16 21:46:44  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.26.3  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.0.26.2  2005/04/21 12:55:31  frank
// rollback to Tuesday's version - stable but slow
//
// Revision 3.0.26.1  2005/04/20 14:45:08  frank
// fixed font height and width computation and retrieval
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:38   ingmar
// Initial revision.
// Revision 1.5  2001/05/11 19:29:28  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/glComboBox.C,v 1.2 2005/08/16 21:46:44 jmeade Exp $
// $Id: glComboBox.C,v 1.2 2005/08/16 21:46:44 jmeade Exp $
