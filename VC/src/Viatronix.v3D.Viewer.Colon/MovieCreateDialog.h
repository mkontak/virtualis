// $Id: MovieCreateDialog.h,v 1.3 2007/03/12 20:47:11 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxMovie.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#pragma once

#include "resource.h"
//#include "FileExtensions.h"
#include "ViatronDialog.h"
//#include "SkinButton.h"
#include "ViatronChildView.h"
#include "MovieFormat.h"
//#include "MovieFrame.h"

OPEN_DIALOGS_NS

class MovieCreateDialog : public ColonViews::Dialogs::ViatronDialog
{
public:
  /// Standard constructor
  MovieCreateDialog(const Movies::MovieFormat& fmt = Movies::MovieFormat(), CWnd* pParent = NULL);
  /// Constructor with preview image
  MovieCreateDialog(ColonViews::ChildViews::ViatronChildView& childView, const Movies::MovieFormat& fmt = Movies::MovieFormat(), CWnd* pParent = NULL);

  // Dialog Data
  //{{AFX_DATA(MovieCreateDialog)
  /// dialog ID
  enum { IDD = IDD_MOVIE_CREATION };
  /// 1-frame preview of movie
  CStatic m_staticPreviewImage;
  /// cancel button
  WinUtils::SkinButton m_btnCancel;
  /// save movie button
  WinUtils::SkinButton m_btnSave;
  /// movie format
  int   m_iFormat;
  /// whether to include the overview
  BOOL  m_bIncludeOverview;
  /// title of movie file
  CString m_sFileTitle;
  //}}AFX_DATA

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(MovieCreateDialog)
  protected:
    /// DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
  //}}AFX_VIRTUAL

protected:
  // Generated message map functions
  //{{AFX_MSG(MovieCreateDialog)
  /// ok pressed
  virtual void OnOK();
  /// initialization
  virtual BOOL OnInitDialog();
  /// no compression
  afx_msg void OnMCDFormatCompressNone();
  /// low compression
  afx_msg void OnMCDFormatCompressLow();
  /// high compression
  afx_msg void OnMCDFormatCompressHigh();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
  /// Called when a format/compression radio group option has changed      
  void OnMCDFormatCompressOptionChanged(const uint4 uOptionResID);

private:
  /// Returns the appropriate file ext for current value of m_iFormat (format selection)
  const vx::FileExtension& GetFileExtension() const;

public:
  /// In/out format for the movie
  Movies::MovieFormat m_format;

private:
  /// Preview (one sample movie frame) of the child view
  WinUtils::BitmapDC m_childBDC;
  /// Preview (one sample movie frame) of overview
  WinUtils::BitmapDC m_overviewBDC;

}; // MovieCreatDialog

CLOSE_DIALOGS_NS


// $Log: MovieCreateDialog.h,v $
// Revision 1.3  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.2  2007/03/08 22:21:56  romy
// code review - header files adjusted
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// Revision 1.3  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.4.2.4  2005/02/15 20:04:57  jmeade
// merge from vc1.4 branch.
//
// Revision 1.4.2.3.2.1  2005/01/04 19:22:53  jmeade
// Movie formatting defaults to no overview.
//
// Revision 1.4.2.3  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.4.2.2  2003/05/23 04:31:19  jmeade
// Issue 2824: Read-only field to display movie filename b/f saving.
//
// Revision 1.4.2.1  2003/02/27 23:13:11  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 1.4  2003/01/31 16:32:50  jmeade
// Save/restore users' movie format options between sessions.
//
// Revision 1.3  2003/01/03 22:44:51  jmeade
// Derive from ViatronDialog for aesthetics; Hooks for preview images.
//
// Revision 1.2  2002/12/20 23:20:09  jmeade
// Tooltips and help text for movie formats.
//
// Revision 1.1  2002/12/20 21:49:05  jmeade
// Provide multiple movie format options.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieCreateDialog.h,v 1.3 2007/03/12 20:47:11 jmeade Exp $
// $Id: MovieCreateDialog.h,v 1.3 2007/03/12 20:47:11 jmeade Exp $
