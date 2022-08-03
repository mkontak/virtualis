// $Id: NotationsDialog.h,v 1.6.6.1 2010/04/11 11:40:28 dongqing Exp $
//
// Copyright © 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


// pragmas
#pragma once


// includes
#include "ViatronDialog.h"
#include "resource.h"
#include "BookmarkList.h"
#include "MeasureLineList.h"
#include <list>


// namespaces
namespace ColonViews
{

  namespace Dialogs
  {

    class NotationsDialog : public ViatronDialog
    {
    public:

      /// Constructor
      NotationsDialog(CWnd* pParent = NULL);

    // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(NotationsDialog)
      protected:
      virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      //}}AFX_VIRTUAL

    // Implementation
    protected:

      // Generated message map functions
      //{{AFX_MSG(NotationsDialog)
      virtual BOOL OnInitDialog();
      afx_msg void OnSelChangeSupineCombo();
      afx_msg void OnSelChangeProneCombo();
      afx_msg void OnDropdownSupineCombo();
      afx_msg void OnDropdownProneCombo();
      afx_msg void OnClickedSupineDelete();
      afx_msg void OnClickedProneDelete();
      afx_msg void OnDeleteAll();
      afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
      afx_msg void OnTimer(UINT_PTR nIDEvent);
      afx_msg void OnCloseupCombos();
      afx_msg void OnSize(UINT uType, int iSizeX, int iSizeY);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

    private:

      /// Update the contents of the given combobox with the list.
      template<class Type> void UpdateComboList(CComboBox& combo, const std::list<Type>& theList);

      /// Deletes a bookmark annotation
      void DeleteAnnotation(const int4 iDel, const CComboBox& combo, ReaderLib::BookmarkList& theList, const bool bBookmark);

      /// Deletes the selected measurement
      void DeleteMeasurement( const int4 iSelection, MeasureLineList & measureLineList );

    private:
    // Dialog Data
      //{{AFX_DATA(NotationsDialog)
      enum { IDD = IDD_NOTATIONS };
      WinUtils::SkinAutoRadioButton m_btnMeasurements;
      WinUtils::SkinAutoRadioButton m_btnAnnotations;
      WinUtils::SkinAutoRadioButton m_btnBookmarks;
      WinUtils::SkinButton m_btnDeleteAll;
      CComboBox m_cbxSupine;
      CComboBox m_cbxProne;
      CButton m_btnSupineDelete;
      CButton m_btnProneDelete;
      //}}AFX_DATA
      /// AFX_DATA member from SkinAutoRadioButton (not MFC), sets/stores the current notation's list selection
      int4 m_iListSel;

      /// There are this number of invalid items in the comboboxes
      enum { eComboCountMin = 1 };

      /// Values of the notations selection radios
      enum { eRadio_Bookmarks = 0, eRadio_Annotations = 1, eRadio_Measurements = 2 };
      
      /// ID of timer event that triggers update of dialog
      int4 m_iUpdateDlgTimerEvent;

    }; // class NotationsDialog

  } // namespace Dialogs

} // namespace ColonViews

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: NotationsDialog.h,v $
// Revision 1.6.6.1  2010/04/11 11:40:28  dongqing
// centering child controls
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6  2006/01/31 14:52:31  frank
// split up volume measure file
//
// Revision 1.5  2006/01/31 14:50:38  frank
// split up volume measure file
//
// Revision 1.4  2006/01/31 14:26:44  frank
// code review
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.2.8.4.1  2005/03/26 03:14:52  frank
// Issue #4063: added measurement deletion, fixed button enabling
//
// Revision 1.1.2.8  2004/03/22 21:57:45  jmeade
// Issue 3776:  Delete button in notations dialog.
//
// Revision 1.1.2.7  2003/04/14 16:31:39  jmeade
// Issue 2841:  Entries for measurements list.
//
// Revision 1.1.2.6  2003/03/21 19:27:18  jmeade
// Issue 2841: Ensure via timer that combo enable state always current.
//
// Revision 1.1.2.5  2003/03/10 21:54:17  jmeade
// Issue 2841: Basic look and functionality implemented.
//
// Revision 1.1.2.4  2003/03/07 02:50:27  jmeade
// Issue 2882: Use default brush to prevent wrapping in background brush for combobox list.
//
// Revision 1.1.2.3  2003/02/28 20:47:14  jmeade
// Comments.
//
// Revision 1.1.2.2  2003/02/27 23:54:49  jmeade
// Issue 2841:  Lists now made functional.
//
// Revision 1.1.2.1  2003/02/27 04:04:19  jmeade
// Issue 2841: Notations pane (annotations and bookmarks).
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/NotationsDialog.h,v 1.6.6.1 2010/04/11 11:40:28 dongqing Exp $
// $Id: NotationsDialog.h,v 1.6.6.1 2010/04/11 11:40:28 dongqing Exp $
