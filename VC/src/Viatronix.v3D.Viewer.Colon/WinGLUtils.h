// $Id: WinGLUtils.h,v 1.2 2005/08/12 21:48:54 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: WinGLUtils.h -  interface for the WinGLUtils static library
// Owner: Jeffrey Meade  jmeade@viatronix.net
//

#if !defined(AFX_WINGLUTILS_H__94AAAE4B_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_)
#define AFX_WINGLUTILS_H__94AAAE4B_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_

#include "glFont.h"
#include "glComboBox.h"
#include "WinUtils.h"


namespace WinGLUtils
{
  /// Translated text for localization of WinGLUtils window items
  class Text
  {
  public:
    /// OK
    static std::string sOK;
    /// Cancel
    static std::string sCancel;
  };
};

#endif // !defined(AFX_WINGLUTILS_H__94AAAE4B_8F07_11D4_B6E7_00D0B7BF4092__INCLUDED_)

// Revision History:
// $Log: WinGLUtils.h,v $
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.6  2003/01/21 19:24:56  jmeade
// Duplicate: Revision : 3.1.2.2: Moving hard-coded strings to string table.
//
// Revision 3.5  2002/04/18 18:21:58  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.4  2002/01/17 18:53:48  binli
// overload functions: UpperCaseString()/LowerCaseString
//
// Revision 3.3  2002/01/09 21:02:08  jmeade
// Code standards.
//
// Revision 3.2  2001/12/26 20:52:35  binli
// adjustment of Report.
//
// Revision 3.1  2001/11/09 01:17:51  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/10/30 23:26:06  jmeade
// FormatSystemErrorMessage convenience function; Conventions
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:50   ingmar
// Initial revision.
// Revision 1.9  2001/08/10 21:07:26  jmeade
// IsKeyStateDown() helper method for GetAsyncKeyState calls
//
// Revision 1.8  2001/06/14 00:39:16  jmeade
// MessageWnd.h
//
// Revision 1.7  2001/05/11 23:13:50  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WinGLUtils.h,v 1.2 2005/08/12 21:48:54 jmeade Exp $
// $Id: WinGLUtils.h,v 1.2 2005/08/12 21:48:54 jmeade Exp $
