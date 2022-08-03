// $Id: PreferencesDlg.h,v 1.11.2.2 2009/11/24 22:24:28 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Application preferences property sheet dialog
// Owner: Jeffrey Meade


#if !defined(AFX_PREFERENCESDLG_H__8841C2A4_41D1_42F4_98FB_7F9C784B2B35__INCLUDED_)
#define AFX_PREFERENCESDLG_H__8841C2A4_41D1_42F4_98FB_7F9C784B2B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// PreferencesDlg.h : header file
//


/// Includes
#include "ViatronDialog.h"
#include "PropertySheetDlg.h"
#include "ReportSpinEdit.h"
#include "resource.h"
#include "BackgroundBrush.h"

/// ColonViews namespace
namespace ColonViews
{

  /// Dialogs namespace 
  namespace Dialogs
  {

    /**
     * Preferences dialog base class
     */
    class PreferencesDlg : public WinUtils::PropertySheetDlg
    {
      DECLARE_DYNAMIC(PreferencesDlg)
        
    public:

      /**
       * Preferences Dialog Page used as a base class for all other preferences pages
       * defined below.
       */
      class PreferencesDlgPage : public CPropertyPage
      {

      public:

        /// Constructor
        PreferencesDlgPage(UINT uIDD);

        /// Destructor
        ~PreferencesDlgPage();

        virtual void OnResetPage() {}
        
        //{{AFX_VIRTUAL(PreferencesDlgPage)
      protected:
        //}}AFX_VIRTUAL
        /// message processing
        virtual LRESULT WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
        
        // Implementation
      protected:
        // Generated message map functions
        //{{AFX_MSG(PreferencesDlgPage)
        //}}AFX_MSG
        /// resize
        afx_msg void OnSize(UINT uType, int iCX, int iCY);
        /// control colour message
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
        DECLARE_MESSAGE_MAP()
          
      private:

        /// Background brush
        WinUtils::BackgroundBrush m_bkgdBrush;

      };  // class PreferencesDlgPage
      

      /**
       * Network Preferences dialog 
       */
      class NetworkPage : public PreferencesDlgPage
      {
      public:
        /// Construction
        NetworkPage();
        /// Destruction
        ~NetworkPage();
        /// store changes
        void SaveValues();

        // Dialog Data
        //{{AFX_DATA(NetworkPage)
        enum { IDD = IDP_PREFS_NETWORK };
        //}}AFX_DATA


        // Overrides
        // ClassWizard generate virtual function overrides
        //{{AFX_VIRTUAL(NetworkPage)
      public:
        /// apply changes
        virtual BOOL OnApply();
        /// cancel changes and close
        virtual void OnCancel();
        /// apply changes and close
        virtual void OnOK();
      protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);
        //}}AFX_VIRTUAL

        // Implementation
      protected:
        // Generated message map functions
        //{{AFX_MSG(NetworkPage)
        /// initializations
        virtual BOOL OnInitDialog();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

      };  // class NetworkPage


      /**
       * General Preferences dialog 
       */
      class GeneralPage : public PreferencesDlgPage
      {
      public:
        /// Construction
        GeneralPage();
        /// Destruction
        ~GeneralPage();

        /// reset msg
        virtual void OnResetPage();

        // Overrides
        // ClassWizard generate virtual function overrides
        //{{AFX_VIRTUAL(GeneralPage)
      protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);
        /// initializations
        virtual BOOL OnInitDialog();
        /// apply changes
        virtual BOOL OnApply();
        /// apply changes and close
        virtual void OnOK();
        /// horizontal scrollbar msg
        virtual void OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar);
        /// mouse moved
        virtual afx_msg void OnMouseMove(UINT uFlags, CPoint point);
        //}}AFX_VIRTUAL

        // Implementation
      protected:
        // Generated message map functions
        //{{AFX_MSG(GeneralPage)
        /// test database connection
        afx_msg void OnTestConnection();
        /// check box clicked
        afx_msg void OnCadDefaultCheckClicked();
        /// check box clicked
        afx_msg void OnChangeMonitorClicked();
        /// monitor detection selection changed
        afx_msg void OnSelChangeDetectWidescreen();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

      private:
        // Dialog Data
        //{{AFX_DATA(GeneralPage)
        enum { IDD = IDP_PREFS_GENERAL };
        /// logging level
        CComboBox m_comboLogLevel;
        /// list of available monitors
        CComboBox m_comboMonitor;
        /// cad default value
        CSliderCtrl m_cadSlider;
        /// display string
        CStatic m_display;
        /// enable/disable default value
        CButton m_btnCadDefEnabled;
        /// Selection of widescreen detection
        CComboBox m_comboWidescreen;
        //}}AFX_DATA

