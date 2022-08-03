// $Id: MovieCreateDialog.C,v 1.5 2007/03/12 20:47:11 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxMovie.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
//#include "WinUtils.h"
//#include "OverView.h"
//#include "GLWnd.h"
#include "DrawOverView.h"

#include "MovieCreateDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Movies;
using namespace ColonViews::Dialogs;
using namespace WinUtils;

/////////////////////////////////////////////////////////////////////////////
// MovieCreateDialog dialog

/**
 * Constructor 
 *
 * @param fmt       Movie format options.
 * @param pParent   Parent window.
 */
MovieCreateDialog::MovieCreateDialog(const MovieFormat& fmt /*= MovieFormat()*/, CWnd* pParent /*=NULL*/)
  : ViatronDialog(MovieCreateDialog::IDD, pParent), m_format(fmt)
{
  //{{AFX_DATA_INIT(MovieCreateDialog)
  m_iFormat = 0;
  m_bIncludeOverview = FALSE;
  m_sFileTitle = _T("");
  //}}AFX_DATA_INIT
} // constructor


/**
 * Constructor 
 *
 * @param childView   Child view for preview image.
 * @param fmt         Movie format options.
 * @param pParent     Parent window.
 */
MovieCreateDialog::MovieCreateDialog(ViatronChildView& childView, const MovieFormat& fmt /*= MovieFormat()*/, CWnd* pParent /*= NULL*/)
  : ViatronDialog(MovieCreateDialog::IDD, pParent), m_format(fmt), m_iFormat(0), m_bIncludeOverview(FALSE)
{
  CClientDC dc(this);
  m_childBDC.CreateBmpDC(&dc);
  m_overviewBDC.CreateBmpDC(&dc);

  const Point2D<uint4> imageSize(256,256);
  Point2D<int4> bufferSz;

  LPBYTE pImageBuffer;

  int4 iWidthAlignedDWORD( 0 );
  pImageBuffer = LPBYTE(childView.GetUpdateBuffer(bufferSz.m_x, bufferSz.m_y, iWidthAlignedDWORD,
    GL_BGR_EXT, GL_UNSIGNED_BYTE, !fmt.m_bIncludeMarkings));
  ResizeBmp(&dc, pImageBuffer, bufferSz, imageSize, m_childBDC);

  DrawOverView ovDrawImage(this, childView.GetDatasetRef(), childView.GetViewRect());
  pImageBuffer = LPBYTE(ovDrawImage.GetUpdateBuffer(bufferSz.m_x, bufferSz.m_y, iWidthAlignedDWORD, GL_BGR_EXT, GL_UNSIGNED_BYTE));
  ResizeBmp(&dc, pImageBuffer, bufferSz, imageSize, m_overviewBDC);
} // constructor


/**
 * Dialog data variable initialization.
 *
 * @param pDX   Variable data.
 */
void MovieCreateDialog::DoDataExchange(CDataExchange* pDX)
{
  ViatronDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(MovieCreateDialog)
  DDX_Control(pDX, IDC_MCD_PREVIEW_FRAME, m_staticPreviewImage);
  DDX_Control(pDX, IDCANCEL, m_btnCancel);
  DDX_Control(pDX, IDOK, m_btnSave);
  DDX_Radio(pDX, IDC_MCD_FORMAT_COMPRESSNONE, m_iFormat);
  DDX_Check(pDX, IDC_MCD_INCLUDE_OVERVIEW_CHECK, m_bIncludeOverview);
  DDX_Text(pDX, IDC_MCD_FILETITLE_EDIT, m_sFileTitle);
  //}}AFX_DATA_MAP

  // Additional check on entered filename
#ifdef WHEN_FILENAME_FIELD_IS_NO_LONGER_READONLY
  if (pDX->m_bSaveAndValidate)
  {
    vx::File vFile(LPCTSTR(m_sFileTitle) + GetFileExtension());
    if (vFile.Exists())
    {
      AfxMessageBox("The specified file already exists.  Please choose another filename.");
      AfxThrowUserException();
    }
    if (!vFile.CreateNewFile())
    {
      AfxMessageBox("An error occurred while creating the specified file.  Please re-check the filename, or choose another filename.");
      AfxThrowUserException();
    }
    vFile.Delete();
  }
#endif
} // DoDataExchange()


/**
 * message map
 **/
BEGIN_MESSAGE_MAP(MovieCreateDialog, ViatronDialog)
  //{{AFX_MSG_MAP(MovieCreateDialog)
  ON_BN_CLICKED(IDC_MCD_FORMAT_COMPRESSNONE, OnMCDFormatCompressNone)
  ON_BN_CLICKED(IDC_MCD_FORMAT_COMPRESSLOW, OnMCDFormatCompressLow)
  ON_BN_CLICKED(IDC_MCD_FORMAT_COMPRESSHIGH, OnMCDFormatCompressHigh)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MovieCreateDialog message handlers


