// $Id: PropertySheetDlg.C,v 1.4 2007/03/10 06:09:14 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PropertySheetDlg.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net
//
// PropertySheet.C : implementation file
// Adapted from MSDN article #Q142170 CmySheet
//

#include "stdafx.h"
#include <afxpriv.h>

#include "PropertySheetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_RESIZEPAGE WM_APP+1

enum ChangeDialogFontEnum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

using namespace WinUtils;

/**
 * Helper function which sets the font for a window and all its children
 * and also resizes everything according to the new font
 * 
 * @param pWnd     The parent window
 * @param pFont    Font for children
 * @param eFlag    Change dialog font flag
 */
void ChangeDialogFont(CWnd * pWnd, CFont * pFont, const ChangeDialogFontEnum & eFlag)
{
  CRect windowRect;

  // grab old and new text metrics
  TEXTMETRIC tmOld, tmNew;
  CDC * pDC = pWnd->GetDC();
  CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
  pDC->GetTextMetrics(&tmOld);
  pDC->SelectObject(pFont);
  pDC->GetTextMetrics(&tmNew);
  pDC->SelectObject(pSavedFont);
  pWnd->ReleaseDC(pDC);

  LONG iOldHeight = tmOld.tmHeight + tmOld.tmExternalLeading;
  LONG iNewHeight = tmNew.tmHeight + tmNew.tmExternalLeading;

  if (eFlag != CDF_NONE)
  {
    // calculate new dialog window rectangle
    CRect clientRect, newClientRect, newWindowRect;

    pWnd->GetWindowRect(windowRect);
    pWnd->GetClientRect(clientRect);
    int4 iXDiff = windowRect.Width() - clientRect.Width();
    int4 iYDiff = windowRect.Height() - clientRect.Height();

    newClientRect.left = newClientRect.top = 0;
    newClientRect.right = clientRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
    newClientRect.bottom = clientRect.bottom * iNewHeight / iOldHeight;

    if (eFlag == CDF_TOPLEFT) // resize with origin at top/left of window
    {
      newWindowRect.left = windowRect.left;
      newWindowRect.top = windowRect.top;
      newWindowRect.right = windowRect.left + newClientRect.right + iXDiff;
      newWindowRect.bottom = windowRect.top + newClientRect.bottom + iYDiff;
    }
    else if (eFlag == CDF_CENTER) // resize with origin at center of window
    {
      newWindowRect.left = windowRect.left - 
              (newClientRect.right - clientRect.right)/2;
      newWindowRect.top = windowRect.top -
              (newClientRect.bottom - clientRect.bottom)/2;
      newWindowRect.right = newWindowRect.left + newClientRect.right + iXDiff;
      newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + iYDiff;
    }
    pWnd->MoveWindow(newWindowRect);
  }

  pWnd->SetFont(pFont);

  // iterate through and move all child windows and change their font.
  for (CWnd * pChildWnd = pWnd->GetWindow(GW_CHILD); pChildWnd != NULL; pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT))
  {
    pChildWnd->SetFont(pFont);
    pChildWnd->GetWindowRect(windowRect);

    CString sClassName;
    ::GetClassName(pChildWnd->m_hWnd, sClassName.GetBufferSetLength(32), 31);
    sClassName.MakeUpper();
    if (sClassName == _T("COMBOBOX"))
    {
      CRect rect;
      pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT, 0, (LPARAM) &rect);
      windowRect.right = rect.right;
      windowRect.bottom = rect.bottom;
    }

    pWnd->ScreenToClient(windowRect);
    windowRect.left = windowRect.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
    windowRect.right = windowRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
    windowRect.top = windowRect.top * iNewHeight / iOldHeight;
    windowRect.bottom = windowRect.bottom * iNewHeight / iOldHeight;
    pChildWnd->MoveWindow(windowRect);
  }
} // ChangeDialogFont()

