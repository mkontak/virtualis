// $Id: TextPromptWnd.C,v 1.3 2007/03/09 00:11:04 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: TextPromptWnd.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "TextPromptWnd.h"
#include "WinGLUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace WinUtils;

///////////////////////////////////////////////////////////////////////////////////////////
// class DialogTemplate


/**
 * Constructor
 *
 * @param sCaption    Caption
 * @param dwStyle     Window style.
 * @param x           X-pos.
 * @param y           Y-pos.
 * @param w           Width.
 * @param h           Height.
 * @param sFont       Font name.
 * @param iFontSize   Font size.
 */
DialogTemplate::DialogTemplate(const std::string& sCaption, DWORD dwStyle, int4 x, int4 y, int4 w, int4 h,
                               const std::string& sFont, WORD iFontSize)
{
  m_iUsedBufferLength = sizeof(DLGTEMPLATE);
  m_iTotalBufferLength = m_iUsedBufferLength;

  m_pDialogTemplate = new DLGTEMPLATE[m_iTotalBufferLength];

  m_pDialogTemplate->style = dwStyle;

  if (!sFont.empty())
  {
    m_pDialogTemplate->style |= DS_SETFONT;
  }

  m_pDialogTemplate->x     = x;
  m_pDialogTemplate->y     = y;
  m_pDialogTemplate->cx    = w;
  m_pDialogTemplate->cy    = h;
  m_pDialogTemplate->cdit  = 0;

  m_pDialogTemplate->dwExtendedStyle = 0;

  // The dialog box doesn't have a menu or a special class

  AppendData(_T("\0"), 2);
  AppendData(_T("\0"), 2);

  // Add the dialog's caption to the template

  AppendString(sCaption);

  if (!sFont.empty())
  {
    AppendData(&iFontSize, sizeof(WORD));
    AppendString(sFont);
  }
} // constructor()


/**
 * Destructor 
 */
DialogTemplate::~DialogTemplate()
{
  delete m_pDialogTemplate;
} // destructor()


/**
 * add a dialog control
 *
 * @param sType       type of control
 * @param sCaption    text for control
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddComponent(const std::string& sType, const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  DLGITEMTEMPLATE item;

  item.style = dwStyle;
  item.x     = x;
  item.y     = y;
  item.cx    = w;
  item.cy    = h;
  item.id    = id;

  item.dwExtendedStyle = dwExStyle;

  AppendData(&item, sizeof(DLGITEMTEMPLATE));

  AppendString(sType);
  AppendString(sCaption);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

  // Increment the component count

  m_pDialogTemplate->cdit++;
} // AddComponent()


/**
 * add button to dialog
 *
 * @param sCaption    text for button
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddButton(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0080, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddButton()


/**
 * add editbox to dialog
 *
 * @param sCaption    text for edit
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddEditBox(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0081, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddEditBox()


/**
 * add static to dialog
 *
 * @param sCaption    text for field
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddStatic(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0082, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddStatic()


/**
 * add listbox to dialog
 *
 * @param sCaption    text for listbox (unused, only for component add function)
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddListBox(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0083, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddListBox()


/**
 * add scrollbar to dialog
 *
 * @param sCaption    text for scroll (unused, parameter required for component add function)
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddScrollBar(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0084, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddScrollBar()


/**
 * add button to dialog
 *
 * @param sCaption    text for combobox
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddComboBox(const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD id)
{

  AddStandardComponent(0x0085, sCaption, dwStyle, dwExStyle, x, y, w, h, id);

  WORD dwCreationDataLength = 0;
  AppendData(&dwCreationDataLength, sizeof(WORD));

} // AddComboBox()


/**
 * Cast operator
 *
 * @return   A pointer to the Win32 dialog template which the object represents.
 *           This pointer may become invalid if additional components are added to the template.
 */
DialogTemplate::operator LPCDLGTEMPLATE() const
{
  return m_pDialogTemplate;
} // LPCDLGTEMPLATE()


/**
 * generic control-add method
 *
 * @param wType       type of control
 * @param sCaption    text for button
 * @param dwStyle     window styles
 * @param dwExStyle   extended styles
 * @param x           x location
 * @param y           y location
 * @param w           width
 * @param h           height
 * @param id          ID
 */
void DialogTemplate::AddStandardComponent(WORD wType, const std::string& sCaption, DWORD dwStyle, DWORD dwExStyle, int4 x, int4 y, int4 w, int4 h, WORD iId)
{

  DLGITEMTEMPLATE item;

  // DWORD align the beginning of the component data

  AlignData(sizeof(DWORD));

  item.style = dwStyle;
  item.x     = x;
  item.y     = y;
  item.cx    = w;
  item.cy    = h;
  item.id    = iId;

  item.dwExtendedStyle = dwExStyle;

  AppendData(&item, sizeof(DLGITEMTEMPLATE));

  WORD wPreType = 0xFFFF;

  AppendData(&wPreType, sizeof(WORD));
  AppendData(&wType, sizeof(WORD));

  AppendString(sCaption);

  // Increment the component count

  m_pDialogTemplate->cdit++;

} // AddStandardComponent()


/**
 * Align data along >size> boundary
 */
void DialogTemplate::AlignData(int4 size)
{

  int4 iPaddingSize = m_iUsedBufferLength % size;

  if (iPaddingSize != 0)
  {
    EnsureSpace(iPaddingSize);
    m_iUsedBufferLength += iPaddingSize;
  }

} // AlignData()


