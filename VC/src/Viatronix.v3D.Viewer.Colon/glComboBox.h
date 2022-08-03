// $Id: glComboBox.h,v 1.3 2006/01/31 14:25:48 frank Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): unknown


// pragmas
#pragma once


// includes
#include "glFont.h"
#include "Afxcoll.h"


namespace WinUtils
{

  class GLComboBox  
  {
  public:

    GLComboBox();

    virtual ~GLComboBox();

	  void Create(const CRect &);

    void OnDraw();

    void OnMouseMove(const unsigned int uFlags, const CPoint & pt);

  public:

    CStringArray m_vStrings;

  private:

    GLFont m_font;

    CRect m_rect;

    bool m_bMouseOver;

    unsigned int m_uMouseFlags;

  };

} // namespace WinUtils


// $Log: glComboBox.h,v $
// Revision 1.3  2006/01/31 14:25:48  frank
// code review
//
// Revision 1.2  2005/08/16 21:46:44  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:39:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:40   ingmar
// Initial revision.
// Revision 1.3  2001/05/11 19:29:28  jmeade
// Coding conventions
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/glComboBox.h,v 1.3 2006/01/31 14:25:48 frank Exp $
// $Id: glComboBox.h,v 1.3 2006/01/31 14:25:48 frank Exp $
