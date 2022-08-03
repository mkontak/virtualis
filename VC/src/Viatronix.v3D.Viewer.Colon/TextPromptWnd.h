// $Id: TextPromptWnd.h,v 1.4 2007/03/09 00:11:04 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: TextPromptWnd.h
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_TEXTPROMPTWND_H__D3EA1AFB_02B0_4569_B8E9_CED63E53D04A__INCLUDED_)
#define AFX_TEXTPROMPTWND_H__D3EA1AFB_02B0_4569_B8E9_CED63E53D04A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "typedef.h"

namespace WinUtils
{

  /**
   * Class for in-memory creation of a dialog.
   */
  class DialogTemplate
  {
  public:
    /// Constructor
    DialogTemplate(const std::string& sCaption, DWORD style, int x, int y, int w, int h, const std::string& sFont = std::string(), WORD iFontSize = 8);
    /// Destructor.
    virtual ~DialogTemplate();

    /// Adds a component to the dialog (called by other add functions).
    void AddComponent(const std::string& type, const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a button to the dialog.
    void AddButton(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a editbox to the dialog.
    void AddEditBox(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a static text field to the dialog.
    void AddStatic(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a listbox to the dialog.
    void AddListBox(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a scrollbar to the dialog.
    void AddScrollBar(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Adds a combobox to the dialog.
    void AddComboBox(const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);

    /// Returns a pointer to the Win32 dialog template. Do not store for use, as it become invalid if more items added.
    operator LPCDLGTEMPLATE() const;

  protected:

    /// Adds a standard windows component.
    void AddStandardComponent(WORD type, const std::string& caption, DWORD style, DWORD exStyle, int x, int y, int w, int h, WORD id);
    /// Aligns data at 8-byte boundary (needed for dialog resource templates).
    void AlignData(int size);
    /// Appends a string.
    void AppendString(const std::string& sString);
    /// Appends general data.
    void AppendData(void* data, int dataLength);
    /// Reallocates m_pDialogTemplate if necessary.
    void EnsureSpace(int length);
  private:

    /// Pointer to Win32 dialog template.
    LPDLGTEMPLATE m_pDialogTemplate;

    /// Total length of m_pDialogTemplate.
    int4 m_iTotalBufferLength;
    /// Amount of used memory in m_pDialogTemplate.
    int4 m_iUsedBufferLength;
  }; // DialogTemplate


  /////////////////////////////////////////////////////////////////////////////
  // TextPromptWnd dialog

  /// Prompt user for text
  class TextPromptWnd : public CDialog
  {
  public:
    /// Type of edit field for text entry
    enum TextBoxTypeEnum { eSingleLine, eMultiLine };

  public:
    /// Prompts user for text with <sPrompt> and returns the entered text, or an empty string if no text or cancelled
    static std::string GetText(const std::string& sPrompt, const std::string& sInitialText = std::string(),
      const TextBoxTypeEnum eType = eSingleLine, CWnd* pParent = NULL);

  private:
    /// Construction
    TextPromptWnd();

  /// Dialog Data
    //{{AFX_DATA(TextPromptWnd)
      // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA


  /// Overrides
    /// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(TextPromptWnd)
    protected:
      /// DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      /// on submission
      virtual void OnOK();
    //}}AFX_VIRTUAL

  /// Implementation
  protected:

    /// Generated message map functions
    //{{AFX_MSG(TextPromptWnd)
      // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  private:
    /// ID for the text box
    enum { eTextBox_DlgID = 1000 };
    /// Returned user text
    std::string m_sUserText;
  }; // TextPromptWnd

} // namespace WinUtils

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPROMPTWND_H__D3EA1AFB_02B0_4569_B8E9_CED63E53D04A__INCLUDED_)

// Revision History:
// $Log: TextPromptWnd.h,v $
// Revision 1.4  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.3  2005/08/16 22:04:48  jmeade
// namespaces for WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.2  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.1  2002/06/04 18:37:00  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 1.1.2.3  2002/06/03 23:22:19  jmeade
// Code standards and comments.
//
// Revision 1.1.2.2  2002/05/23 21:32:28  jmeade
// First working revision.
//
// Revision 1.1.2.1  2002/05/23 15:42:29  jmeade
// Initial revision.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/TextPromptWnd.h,v 1.4 2007/03/09 00:11:04 jmeade Exp $
// $Id: TextPromptWnd.h,v 1.4 2007/03/09 00:11:04 jmeade Exp $