/**
 * WM_INITDIALOG handler.
 *
 * @return    FALSE if focus is set to a control, TRUE otherwise.
 */
BOOL MovieCreateDialog::OnInitDialog() 
{
  ViatronDialog::OnInitDialog();

  // Initialization
  EnableToolTips(TRUE);
  SetButtonIcon(m_btnSave, IDB_BUTTON_LARGE);

  m_iFormat = (m_format.m_sFileTypeExt == vx::AVIExt) ? 0 : (!m_format.m_bIncludeMarkings ? 1 : 2);
  m_bIncludeOverview = m_format.m_bIncludeOverview;
  m_sFileTitle = m_format.m_sFileTitle.c_str();

  OnMCDFormatCompressOptionChanged(m_iFormat==0 ? IDC_MCD_FORMAT_COMPRESSNONE :
    (m_iFormat==1 ? IDC_MCD_FORMAT_COMPRESSLOW : IDC_MCD_FORMAT_COMPRESSHIGH));

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog()


/**
 * Returns the appropriate file ext for current value of m_iFormat (format selection)
 *
 * @return    the appropriate file ext for current value of m_iFormat (format selection)
 */
const vx::FileExtension& MovieCreateDialog::GetFileExtension() const
{
  return (m_iFormat == 0) ? vx::AVIExt : vx::WindowsMediaVideoExt;
} // GetFileExtension()


/**
 * Called when Create movie button is clicked.
 */
void MovieCreateDialog::OnOK() 
{
  try
  {
    UpdateData(TRUE);
  }
  catch (...)
  {
    return;
  }

  m_format.m_sFileTypeExt = GetFileExtension();
  m_format.m_bIncludeMarkings = (m_iFormat == 2);
  m_format.m_bIncludeOverview = (m_bIncludeOverview == TRUE);
  m_format.m_sFileTitle = LPCTSTR(m_sFileTitle);

  EndDialog(IDOK);
} // OnOK()


/**
 * Called when the movie compression format option changes.
 *
 * @param uOptionResID               Resource ID of compression option radio (and accompanying string resource).
 */
void MovieCreateDialog::OnMCDFormatCompressOptionChanged(const uint4 uOptionResID)
{
  GetDlgItem(IDC_MCD_FORMAT_DESCRIPTION)->SetWindowText(LoadResourceString(uOptionResID));
} // OnMCDFormatCompressOptionChanged()


/**
 * Called when 'no compression' option selected.
 */
void MovieCreateDialog::OnMCDFormatCompressNone()
{
  OnMCDFormatCompressOptionChanged(IDC_MCD_FORMAT_COMPRESSNONE);
} // OnMCDFormatCompressNone()


/**
 * Called when 'low compression' option selected.
 */
void MovieCreateDialog::OnMCDFormatCompressLow()
{
  OnMCDFormatCompressOptionChanged(IDC_MCD_FORMAT_COMPRESSLOW);
} // OnMCDFormatCompressLow()


/**
 * Called when 'high compression' option selected.
 */
void MovieCreateDialog::OnMCDFormatCompressHigh()
{
  OnMCDFormatCompressOptionChanged(IDC_MCD_FORMAT_COMPRESSHIGH);
} // OnMCDFormatCompressHigh()


// Revision History:
// $Log: MovieCreateDialog.C,v $
// Revision 1.5  2007/03/12 20:47:11  jmeade
// code standards.
//
// Revision 1.4  2007/03/08 22:21:56  romy
// code review - header files adjusted
//
// Revision 1.3  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.2.2.1  2005/11/17 16:30:43  geconomos
// issue# 4540: report preview causing crash (reworked GetUpdateBuffer)
//
// Revision 1.2  2005/10/28 17:48:23  geconomos
// renamed files from GLViewWnd => GLWnd
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
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.4.4.2.4.1.2.1  2005/06/21 15:16:02  geconomos
// Broke out DrawOverView class into it's own .h and .C
//
// Revision 1.4.4.2.4.1  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.4.4.2.8.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 1.4.4.2  2003/05/23 04:31:19  jmeade
// Issue 2824: Read-only field to display movie filename b/f saving.
//
// Revision 1.4.4.1  2003/02/27 23:13:11  jmeade
// Put common tooltip text get method in base class for use by all derivatives.
//
// Revision 1.4  2003/01/03 22:56:32  jmeade
// Oops, compile error.
//
// Revision 1.3  2003/01/03 22:44:42  jmeade
// Derive from ViatronDialog for aesthetics; Hooks for preview images.
//
// Revision 1.2  2002/12/20 23:20:09  jmeade
// Tooltips and help text for movie formats.
//
// Revision 1.1  2002/12/20 21:49:05  jmeade
// Provide multiple movie format options.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieCreateDialog.C,v 1.5 2007/03/12 20:47:11 jmeade Exp $
// $Id: MovieCreateDialog.C,v 1.5 2007/03/12 20:47:11 jmeade Exp $
