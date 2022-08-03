// $Id: RegistrationView.h,v 1.14.2.4 2010/04/17 05:17:01 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: RegistrationView.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_REGISTRATIONCOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_)
#define AFX_REGISTRATIONCOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// RegistrationView.h : header file
//

// Includes
#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"


/////////////////////////////////////////////////////////////////////////////
// RegistrationLayoutFrame frame

namespace ColonViews
{

  namespace Layouts
  {
    /// dual-dataset registration
    class RegistrationLayoutFrame : public ChildLayoutFrame
    {
    public:
  
      /// Constructor
      RegistrationLayoutFrame();
      /// destructor
      virtual ~RegistrationLayoutFrame() {}


    protected:

      DECLARE_DYNCREATE(RegistrationLayoutFrame)

      DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////


    /// dual-dataset registration
    class RegistrationLayout : public ColonViatronixWnd
    {

    public:

      /// Constructor
      RegistrationLayout();

      /// Destructor
      ~RegistrationLayout();

    // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(RegistrationLayout)
      public:
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
      virtual BOOL PreTranslateMessage(MSG* pMsg);
      //}}AFX_VIRTUAL

      /// Is datasets locking tracked by this view
      virtual bool IsDatasetsLockingEnabled() const;

      /// this view displays multiple datasets
      virtual bool IsDisplayingMultipleDatasets() const { return true; }

    protected:
      /// Creates child views
      virtual void CreateChildViews(HWND hWndThis);

      /// Renders the views
      void GLRenderViews();

      /// Idle time processing
      void GLIdle();

      /// sets the text in the pane titles
      void SetDatasetPaneTitles();

      /// create event handler
      afx_msg virtual int OnCreate( LPCREATESTRUCT lpCreateStruct );

      /// destroy event handler
      afx_msg virtual void OnDestroy();

      /// Event handler for the Key Down event
      afx_msg void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// Event handler for the Right Button Down Event
      afx_msg void OnRButtonDown(UINT uFlags, CPoint point);

      /// Event handler for the Left Button Double CLick event
      afx_msg void OnLButtonDblClk(UINT uFlags, CPoint origPt);

      /// Event handler for the Mouse MOve event
      afx_msg void OnMouseMove(UINT uFlags, CPoint point);

      /// Add new bookmark
      afx_msg void OnBookmarksAddBookmark();

      /// Jump to the previous segment
      afx_msg void OnJumpPrevSegment();

      /// Jump to the next segment
      afx_msg void OnJumpNextSegment();

      /// Toggle line measure mode
      afx_msg void OnToggleLineMeasure();
      /// Toggle lighting mode
      afx_msg void OnToggleLighting();
      /// Toggle roi measure mode
      afx_msg void OnToggleRoiMeasure();
      /// Toggle translucent mode
      afx_msg void OnToggleTranslucent();

      /// Add new report snapshot
      afx_msg void OnReportAddSnapshot();

      /// Child view application message
      virtual afx_msg LRESULT OnAppChildViewMessage(WPARAM wParam, LPARAM lParam);

      /// Autofly mode toggle
      afx_msg void OnToggleAutofly();

      /// view dir toggle
      afx_msg void OnToggleViewDirection();

      /// indicates that the supine/prone button should be enabled in the PatientInfo dialog
      LRESULT OnQueryEnableSupineProneSwitch(WPARAM, LPARAM);

      /// Update overview on autofly message
      afx_msg LRESULT OnUpdate2DAutoFlight(WPARAM wParam, LPARAM lParam);

      /// Event handler for the Initialized event
      virtual void OnInitialized();

      /// mfc message map
      DECLARE_MESSAGE_MAP()

      /// WM_SIZE handler
      afx_msg virtual void OnSize(UINT uType, int cx, int cy);

      /// WM_PAINT handler
      void OnPaint();

      /// derived classes can override to remove areas from the parent region
      virtual void OnUpdateBackgroundRegion( CRgn & parentRgn );

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID();
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID();
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID();
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID();

      /// handles datasets locked changes
      virtual void OnDatasetsLocked( const Colon::DatasetsLockedArgs & args );

    /// Data members
    private:

      /// Flag to indicate if the current registered position of the studies need updating.
      enum UpdateRegistrationEnum 
      { 
        eUpdateNone     = 0x0000,
        eUpdateProne2d  = 0x0001,
        eUpdateSupine2d = 0x0002,
        eUpdateProne3d  = 0x0004, 
        eUpdateSupine3d = 0x0008
      };
      
      /// Update region position
      uint4 m_eUpdateRegPosn;
      
      /// indicates if the mouse is over the specified pane control
      bool IsMouseOverPaneControl(  CPoint & point, CWnd & wndPane );

      /// Primary overview window
      GLWnd * m_pPrimaryOverviewWnd;

      /// Dual overview window
      GLWnd * m_pDualOverviewWnd;

      /// Middle mouse down flag
      bool m_bMiddleMouseDown;

      /// Middle mouse supine flag
      bool m_bMiddleMouseSupine;

      /// indicates if the panes have been initialized already
      bool m_bPanesInitialized;

      /// bypass the lookup
      bool m_bBypassDatasetLockingCheck;

    };  // class RegistrationViewFrame

  } // namespace Layouts

} // namespace ColonViews

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTRATIONCOLONWND_H__C583B9A6_9374_4281_8D9F_B7A8DE44AF0E__INCLUDED_)