/////////////////////////////////////////////////////////////////////////////
// CMySheet

IMPLEMENT_DYNAMIC(PropertySheetDlg, CPropertySheet)


/**
 * Constructor 
 * 
 * @param uIDCaption    Caption string ID
 * @param pParentWnd    Parent window
 * @param uSelectPage   Initially selected page
 */
PropertySheetDlg::PropertySheetDlg(UINT uIDCaption, CWnd * pParentWnd, UINT uSelectPage)
  :CPropertySheet(uIDCaption, pParentWnd, uSelectPage)
{
} // constructor


/**
 * Constructor 
 * 
 * @param sCaption      Window caption
 * @param pParentWnd    Parent window
 * @param uSelectPage   Initially selected page
 */
PropertySheetDlg::PropertySheetDlg(LPCTSTR sCaption, CWnd * pParentWnd, UINT uSelectPage)
  :CPropertySheet(sCaption, pParentWnd, uSelectPage)
{
} // constructor


/**
 * Destructor 
 */
PropertySheetDlg::~PropertySheetDlg()
{
  if (m_pageFont.m_hObject)
    VERIFY(m_pageFont.DeleteObject());
} // destructor


/**
 * message map
 */
BEGIN_MESSAGE_MAP(PropertySheetDlg, CPropertySheet)
  //{{AFX_MSG_MAP(PropertySheetDlg)
  //}}AFX_MSG_MAP
  ON_MESSAGE(WM_RESIZEPAGE, OnResizePage) 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropertySheetDlg message handlers


/**
 * Initializes font
 */
void PropertySheetDlg::BuildPropPageArray()
{
  CPropertySheet::BuildPropPageArray();

  // get first page
  CPropertyPage * pPage = GetPage(0);
  ASSERT(pPage);
  
  // dialog template class in afxpriv.h
  CDialogTemplate dlgTemplate;
  // load the dialog template
  VERIFY(dlgTemplate.Load(pPage->m_psp.pszTemplate));
  // get the font information
  CString sFontFace;
  WORD  wFontSize;
  VERIFY(dlgTemplate.GetFont(sFontFace, wFontSize));
  if(m_pageFont.m_hObject)
    VERIFY(m_pageFont.DeleteObject());
  // create a font using the info from first page
  VERIFY(m_pageFont.CreatePointFont(wFontSize*10, sFontFace));
} // BuildPropPageArray()


/**
 * Dialog initializaton, initialize skin buttons and set font
 * 
 * @return     Success of dialog init
 */
