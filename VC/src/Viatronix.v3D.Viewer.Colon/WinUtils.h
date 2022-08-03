// $Id: WinUtils.h,v 1.2.12.1 2009/11/25 22:05:47 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: WinUtils.h -  interface for the WinGLUtils static library
// Owner(s): Jeffrey Meade  jmeade@viatronix.net
//

#if !defined(AFX_WINUTILS_H__7D3D91CE_3BA2_47EB_805B_46E94E4A3E74__INCLUDED_)
#define AFX_WINUTILS_H__7D3D91CE_3BA2_47EB_805B_46E94E4A3E74__INCLUDED_

#include "MenuButton.h"
#include "MessageWnd.h"

namespace WinUtils
{

  /// Sets icons or bitmap buttons
  template <class T>
  inline void SetButtonIcon(T & btn, const int4 iBtnIconID);

  /// Sets icons or bitmap buttons
  inline void SetButtonIcons(CWnd * pWnd, const UINT vuBtnsNicons[][2], const int4 iNumBtns);

  /// Enables/disables the child item within the window
  inline void EnableDlgItem(CWnd * pWnd, const int iDlgID, const bool bEnable);

  /// Alters the point to reflect the closest point within the window
  inline void ClipToRect(CPoint & p, const CRect & r);

  /// Converts rectangle to client coordinates
  inline BOOL ScreenToClient(HWND hWnd, CRect & r);

  /// Returns the client rectangle (left and top equal to 0)
  inline CRect GetClientRect(const CWnd * pWnd);

  /// Returns the screen-coordinate rectangle
  inline CRect GetWindowRect(const CWnd * pWnd);

  /// Returns the client rectangle (left and top equal to 0)
  inline CRect GetClientRect(const CWnd & wnd);

  /// Returns the screen-coordinate rectangle
  inline CRect GetWindowRect(const CWnd & wnd);

  /// Return the child's display rectangle within the given parent window
  inline CRect GetChildRect(const CWnd & child, const CWnd & parent);

  /// Return the child's display rectangle within the given parent window
  inline CRect GetChildRect(const CWnd * pChild, const CWnd * pParent);

  /// Return the child's display rectangle within the parent window
  inline CRect GetChildRect(const CWnd & child);

  /// Return the child's display rectangle within the parent window
  inline CRect GetChildRect(const CWnd * pChild);

  /// Returns the specified CString from the string resource
  CString LoadResourceString(UINT uID);

  /// Returns the specified CString from the string resource, with the first letter being UPPER case.
  CString LoadResourceStringWithUpperHead(UINT uID);

  /// Returns the specified CString from the string resource, with the first letter being LOWER case.
  CString LoadResourceStringWithLowerHead(UINT uID);

  /// Change the first alpha letter to Upper.
  inline CString UpperHeadString(CString sStr);

  /// Change the first alpha letter to Lower.
  inline CString LowerHeadString(CString sStr);

  /// Change all chars to lower case.
  inline CString LowerCaseString(CString sStr);
  inline CString LowerCaseString(UINT uID);

  /// Change all chars to upper case.
  inline CString UpperCaseString(CString sStr);
  inline CString UpperCaseString(UINT uID);

  /// Formats a message string based on a system error (e.g. from GetLastError())
  inline std::string FormatSystemErrorMessage(const uint4 iError);

  /// Returns true if the specified keyboard key or physical mouse button is currently held down
  inline bool IsKeyStateDown(const int iVirtKeyCode);

  #include "Dialogs.inl"

} // namespace WinUtils

#endif // !defined(AFX_WINUTILS_H__7D3D91CE_3BA2_47EB_805B_46E94E4A3E74__INCLUDED_)

// Revision History:
// $Log: WinUtils.h,v $
// Revision 1.2.12.1  2009/11/25 22:05:47  dongqing
// GetChildRect method
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.4.2.1  2003/04/14 22:30:18  jmeade
// const parameters where appropriate.
//
// Revision 1.4  2003/01/21 19:28:15  jmeade
// Two files had the same #ifdef used to prevent repeated includes.
//
// Revision 1.3  2003/01/03 22:41:21  jmeade
// Header.
//
// Revision 1.2  2002/05/30 16:13:01  jmeade
// Use template instead of typeid for SetButtonIcon() fn.
//
// Revision 1.1  2002/04/18 18:20:45  jmeade
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WinUtils.h,v 1.2.12.1 2009/11/25 22:05:47 dongqing Exp $
// $Id: WinUtils.h,v 1.2.12.1 2009/11/25 22:05:47 dongqing Exp $
