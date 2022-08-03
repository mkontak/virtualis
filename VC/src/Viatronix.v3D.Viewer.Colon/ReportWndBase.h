// $Id: ReportWndBase.h,v 1.9.2.1 2007/08/22 15:32:30 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: George Economos (mailto:gecono@viatronix.com)


// pragmas
#pragma once


// imports
#if defined(_DEBUG)
# import "..\..\..\..\Libraries\com\ReportLibd.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "..\..\..\..\Libraries\com\ReportLib.dll" named_guids
#else
# import "..\..\..\..\Libraries\com\ReportLibdr.dll" named_guids
#endif


// includes
#include "WinUtils.h"
#include "ReportSpinEdit.h"
#include "BackgroundBrush.h"


// namespaces
namespace ColonReport
{

 
  /**
   * Description: Base class for all report windows
   */
  class ReportWndBase : public CDialog
  {
    // member functions
  public:

    /// constructor
    ReportWndBase(UINT uID, CWnd* pParent = NULL);

    /// destructor
    virtual ~ReportWndBase();

    /// displays the specified report error
    static void DisplayReportError(uint4 uErrorIDS, HRESULT hr);

    /// sets the report instance for the dialog
    virtual void SetReport(ReportLib::IReportRevisionPtr& spRevision) {m_spRevision = spRevision;}

    /// called when the view is being activated
    virtual void OnReportViewActivate(){};

    /// called when the view is being deactivated
    virtual void OnReportViewUnactivate(){};

  protected:

    /// gets the background bitmap for the specified child control
    HBITMAP GetChildBackground( UINT uID );

    /// creates a spin edit control with the specified properties
    void CreateSpinEdit(const UINT uID, ReportSpinEdit& wndSpin, const float4 fMin, const float4 fMax,
      const float4 fInitial = 0.0F, const float4 fStep = 1.0F, const float4 fPrecs = 0.0F, const bool bWrap = false);

    /// selectes an item in the specified combo box
    BOOL SelectComboItem(CComboBox& wndCombo, DWORD dwData);

    /// mfc message map
    DECLARE_MESSAGE_MAP()

    /// called by the framework to exchange and validate dialog data
    virtual void DoDataExchange( CDataExchange * pDX );

    /// WM_INITDIALOG handler
    virtual BOOL OnInitDialog();

    /// IDOK click handler
    virtual void OnOK();

    /// WM_PAINT handler
    virtual void OnPaint();    

    /// WM_CTLCOLOR handler
    afx_msg HBRUSH OnCtlColor( CDC * pDC, CWnd * pWnd, UINT uCtlColor );

    /// WM_SETCURSOR handler
    afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT uHitTest, UINT uMessage );

    /// WM_SIZE handler
    afx_msg void OnSize( UINT uType, int cx, int cy );

    /// WM_ERASEBKGND handler
    afx_msg BOOL OnEraseBkgnd( CDC * pdc );

    /// callback function for creating the update HREGION
    static BOOL CALLBACK UpdateBackgroundRegionCallback( HWND hWnd, LPARAM lParam );

  protected:

    /// current report revision
    ReportLib::IReportRevisionPtr m_spRevision;

    // contains scaled background image
    Gdiplus::Bitmap * m_pDrawBuffer;

    // pattern use for child controls
    HBRUSH m_hPatternBrush;

    // handle to the bitmap that the pattern brush is based on
    HBITMAP m_hBitmap;

    /// background brush for edit controls
    CBrush m_bkEdit;

    /// font used for displaying patient name
    CFont m_fontName;

    /// small font
    CFont m_fontSmall;

    /// normal font
    CFont m_fontNormal;

    /// bold font
    CFont m_fontBold;

    /// underline font
    CFont m_fontUnderline;

    /// back color
    COLORREF m_clrBack;

  }; // class ReportWndBase

} // namespace ColonReport


// $Log: ReportWndBase.h,v $
// Revision 1.9.2.1  2007/08/22 15:32:30  jmeade
// comments.
//
// Revision 1.9  2007/03/09 15:59:11  jmeade
// code standards.
//
// Revision 1.8  2006/01/31 15:44:52  geconomos
// updated for code review
//
// Revision 1.7  2006/01/31 14:02:31  frank
// code review
//
// Revision 1.6  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.5.2.1  2006/01/10 18:54:19  geconomos
// first crack at reducing flicker while resizing
//
// Revision 1.5  2005/10/04 02:10:32  geconomos
// reworked drawing of background image
//
// Revision 1.4  2005/09/13 17:27:19  jmeade
// ColonReport namespace.
//
// Revision 1.3  2005/08/16 22:56:12  jmeade
// namespaces for WinUtils.
//
// Revision 1.2  2005/08/05 14:14:10  geconomos
// updated #import for location of reporting library
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3.10.1.12.1  2005/06/17 13:15:10  geconomos
// using new BackgroundBrush class
//
// Revision 3.3.10.1  2003/04/09 21:50:46  jmeade
// Coding standards.
//
// Revision 3.3  2002/04/18 18:32:35  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.2  2002/03/15 03:27:47  jmeade
// Project configuration changes: Release ==> Debug-Release, Final Release ==> Release.
//
// Revision 3.1  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.0.4.1  2002/02/26 22:37:39  binli
// override OnOK() to avoid unnecessary pop-down of dialog view.
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
// 
//    Rev 2.0   Sep 16 2001 23:41:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:52   ingmar
// Initial revision.
// Revision 1.15  2001/09/10 08:36:57  soeren
// new dirs
//
// Revision 1.14  2001/05/10 21:04:40  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.13  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.12  2001/02/07 16:25:04  geconomos
// Added revisions to report file
//
// Revision 1.11  2001/02/05 16:24:17  jeff
// DisplayReportError(): static and public, so that it's accessible by non-derived (but still
// report-related) classes; corrected parameter to string format function
//
// Revision 1.10  2001/02/02 08:06:33  jeff
// Moving strings to resource table
//
// Revision 1.9  2001/02/01 17:07:03  geconomos
// Added comment headers and footers
//
// Revision 1.8  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportWndBase.h,v 1.9.2.1 2007/08/22 15:32:30 jmeade Exp $
// $Id: ReportWndBase.h,v 1.9.2.1 2007/08/22 15:32:30 jmeade Exp $
