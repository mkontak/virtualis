// $Id: MainAppFrameBkgndProc.C,v 1.6.2.1 2009/04/03 14:21:57 mkontak Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MainAppFrameBkgndProc.C
// Description: Functions related to running background (thread) processes for 
//        simple but time-consuming UI functions
// Created:
// Author: Jeff Meade  jmeade@viatronix.net
//
// Complete History at bottom of file.
//


#include <stdafx.h>
#include "MainAppFrame.h"
#include "V1KMessages.h"
#include "Viatron.h"
#include "vcUtils.h"
#include "ReportViewWnd.h"
#include <sstream>

#define FILE_REVISION "$Revision: 1.6.2.1 $"

using namespace ColonViews::Layouts;
using namespace ColonViews::FramesManager;
using namespace ColonViews::Dialogs;
using namespace ReaderLib;

/**
 * Message handler, called when a RunBackgroundProcess()-called background thread starts or finishes.
 *
 * @param iStartParams   HideParamsEnum, identifies what user interface controls, if any, to hide while process is running.
 * @param processID      Process identifier.
 * @return               TRUE.
 */
LRESULT MainAppFrame::OnBackgroundProcessStartFinish(WPARAM iStartParams, LPARAM processID)
{
  const bool bStart = LOWORD(iStartParams) == TRUE;
  const HideParamsEnum eHide = HideParamsEnum(HIWORD(iStartParams));
  
  if (bStart)
  {
    if (eHide & eBKHIDE_VIEWWIN)
    {
      if (m_pCurrentViewFrame)  m_pCurrentViewFrame->EnableWindow(FALSE);
      m_hideParamsRefCtMap[eBKHIDE_VIEWWIN]++;
    }
    
    if (eHide & eBKHIDE_MENU)
    {
      EnableMenuBar(false);
      m_hideParamsRefCtMap[eBKHIDE_MENU]++;
    }
    
    if (eHide & eBKHIDE_TABS)
    {
      // Disable all tabs
      m_viewSelBar.EnableItems(ViewSelectDlgBar::EnableViewsBitSet(std::string("00000")));

      m_navigationBar.EnableWindow(FALSE);

      m_hideParamsRefCtMap[eBKHIDE_TABS]++;

    } // END ... If the we are to hide all tabs
    
    m_bkgdProcIdMap[processID] = eHide;
  }
  else
  {
    BackgroundProcIdMap::iterator idIter = m_bkgdProcIdMap.find(processID);
    HideParamsEnum eProcHide = eBKHIDE_NONE;
    if (idIter != m_bkgdProcIdMap.end())
    {
      eProcHide = m_bkgdProcIdMap[processID];
      m_bkgdProcIdMap.erase(idIter);
    }
    
    if ((eProcHide & eBKHIDE_VIEWWIN) && (--m_hideParamsRefCtMap[eBKHIDE_VIEWWIN] <= 0))
    {
      if (m_pCurrentViewFrame)  m_pCurrentViewFrame->EnableWindow(TRUE);
      m_hideParamsRefCtMap[eBKHIDE_VIEWWIN] = 0;
    }
    
    if ((eProcHide & eBKHIDE_MENU) && (--m_hideParamsRefCtMap[eBKHIDE_MENU] <= 0))
    {
      EnableMenuBar(true);
      m_hideParamsRefCtMap[eBKHIDE_MENU] = 0;
    }
    
    // show tabs
    if ((eProcHide & eBKHIDE_TABS) && (--m_hideParamsRefCtMap[eBKHIDE_TABS] <= 0))
    {

      // Set the Console tabs enable flag based on the presence of the -console flag on the command line
      std::string sConsoleTabEnabled(ViatronApp::GetViatronApp()->GetCommandLine().GetLaunchedFromConsole() ? "1" : "0" );

      std::stringstream enableBits;

      enableBits << (( ReportLayout::CanOpenReport() ) ? "1" : "0") << (( ReportLayout::CanOpenReport() ) ? "1" : "0") << "11";

      // Re-enable all appropriate tab items
      m_viewSelBar.EnableItems( ViewSelectDlgBar::EnableViewsBitSet(std::string((rdrGlobal.m_pCurrDataset->IsLoadingComplete()) ? enableBits.str() : "0000") + sConsoleTabEnabled ));


      // Show the navigation bar
      m_navigationBar.EnableWindow(TRUE);
      m_hideParamsRefCtMap[eBKHIDE_TABS] = 0;
    }
  }
  
  return TRUE;
} // OnBackgroundProcessStartFinish(



#undef FILE_REVISION

// Revision History:
// $Log: MainAppFrameBkgndProc.C,v $
// Revision 1.6.2.1  2009/04/03 14:21:57  mkontak
// Fix so that the Console tab is not enabled when launched stand-alone or from a
// launcher
//
// Revision 1.6  2007/03/13 03:10:51  jmeade
// code standards.
//
// Revision 1.5  2005/11/21 23:16:18  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.4.2.1  2005/11/21 19:37:55  jmeade
// enable console tab for new functionality linking with external console.
//
// Revision 1.4  2005/10/25 15:47:37  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/10 23:48:54  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.7.10.1  2004/03/22 21:46:21  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.7  2002/04/12 02:12:22  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.6  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.5.2.1  2002/02/07 22:20:27  jmeade
// Background processes get their own class and file.
//
// Revision 3.5  2001/12/31 19:15:17  jmeade
// Disable (will later hide) study browser tab selection.
//
// Revision 3.4  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.3  2001/12/07 01:28:49  jmeade
// Guard against accessing a potentially-NULL pointer.
//
// Revision 3.2  2001/11/09 04:41:55  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0.2.2  2001/11/14 18:16:48  jmeade
// Log error when thread for background process cannot be created;  comments.
//
// Revision 3.0.2.1  2001/10/24 20:36:51  jmeade
// Coding standards
//
// Revision 3.0  2001/10/14 23:01:58  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:30   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:18   ingmar
// Initial revision.
// Revision 1.15  2001/08/30 15:27:22  jmeade
// Unsupported data type?!?
//
// Revision 1.14  2001/08/20 20:08:09  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.13  2001/07/25 22:51:53  binli
// ID 89: new exception handling.
//
// Revision 1.12  2001/07/24 21:48:55  binli
// ID 89: exception handling in v1k GUI
//
// Revision 1.11  2001/07/24 15:04:23  binli
// task ID: 89: V1K GUI exception handling.
//
// Revision 1.10  2001/06/14 00:54:57  jmeade
// initialize COM for background threads
//
// Revision 1.9  2001/06/08 00:46:19  jmeade
// coding conventions
//
// Revision 1.8  2001/05/23 17:53:32  jmeade
// enable/disable view selection tab items (instead of hiding entire tab control)
//
// Revision 1.7  2001/05/19 00:50:24  jmeade
// Nest BkgdProcStruct into MainAppFrame as a private class
//
// Revision 1.6  2001/05/17 06:19:28  jmeade
// BackgroundProcess(): do not exit prior to cleanup!; typedef name
//
// Revision 1.5  2001/05/07 18:47:12  jmeade
// clean-up after background thread finishes (either successfully or thru
// a thrown exception
//
// Revision 1.4  2001/05/01 19:07:19  jmeade
// catch all unhandled exceptions from background process function
// (preventing Dr. Watson type errors and other strange anomalies)
//
// Revision 1.3  2001/04/13 02:34:11  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MainAppFrameBkgndProc.C,v 1.6.2.1 2009/04/03 14:21:57 mkontak Exp $
// $Id: MainAppFrameBkgndProc.C,v 1.6.2.1 2009/04/03 14:21:57 mkontak Exp $