/**
 * append string data to dialog template
 * @param sString   string to append
 **/
void DialogTemplate::AppendString(const std::string& sString)
{

  int4 iLength = MultiByteToWideChar(CP_ACP, 0, sString.c_str(), -1, NULL, 0);

  WCHAR* wideString = (WCHAR*)malloc(sizeof(WCHAR) * iLength);
  MultiByteToWideChar(CP_ACP, 0, sString.c_str(), -1, wideString, iLength);

  AppendData(wideString, iLength * sizeof(WCHAR));
  free(wideString);

} // AppendString()


/**
 * append data to dialog template
 *
 * @param pData         Data to be appended
 * @param iDataLength   length of data
 **/
void DialogTemplate::AppendData(void* pData, int4 iDataLength)
{

  EnsureSpace(iDataLength);

  memcpy((char*)m_pDialogTemplate + m_iUsedBufferLength, pData, iDataLength);
  m_iUsedBufferLength += iDataLength;

} // AppendData()


/**
 * ensure dialog template buffer is large enough
 *
 * @param iLength   necessary length for dialog template
 **/
void DialogTemplate::EnsureSpace(int4 iLength)
{

  if (iLength + m_iUsedBufferLength > m_iTotalBufferLength)
  {

    m_iTotalBufferLength += iLength * 2;

    void* newBuffer = malloc(m_iTotalBufferLength);
    memcpy(newBuffer, m_pDialogTemplate, m_iUsedBufferLength);

    free(m_pDialogTemplate);
    m_pDialogTemplate = (DLGTEMPLATE*)newBuffer;

  }

} // EnsureSpace()



/////////////////////////////////////////////////////////////////////////////
// TextPromptWnd dialog


/**
 * Constructor 
 */
TextPromptWnd::TextPromptWnd() : CDialog()
{
  //{{AFX_DATA_INIT(TextPromptWnd)
    // NOTE: the ClassWizard will add member initialization here
  //}}AFX_DATA_INIT
} // constructor()


/**
 * Dialog data variable initialization.
 *
 * @param pDX   Variable data.
 */
void TextPromptWnd::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(TextPromptWnd)
    // NOTE: the ClassWizard will add DDX and DDV calls here
  //}}AFX_DATA_MAP
} // DoDataExchange()


/**
 * Called when user clicks OK button.
 */
void TextPromptWnd::OnOK()
{
  CString s;
  GetDlgItemText(eTextBox_DlgID, s);
  m_sUserText = s;
  CDialog::OnOK();
} // OnOK()


/**
 * Prompts the user (sPrompt) to enter text, returning their entry.
 *
 * @param sPrompt        Text prompt.
 * @param sInitialText   Initial text in the edit field.
 * @param eType          Type of text box (single vs. multi-line edit).
 * @param pParent        Parent window.
 * @return               Entered text, or an empty string if dialog is canceled.
 */
std::string TextPromptWnd::GetText(const std::string& sPrompt, const std::string& sInitialText,
                                   const TextBoxTypeEnum eType /* = eSingleLine */, CWnd* pParent /* = NULL */)
{
  TextPromptWnd wnd;

  DialogTemplate dialogTemplate(AfxGetAppName(), WS_CAPTION | DS_CENTER, 10, 10, 200, 85, _T("Tahoma"), 12);

  DWORD dwEditStyle(WS_VISIBLE | WS_BORDER);
  if (eType == eMultiLine)  { dwEditStyle |= ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL; }
  dialogTemplate.AddEditBox(sInitialText.c_str(), dwEditStyle, 0, 2 + 3, 44, 186, 12 * ((eType==eSingleLine)?1:3), wnd.eTextBox_DlgID);

  dialogTemplate.AddStatic(sPrompt.c_str(), WS_VISIBLE, 0, 2 + 3, 7, 137, 36, -1);
  dialogTemplate.AddButton(WinGLUtils::Text::sOK, WS_VISIBLE, 0, 149 + 3, 7, 39, 14, IDOK);
  dialogTemplate.AddButton(WinGLUtils::Text::sCancel, WS_VISIBLE, 0, 149 + 3, 24, 39, 14, IDCANCEL);

  LRESULT res = wnd.InitModalIndirect(dialogTemplate);
  res = wnd.DoModal();

  return (res == IDOK) ? wnd.m_sUserText : std::string();
} // GetText()


BEGIN_MESSAGE_MAP(TextPromptWnd, CDialog)
//{{AFX_MSG_MAP(TextPromptWnd)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TextPromptWnd message handlers

// Revision History:
// $Log: TextPromptWnd.C,v $
// Revision 1.3  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.2  2005/08/16 22:04:48  jmeade
// namespaces for WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.3  2003/01/21 19:21:03  jmeade
// Branch merge: Revision : 1.1.2.3: Moving hard-coded strings to string table.
//
// Revision 3.2  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.1  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 1.1.2.2  2002/06/03 23:22:19  jmeade
// Code standards and comments.
//
// Revision 1.1.2.1  2002/05/23 21:32:28  jmeade
// First working revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TextPromptWnd.C,v 1.3 2007/03/09 00:11:04 jmeade Exp $
// $Id: TextPromptWnd.C,v 1.3 2007/03/09 00:11:04 jmeade Exp $