        /// Saves the real current database selected in the software
        int4 m_iInitialDSNSelection;

      };  // class GeneralPage


      /**
       * Defaults Preferences dialog 
       */
      class DefaultsPage : public PreferencesDlgPage
      {
      public:
        /// Construction
        DefaultsPage();

        /// Destruction
        ~DefaultsPage();

        /// reset msg
        virtual void OnResetPage();

        // Overrides
        // ClassWizard generate virtual function overrides
        //{{AFX_VIRTUAL(GeneralPage)
      protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);

        /// initializations
        virtual BOOL OnInitDialog();

        /// apply changes
        virtual BOOL OnApply();

        /// apply changes and close
        virtual void OnOK();

        /// cancel changes and close
        virtual void OnCancel();

        /// horizontal scrollbar msg
        virtual void OnHScroll(UINT uSBCode, UINT uPos, CScrollBar* pScrollBar);

        //}}AFX_VIRTUAL

        // Implementation
      protected:
        // Generated message map functions
        //{{AFX_MSG(DefaultsPage)
       
        /// handles the apply button clicked event
        afx_msg void OnApplyClickedHandler();

        /// handles the apply button clicked event
        afx_msg void OnSelectionChangedHandler();

        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

      private:

        /// sets up the combobox and associated label
        void SetupComboPreference( CComboBox & comboBox, UINT iLabelControl, UINT iLabelText, UINT iOptions, bool bSetting );

        /// sets up the combobox and associated label
        void SetupComboPreference( CComboBox & comboBox, UINT iLabelControl, UINT iLabelText, UINT iOptions, int4 iSetting );

        /// set static text
        void SetStaticText( UINT iLabelControl, CString sText );

      private:
        // Dialog Data
        //{{AFX_DATA(DefaultsPage)
        enum { IDD = IDP_PREFS_DEFAULTS };

        /// layout combobox
        CComboBox m_layoutComboBox;

        /// slider for speed control
        CSliderCtrl m_flightSpeedSlider;

        /// flight speed label
        CStatic m_flightSpeedLabel;

        /// mouse coord combobox
        CComboBox m_mouseCoordComboBox;

        /// annotation edit box
        CEdit m_annotationEdit;

        /// scrollbar orientation combobox
        CComboBox m_scrollbarComboBox;

				/// tool bar position combobox
				CComboBox m_toolbarPositionComboBox;

        /// extra-colonic findings combobox
        CComboBox m_ecFindingsComboBox;

        /// cleansed combobox
        CComboBox m_cleansedComboBox;

        /// linear measurement combobox
        CComboBox m_measurementComboBox;

        /// color scheme combobox
        CComboBox m_colorSchemeComboBox;

        /// pseudo color combobox
        CComboBox m_pseudoColorComboBox;

        /// volume measure combobox
        CComboBox m_volumeMeasureComboBox;

        /// link datasets combobox
        CComboBox m_linkDatasetsComboBox;

        /// centerline combobox
        CComboBox m_centerlineComboBox;

        /// applies the defaults
        CButton m_applyButton;

        /// slider for fov
        CSliderCtrl m_fovSlider;

        /// fov label
        CStatic m_fovLabel;

        bool m_bPreferencesChanged;
        //}}AFX_DATA

      };  // class GeneralPage


      /**
       * Report Preferences dialog 
       */
      class ReportPage : public PreferencesDlgPage
      {
      public:
        /// Construction
        ReportPage();
        /// destruction
        ~ReportPage();

        /// reset msg
        virtual void OnResetPage();

        //{{AFX_VIRTUAL(ReportPage)
      protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);

        /// initializations
        virtual BOOL OnInitDialog();
        /// apply changes
        virtual BOOL OnApply();
        /// apply changes and close
        virtual void OnOK();
        //}}AFX_VIRTUAL
        
        /// Creates a spin edit 
        void CreateSpinEdit(const UINT uID, ColonReport::ReportSpinEdit & wndSpin, const float4 fInitial);

        /// Saves values from page
        void SaveValues();
        
        //{{AFX_MSG(ReportPage)
        //}}AFX_MSG
        /// no header
        afx_msg void OnRbHeaderNone();
        /// header text
        afx_msg void OnRbHeaderDefined();
        /// image header
        afx_msg void OnRbHeaderBitmap();
        /// transfer bitmap header
        afx_msg void OnTransfer();
        DECLARE_MESSAGE_MAP()

        /// Event handler for the report header selection changed event
        void OnReportHeaderSelectionChanged(const int4 iRadioID);
        
      public:
        /// top margin value
        ColonReport::ReportSpinEdit m_topMarginSpin;
        /// bottom margin value
        ColonReport::ReportSpinEdit m_bottomMarginSpin;
        // Dialog Data
        //{{AFX_DATA(ReportPage)
        /// dialog ID
        enum { IDD = IDP_PREFS_REPORT };
        /// transfer image button
        WinUtils::SkinButton  m_transferBtn;
        //}}AFX_DATA

      private:

        /// Font for dialog
        CFont m_font;

      };  // class ReportPage


      public:

        /// Constructor
        PreferencesDlg(UINT uIDCaption, CWnd* pParentWnd = NULL, UINT uSelectPage = 0);

        /// Constructor
        PreferencesDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT uSelectPage = 0);

        /// Destructor
        virtual ~PreferencesDlg();
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PreferencesDlg)
      public:
        virtual BOOL OnInitDialog();
        //}}AFX_VIRTUAL
        
        // Generated message map functions
      protected:
        //{{AFX_MSG(PreferencesDlg)
        /// resize
        afx_msg void OnSize(UINT nType, int iCX, int iCY);
        /// control colour
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT uCtlColor);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
          
      private:
        /// initializations
        void SetupDialog();

        /// handles the reset button clicked event
        afx_msg void OnResetClickedHandler();
        
      public:

        /// Network Page Preferences Dialog
        NetworkPage m_networkPage;
        
        /// General Page Preferences Dialog
        GeneralPage m_generalPage;

        /// Report Page Preferences Dialog
        ReportPage m_reportPage;

        /// Defaults Page Preferences Dialog
        DefaultsPage m_defaultsPage;
        
      private:

        /// Background brush
        WinUtils::BackgroundBrush m_bkgdBrush;

        /// reset the defaults
        WinUtils::SkinButton m_resetButton;

    };  // class PreferencesDlg

  } // namespace Dialogs

} // namespace ColonViews

