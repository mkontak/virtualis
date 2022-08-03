// $Id: ReportViewFrame.h,v 1.5 2007/03/09 15:59:01 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Interface for View frame for report panes and report preview
// Created: 2/1/2001
// Owner: George Economos

#if !defined(AFX_REPORTVIEWFRAME_H__83450EA4_A0AA_11D4_99F5_00D0B7AC3819__INCLUDED_)
#define AFX_REPORTVIEWFRAME_H__83450EA4_A0AA_11D4_99F5_00D0B7AC3819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildViewFrame.h"
#include "reportpreview.h"

namespace ColonViews
{

  namespace Layouts
  {
    // report layout
    class ReportLayoutFrame : public ChildLayoutFrame
    {
    public:
      /// constructor
      ReportLayoutFrame();
      /// destructor
      virtual ~ReportLayoutFrame();

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ReportLayoutFrame)
      //}}AFX_VIRTUAL

    protected:
      //{{AFX_MSG(ReportLayoutFrame)
      //}}AFX_MSG
     
      DECLARE_DYNCREATE(ReportLayoutFrame)
    }; // class ReportLayoutFrame

    // report preview layout
    class ReportPreviewLayoutFrame : public ChildLayoutFrame
    {
    public:
      /// constructor
      ReportPreviewLayoutFrame();
      /// destructor
      virtual ~ReportPreviewLayoutFrame();
      /// gets the active GLWnd
      inline virtual GLWnd *GetActiveLayout() const;
      /// command message handler
      virtual BOOL OnCmdMsg(UINT uID, int iCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
      /// creation routine
      virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );
      
    protected:
      /// message handler
      virtual LRESULT WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
      
      DECLARE_DYNCREATE(ReportPreviewLayoutFrame)
      /// window receives focus
      afx_msg void OnSetFocus(CWnd* pOldWnd);
      /// create handler
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      /// size-change handler
      afx_msg void OnSize(UINT uType, int cx, int cy);
      /// opens a report preview
      afx_msg void OnOpenPreview();
  
       DECLARE_MESSAGE_MAP();
    }; // class ReportPreviewLayoutFrame

  } // namespace Layouts

} // namespace ColonViews

#endif

// $Log: ReportViewFrame.h,v $
// Revision 1.5  2007/03/09 15:59:01  jmeade
// code standards.
//
// Revision 1.4  2007/03/01 01:18:56  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.3  2005/10/25 16:46:49  jmeade
// ParentViews ==> Layouts
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.1.4.1  2003/02/22 11:59:03  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.1  2002/07/26 15:04:27  jmeade
// Fixes for Visual Studio class wizard database.
//
// Revision 3.0  2001/10/14 23:02:00  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:54   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:48   ingmar
// Initial revision.
// Revision 1.6  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.5  2001/02/01 17:07:14  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportViewFrame.h,v 1.5 2007/03/09 15:59:01 jmeade Exp $
// $Id: ReportViewFrame.h,v 1.5 2007/03/09 15:59:01 jmeade Exp $
