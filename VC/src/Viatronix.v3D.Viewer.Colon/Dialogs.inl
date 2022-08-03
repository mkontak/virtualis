// $Id: Dialogs.inl,v 1.3.2.2 2010/01/30 09:07:59 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Dialogs.inl  
// Description:  
// Created:  
// Author: Jeffrey Meade  jmeade@viatronix.net
//
// Complete History at bottom of file.


#ifndef VTX_DIALOGS_INL_HELPER_FUNCTIONS
#define VTX_DIALOGS_INL_HELPER_FUNCTIONS

#include <string>

/**
 * Sets a button's icon or bitmap
 *
 * @param CWnd & btnWnd       Button window instance
 * @param const int4 iBtnID   Resource ID of the button icon or bitmap
 */
template <class T>
inline void SetButtonIcon(T & btn, const int4 iBtnIconID)
{
   btn.SetBitmap( iBtnIconID );
}


/**
 * Sets icons for a number of buttons.
 *
 * @param CWnd * pWnd                    Pointer to parent window.
 * @param const UINT muBtnsNicons[][2]   Array of button ID/button icon matchups
 * @param const int4 iNumBtns            Number of buttons (determines size of muBtnsNicons)
 */
inline void SetButtonIcons(CWnd * pWnd, const UINT muBtnsNicons[][2], const int4 iNumBtns)
{
  for (uint4 i(0); i < iNumBtns; i++)
  {
    SkinButton * pBtnWnd = (SkinButton *) pWnd->GetDlgItem(muBtnsNicons[i][0]);

#ifndef FINAL_RELEASE
    if (pBtnWnd == (CWnd *) NULL)
    {
      AfxMessageBox("GetDlgItem() failed!");
      continue;
    }
#endif

    SetButtonIcon(*pBtnWnd, muBtnsNicons[i][1]);
  }
}


/**
 * Enables the item in the given dialog
 *
 * @param CWnd * pWnd            Pointer to the items parent window
 * @param const int & iDlgID     Dialog item ID
 * @param const bool & bEnable   True if enabling, false otherwisd
 */
inline void EnableDlgItem(CWnd * pWnd, const int iDlgID, const bool bEnable)
{
  CWnd * pWndItem = pWnd->GetDlgItem(iDlgID);
#ifndef FINAL_RELEASE
  if (pWndItem == (CWnd *) NULL)
  {
    return;
  }
#endif
  pWndItem->EnableWindow(bEnable == true);
}


/**
 * Restricts point to the inside of the dialog
 *
 * @param CPoint & pt          Point to restrict
 * @param const CRect & rect   Rectangle that is to be bounding for the point
 * @return inline void         
 */
inline void ClipToRect(CPoint & pt, const CRect & rect)
{
  if (pt.x < rect.left)           pt.x = rect.left;
  else if (pt.x > rect.right)     pt.x = rect.right;
  if (pt.y < rect.top)            pt.y = rect.top;
  else if (pt.y > rect.bottom)    pt.y = rect.bottom;
}


/**
 * Converts the rectangle to screen clients
 *
 * @param HWND hWnd      Handle to window
 * @param CRect & rect   Rectangle whose coordinate will be changes
 * @return inline BOOL   Success of conversion.
 */
inline BOOL ScreenToClient(HWND hWnd, CRect & rect)
{
  BOOL bRetVal = TRUE;
  CPoint pt;

  bRetVal &= ScreenToClient(hWnd, &(pt = rect.TopLeft()));
  rect.TopLeft() = pt;

  bRetVal &= ScreenToClient(hWnd, &(pt = rect.BottomRight()));
  rect.BottomRight() = pt;

  return bRetVal;
}


/**
 * Returns the client rectangle for the given window.
 *
 * @param CWnd * pWnd   Pointer to window
 * @return CRect        Client rectangle
 */
inline CRect GetClientRect(const CWnd * pWnd)
{
  CRect rect;
  pWnd->GetClientRect(rect);
  return rect;
}


/**
 * Gets window rectangle (screen coordinates) for the window.
 *
 * @param CWnd * pWnd    Pointer to window
 * @return CRect         Window rectangle
 */