BOOL PropertySheetDlg::OnInitDialog() 
{
  CPropertySheet::OnInitDialog();

  SkinButton * vpSkinButtons[] = { &m_okBtn, &m_cancelBtn, &m_applyBtn, &m_helpBtn };
  CWnd * vpCButtons[] = { GetDlgItem(IDOK), GetDlgItem(IDCANCEL), GetDlgItem(ID_APPLY_NOW), GetDlgItem(IDHELP) };

  // want SkinButtons for the dialog, so replace each button with the SkinButton equivalent
  for (int4 iBtns = 0; iBtns < (sizeof(vpCButtons) / sizeof(CWnd *)); iBtns++)
  {
    if (vpCButtons[iBtns] == (CButton *) NULL)
      continue;

    CString sButtonTitle; vpCButtons[iBtns]->GetWindowText(sButtonTitle);
    DWORD dwStyle(vpCButtons[iBtns]->GetStyle() | BS_OWNERDRAW);
    CRect rect;     vpCButtons[iBtns]->GetWindowRect(rect);    ScreenToClient(rect);
    UINT uID(vpCButtons[iBtns]->GetDlgCtrlID());

    vpCButtons[iBtns]->ShowWindow(SW_HIDE);
    vpCButtons[iBtns]->EnableWindow(FALSE);
    // Unsure if it is necessary to destroy the old button
    // vpCButtons[iBtns]->DestroyWindow();

    vpSkinButtons[iBtns]->Create(sButtonTitle, dwStyle, rect, this, uID);
  }

  // get the font for the first active page
  CPropertyPage * pPage = GetActivePage();
  ASSERT(pPage);

  // change the font for the sheet
  ChangeDialogFont(this, &m_pageFont, CDF_CENTER);
  // change the font for each page
  for (int4 iCntr = 0; iCntr < GetPageCount(); iCntr++)
  {
    VERIFY(SetActivePage(iCntr));
    CPropertyPage * pPage = GetActivePage();
    ASSERT(pPage);
    ChangeDialogFont(pPage, &m_pageFont, CDF_CENTER);
  }

  VERIFY(SetActivePage(pPage));

  // set and save the size of the page
  CTabCtrl * pTab = GetTabControl();
  ASSERT(pTab);

  // CPropertyDialog::m_psh is used to store/set the characteristics of the dialog
  if (m_psh.dwFlags & PSH_WIZARD)
  {
    pTab->ShowWindow(SW_HIDE);
    GetClientRect(&m_pageRect);

    CWnd * pButton = GetDlgItem(ID_WIZBACK);
    ASSERT(pButton);
    CRect rc;
    pButton->GetWindowRect(&rc);
    ScreenToClient(&rc);
    m_pageRect.bottom = rc.top-2;
  }
  else
  {
    pTab->GetWindowRect(&m_pageRect);
    ScreenToClient(&m_pageRect);
    pTab->AdjustRect(FALSE, &m_pageRect);
  }

  // resize the page  
  pPage->MoveWindow(&m_pageRect);

  return TRUE;
} // OnInitDialog()


/**
 * Called when activating a new page; posts page resize message
 * 
 * @param wParam    Unused
 * @param lParam    Pointer to notify message header
 * @param pResult   Result code (unused)
 * @return          Value of base class OnNotify
 */
BOOL PropertySheetDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT * pResult)
{
  NMHDR * pNmh = (LPNMHDR) lParam;

  // the sheet resizes the page whenever it is activated so we need to size it correctly
  if (TCN_SELCHANGE == pNmh->code)
    PostMessage(WM_RESIZEPAGE);
  
  return CPropertySheet::OnNotify(wParam, lParam, pResult);
} // OnNotify()


/**
 * Resize page message handler
 * 
 * @param UINT   Unused
 * @param LONG   Unused
 * @return       Zero
 */
LONG_PTR PropertySheetDlg::OnResizePage(WPARAM, LPARAM)
{
  // resize the page
  CPropertyPage * pPage = GetActivePage();
  ASSERT(pPage);
  pPage->MoveWindow(&m_pageRect);

  return 0;
} // OnResizePage()


/**
 * Posts resize message for property or wizard button click events
 * 
 * @param wParam   Identifies button pressed
 * @param lParam   lparam for message
 * @return         Return code from base class OnCommand() (whether command was handled)
 */
BOOL PropertySheetDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  // the sheet resizes the page whenever the Apply button is clicked so we need to size it correctly
  if ((ID_APPLY_NOW == wParam) || (ID_WIZNEXT == wParam) || (ID_WIZBACK == wParam))
    PostMessage(WM_RESIZEPAGE);
  
  return CPropertySheet::OnCommand(wParam, lParam);
} // OnCommand()


// Revision History:
// $Log: PropertySheetDlg.C,v $
// Revision 1.4  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.3  2007/03/08 22:22:20  romy
// code review - header files adjusted
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1  2002/09/25 18:55:48  jmeade
// Code standards.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:44   ingmar
// Initial revision.
// Revision 1.3  2001/05/11 21:13:52  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PropertySheetDlg.C,v 1.4 2007/03/10 06:09:14 jmeade Exp $
// $Id: PropertySheetDlg.C,v 1.4 2007/03/10 06:09:14 jmeade Exp $