#endif // !defined(AFX_PREFERENCESDLG_H__8841C2A4_41D1_42F4_98FB_7F9C784B2B35__INCLUDED_)


// Revision History:
// $Log: PreferencesDlg.h,v $
// Revision 1.11.2.2  2009/11/24 22:24:28  dongqing
// user can choose whether to auto-detect widescreen
//
// Revision 1.11.2.1  2008/04/23 18:05:11  jmeade
// multi-monitor support.
//
// Revision 1.11  2007/03/10 06:09:14  jmeade
// code standards.
//
// Revision 1.10  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.9  2006/08/24 01:27:00  jmeade
// disable default cad slider when not needed.
//
// Revision 1.8  2006/07/20 00:29:16  jmeade
// Issue 4885: implemented checkbox for user cad default.
//
// Revision 1.7  2006/01/31 17:30:14  mkontak
// Coding standards
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.2.3  2006/09/21 00:32:38  jmeade
// Issue #4926 (duplicating fix done in #4885, revisions 1.8,1.9): implement cad default slider and checkbox.
//
// Revision 1.5.2.2  2006/01/10 02:25:00  jmeade
// tabs to spaces.
//
// Revision 1.5.2.1  2006/01/10 02:18:26  jmeade
// Issue 4594: User-preferred cad slider default.
//
// Revision 1.5  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.4  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
//
// Revision 1.3  2005/08/16 21:17:33  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3.10.1.12.1  2005/06/17 13:14:24  geconomos
// using new BackgroundBrush class
//
// Revision 3.3.10.1  2003/02/25 17:57:28  jmeade
// Removed obsolete class.
//
// Revision 3.3  2002/05/15 20:49:11  jmeade
// Allow for testing connection to a database provider before selecting.
//
// Revision 3.2  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.1.2.1  2002/02/22 23:10:08  jmeade
// Log level system setting.
//
// Revision 3.1  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.2  2001/10/28 20:56:23  jmeade
// Choice of database provider implemented fully;
// Remainder of settings using the ViatronSettings::Setting class;
//  Coding standards
//
// Revision 3.0.2.1  2001/10/26 20:46:53  jmeade
// Now showing!  The Preferences General Page!  Allows selection of database DSN.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 25 2001 19:59:02   jmeade
// Removed obsolete share names preferences and empty general preferences
// 
//    Rev 2.0   Sep 16 2001 23:41:38   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:26   ingmar
// Initial revision.
// Revision 1.10  2001/09/06 16:49:03  jmeade
// Coding conventions
//
// Revision 1.9  2001/09/05 16:56:28  jmeade
// Cleaning up interaction on ReportPage
//
// Revision 1.8  2001/04/13 02:34:11  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/PreferencesDlg.h,v 1.11.2.2 2009/11/24 22:24:28 dongqing Exp $
// $Id: PreferencesDlg.h,v 1.11.2.2 2009/11/24 22:24:28 dongqing Exp $