inline CRect GetWindowRect(const CWnd * pWnd)
{
  CRect rect;
  pWnd->GetWindowRect(rect);
  return rect;
}


/**
 * Returns the client rectangle for the window.
 *
 * @param const CWnd & wnd   Reference to window
 * @return CRect             Client rectangle
 */
inline CRect GetClientRect(const CWnd & wnd)
{
  CRect rect;
  wnd.GetClientRect(rect);
  return rect;
}


/**
 * Gets the window rectangle (client coordinates).
 *
 * @param const CWnd & wnd   Reference to window
 * @return inline CRect      Window rectangle
 */
inline CRect GetWindowRect(const CWnd & wnd)
{
  CRect rect;
  wnd.GetWindowRect(rect);
  return rect;
}


/**
 * Return the child's display rectangle within the parent window
 *
 * @param pChild    child window
 * @param pParent   child's parent
 * @return          the child's window rectangle in parent-window coordinates
 */
inline CRect GetChildRect(const CWnd & child, const CWnd & parent)
{
  CRect rect; child.GetWindowRect(rect);
  parent.ScreenToClient(rect);
  return rect;
}


/**
 * Return the child's display rectangle within the parent window
 *
 * @param pChild    child window
 * @param pParent   child's parent
 * @return          the child's window rectangle in parent-window coordinates
 */
inline CRect GetChildRect(const CWnd * pChild, const CWnd * pParent)
{
  CRect rect; pChild->GetWindowRect(rect);
  pParent->ScreenToClient(rect);
  return rect;
}


/**
 * Return the child's display rectangle within the parent window
 *
 * @param pChild    child window
 * @return          the child's window rectangle in parent-window coordinates
 */
inline CRect GetChildRect(const CWnd & child)
{
  return GetChildRect(child, *(child.GetParent()));
}

/**
 * Return the child's display rectangle within the parent window
 *
 * @param pChild    child window
 * @return          the child's window rectangle in parent-window coordinates
 */
inline CRect GetChildRect(const CWnd * pChild)
{
  return GetChildRect(pChild, pChild->GetParent());
}


/**
 *  Change the first alpha letter to Upper.
 *
 * @param sStr  the string to be processed.
 * @return      the string after the processed.
 */
inline CString UpperHeadString(CString sStr)
{
  for (int4 i=0; i<sStr.GetLength(); i++)
  {
    if (isalpha(sStr.GetAt(i)))
    {
      sStr.SetAt(i, toupper(sStr.GetAt(i)));
      break;
    }
    else
      continue;
  }
  return sStr;
}


/**
 *  Change the first alpha letter to Lower.
 *
 * @param sStr  the string to be processed.
 * @return      the string after the processed.
 */
inline CString LowerHeadString(CString sStr)
{
  for (int4 i=0; i<sStr.GetLength(); i++)
  {
    if (isalpha(sStr.GetAt(i)))
    {
      sStr.SetAt(i, tolower(sStr.GetAt(i)));
      break;
    }
    else
      continue;
  }
  return sStr;
}

inline CString LowerCaseString(CString sStr)
{
  sStr.MakeLower();
  return sStr;
}

inline CString UpperCaseString(CString sStr)
{
  sStr.MakeUpper();
  return sStr;
}

inline CString LowerCaseString(UINT uID)
{
  CString sStr = LoadResourceString(uID);
  sStr.MakeLower();
  return sStr;
}

inline CString UpperCaseString(UINT uID)
{
  CString sStr = LoadResourceString(uID);
  sStr.MakeUpper();
  return sStr;
}


/**
 * Formats a message string based on a system error (e.g. from GetLastError())
 *
 * @param const uint4 iError   The error.
 * @return std::string         A string representation of the error.
 */
inline std::string FormatSystemErrorMessage(const uint4 iError)
{
  LPVOID lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, iError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf, 0,NULL);
  std::string sError = LPCTSTR(lpMsgBuf);
  LocalFree( lpMsgBuf );
  return sError;
}


/**
 * Returns true if the specified key is currently down
 *
 * @param const int iVirtKeyCode   Code for the keyboard key
 * @return inline bool             True if the key is being held down, false otherwise
 */
