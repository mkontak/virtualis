//////////////////////////////////////////////////////////////////////
// $Id: VisiSurfaceView.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VisiSurfaceView.h:
// Description: interface for the CVisiSurfaceView class
// Created:
// Author: Jeff Meade  jmeade@viatronix.net
//
// Complete History at bottom of file.
// 

#if !defined(AFX_VISISURFACEVIEW_H__A2661F73_88EF_11D4_B6E5_00D0B7BF4092__INCLUDED_)
#define AFX_VISISURFACEVIEW_H__A2661F73_88EF_11D4_B6E5_00D0B7BF4092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLChildView.h"

class CVisiSurfaceView : public GLChildView  
{
public:
	CVisiSurfaceView(HWND hWndParent);
	virtual ~CVisiSurfaceView();

protected:
	virtual void GLRenderScene(void *data);
  virtual void GLRasterScene(HDC hDC);
	virtual void GLSetupRC(void *data);
	virtual void GLResize(GLsizei width, GLsizei height);
	virtual bool GLIdle(void *data);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

private:
  unsigned int m_zoomPos;
  unsigned int m_zoomWidth;
	bool m_backTextureCreated;
	unsigned int m_texture;
};

#endif // !defined(AFX_VISISURFACEVIEW_H__A2661F73_88EF_11D4_B6E5_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: VisiSurfaceView.h,v $
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:02:02  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:38   ingmar
// Initial revision.
// Revision 1.7  2001/04/13 02:35:22  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VisiSurfaceView.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
// $Id: VisiSurfaceView.h,v 1.1 2005/08/05 12:59:45 geconomos Exp $
