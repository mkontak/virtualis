// $Id: ReportPreview.C,v 1.6 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: HTML view to view generated PDF file(requires AcrobatReader)
// Author: George Economos


#include "stdafx.h"
#include "viatron.h"
#include "resource.h"
#include "ReportPreview.h"
#include "VxReportCreator.h"
#include "VxVCReport.h"
#include "File.h"
#include "WinUtils.h"
#include "FileExtensions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ColonReport;
using namespace WinUtils;

#define FILE_REVISION   "$Revision: 1.6 $"

IMPLEMENT_DYNCREATE(ReportPreview, CHtmlView)

BEGIN_MESSAGE_MAP(ReportPreview, CHtmlView)
ON_WM_DESTROY()
END_MESSAGE_MAP()

/**
 * constructor
 */
ReportPreview::ReportPreview()
{
  // empty
} // constructor


/**
 * Sets parameters for window creation
 *
 * @param cs   Create parameters (in/out)
 * @return     Whether to continue with window create
 */
BOOL ReportPreview::PreCreateWindow(CREATESTRUCT& cs)
{
  cs.style |= WS_CLIPCHILDREN;
  
  return CHtmlView::PreCreateWindow(cs);
} // PreCreateWindow()


/**
 * first update routine
 */
void ReportPreview::OnInitialUpdate()
{
  CHtmlView::OnInitialUpdate();

  CWaitCursor cursor;

  std::string sFileName;

  try
  { 
    m_sTempFileName = vx::File::CreateTempFile(std::string(vx::ReportExt).substr(1,3),
      std::string(vx::AdobePDFExt).substr(1,3)).ToString().c_str();
  }
  catch (ex::IOException &)
  {
    LogErr("Unable to create temporary file for report preview.", "ReportPreview", "OnInitialUpdate");
    const CString sErr = LoadResourceString(IDS_ERROR_CREATE_REPORT_PREVIEW) + "\n\n" + LoadResourceString(IDS_CONTACT_SERVICE);
    AfxMessageBox(sErr, MB_ICONSTOP|MB_OK);
    return;
  }

  ConsoleLib::VxReportCreator creator(VxVCReport::m_spRevision);
  if(creator.Create(m_sTempFileName))
    Navigate2(m_sTempFileName, NULL, NULL);  
} // OnInitialUpdate()


/**
 * WM_DESTROY handler
 */
void ReportPreview::OnDestroy()
{
  CHtmlView::OnDestroy();

  DeleteFile(m_sTempFileName);
} // OnDestroy()


// Revision History:
// $Log: ReportPreview.C,v $
// Revision 1.6  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.5  2005/11/07 20:38:18  geconomos
// modified headers
//
// Revision 1.4  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.3  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.4  2002/07/18 20:25:39  jmeade
// Moved report functionality to ConsoleLib.
//
// Revision 3.3  2002/04/18 18:32:11  jmeade
// Reorganized includes in GLWinUtils lib.
//
// Revision 3.2  2001/11/09 04:18:22  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/11/05 22:09:20  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:48   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:40   ingmar
// Initial revision.
// Revision 1.9  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.8  2001/04/12 19:21:10  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.7  2001/04/12 16:49:50  jmeade
// exceptions in namespace vx
//
// Revision 1.6  2001/02/01 17:07:14  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportPreview.C,v 1.6 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportPreview.C,v 1.6 2007/03/09 23:38:54 jmeade Exp $