inline bool IsKeyStateDown(const int4 iVirtKeyCode)
{
  return ((GetAsyncKeyState(iVirtKeyCode) & 0x8000) != 0);
}

#endif //ifndef VTX_DIALOGS_INL_HELPER_FUNCTIONS


// $Log: Dialogs.inl,v $
// Revision 1.3.2.2  2010/01/30 09:07:59  dongqing
// spacing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3.2.1  2009/11/25 22:13:29  dongqing
// GetChildRect method
//
// Revision 1.3  2006/04/12 23:11:14  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.2.2.1  2006/02/28 17:27:46  geconomos
// modified for SkinButtonChanges
//
// Revision 1.2  2005/10/25 15:41:19  jmeade
// calling related methods instead of repeating code.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.10.2.1  2003/04/14 22:30:18  jmeade
// const parameters where appropriate.
//
// Revision 3.10  2003/01/03 22:39:46  jmeade
// Removed unnecessary include.
//
// Revision 3.9  2002/06/04 18:37:01  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.8  2002/05/30 16:12:32  jmeade
// Use template instead of typeid for SetButtonIcon() fn.
//
// Revision 3.7  2002/05/29 15:43:45  jmeade
// Forward class declaration.
//
// Revision 3.6.2.1  2002/05/13 17:21:44  jmeade
// Necessary forward class declaration.
//
// Revision 3.6  2002/01/17 18:53:48  binli
// overload functions: UpperCaseString()/LowerCaseString
//
// Revision 3.5  2002/01/09 21:02:08  jmeade
// Code standards.
//
// Revision 3.4  2001/12/26 20:52:35  binli
// adjustment of Report.
//
// Revision 3.3  2001/11/23 14:56:21  jmeade
// Default to bitmap style for SkinButton if none set.
//
// Revision 3.2  2001/11/17 01:47:58  jmeade
// References instead of pointers (code standards)
//
// Revision 3.1  2001/11/09 01:16:31  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.2  2001/10/30 23:26:06  jmeade
// FormatSystemErrorMessage convenience function; Conventions
//
// Revision 3.0.2.1  2001/10/28 11:34:16  jmeade
// Coding standards
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:38   ingmar
// Initial revision.
// Revision 1.10  2001/08/10 21:07:26  jmeade
// IsKeyStateDown() helper method for GetAsyncKeyState calls
//
// Revision 1.9  2001/05/11 19:20:10  jmeade
// Coding conventions
//
// Revision 1.8  2001/01/24 21:00:07  jeff
// SetButtonIcon(): added a typeid check for SkinAutoCheckButton(), consolidated code
//
// Revision 1.7  2000/12/12 17:46:20  jeff
// SetButtonIcon() method encapsulates some functionality of SetButtonIcons()
//
// Revision 1.6  2000/12/08 00:51:22  jeff
// check run-time type before casting buttons in SetButtonIcons
//
// Revision 1.5  2000/11/22 19:19:45  jeff
// Check button style to set for bitmap (BS_BITMAP) or icon (BS_ICON) types
//
// Revision 1.4  2000/11/13 12:52:17  jeff
// RegistryKey, function declarations for Dialogs.inl helper functions
//
// Revision 1.3  2000/09/27 14:40:19  jeff
// Added GetWindowRect function that RETURNS THE RECT
//
// Revision 1.2  2000/09/20 21:52:47  jeff
// Adding menu buttons
//
// Revision 1.1  2000/09/18 19:19:40  jeff
// no message
//
// Revision 1.2  2000/09/13 16:31:17  jeff
// Removing unnecessary references to obsolete CChildView class
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 3     9/01/00 12:10p Jeff
// Another 'modes' incremental checkin
// 
// 2     8/28/00 2:13p Antonio
// updated header and footer
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/24/00  Time:  7:19p
// Created Dialogs.inl
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Dialogs.inl,v 1.3.2.2 2010/01/30 09:07:59 dongqing Exp $
// $Id: Dialogs.inl,v 1.3.2.2 2010/01/30 09:07:59 dongqing Exp $
