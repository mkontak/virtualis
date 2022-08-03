// $Id: LoadStudy.C,v 1.19.2.1 2008/04/05 00:42:12 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LoadStudy.C  
// Owner: Jeffrey R. Meade  jeff@viatronix.net


#include "stdafx.h"
#include "Viatron.h"
#include "MainAppFrame.h"
#include "ReportViewWnd.h"
#include "StandardView.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"
#include "ViatronStudy.h"
#include "File.h"
#include "VxVCReport.h"
#include "FunctionProgress.h"
#include "Crypt.h"
#include "LoaderFactory.h"

#include <direct.h>

using namespace ColonViews::Layouts;
using namespace ReaderLib;
using namespace WinUtils;


/////////////////////////////////////////////////////////////////////////////
// Functions that handle Viatron Study Loading

#ifdef _DEBUG
CMemoryState GlobalMemoryState;
#endif

#define FILE_REVISION "$Revision: 1.19.2.1 $"


/**
 * Message: IDM_STUDY_CLOSE: Closes the current study
 */
void MainAppFrame::OnMenuStudyClose()
{
  Timer closeTimer; closeTimer.Reset();

  // release the license
  ViatronApp::GetViatronApp()->ReleaseLicense();
	
	bool bReportComplete( false );
	bReportComplete = VxVCReport::IsReportMarkedCompleted();

  // Close and save report file
	VxVCReport::WritePDFandSnapshots(true);
  VxVCReport::CloseReport(true, false);
	VxVCReport::RemoveTemporaryReportFiles();
  LogFyi("Closing Report took " + closeTimer.AsString() + " seconds to complete.","MainAppFrame","OnMenuStudyClose"); closeTimer.Reset();

  rdrGlobal.FreeAllLargeVolumesButPaint();
  LogFyi("Freeing ECV and DFB took " + closeTimer.AsString() + " seconds to complete.","MainAppFrame","OnMenuStudyClose"); closeTimer.Reset();

	if( !bReportComplete )
	{
    if( !rdrGlobal.SaveData() )
	  {
	    AfxMessageBox("An error occurred while saving the session.");
	  } // if
    LogFyi("Saving Session Info took " + closeTimer.AsString() + " seconds to complete.","MainAppFrame","OnMenuStudyClose"); closeTimer.Reset();
	}

  rdrGlobal.Reset();
  LogFyi("ReaderGlobal Reset took " + closeTimer.AsString() + " seconds to complete.","MainAppFrame","OnMenuStudyClose"); closeTimer.Reset();

	/// we are not sure if this causes trouble when one viewer is still closing while another
	/// viewer is launching. That might cause resource conflict. From safeside, we comments out for now.
	/// Dongqing 03-12-2013
  //// Remove the update timer and remove the lock on the open study
  //m_timerUpdateLocationLock.KillTimer();
  
#if 0 // defined(_DEBUG)
  // Perform a memory state check and print results (to Visual Studio or other console window)
  CMemoryState currMemState;
  currMemState.Checkpoint();

  CMemoryState diffState;
  diffState.Difference(GlobalMemoryState, currMemState);

  diffState.DumpStatistics();

  GlobalMemoryState.DumpAllObjectsSince();
  GlobalMemoryState.Checkpoint();
#endif

  LogFyi("Remainder took " + closeTimer.AsString() + " seconds to complete.","MainAppFrame","OnMenuStudyClose"); closeTimer.Reset();
} // OnMenuStudyClose()


/////////////////////////////////////////////////////////////////////////////


/**
 * Background thread function, notifies main thread
 *  of a transition between steps during study load.
 *
 * @param pData              Handle to the main window to receive the notify messages.
 * @param ePrevStep          The completed (previous) step.
 * @param eNextStep          The next step to be performed
 * @param fPercentComplete   Percentage of study load completion.
 * @param eNewOrientation    Orientation currently loading.
 * @return                   TRUE.
 */
int4 MainAppFrame::StudyLoadNotify(void * pData, const StudyLoad::StepsEnum ePrevStep,
                    const StudyLoad::StepsEnum eNextStep, const float4 fPercentComplete,
                    const DatasetOrientations::TypeEnum eNewOrientation)
{
  if (!rdrGlobal.m_bAbortLoad)
  {
    if (eNextStep == StudyLoad::eLARGESIZE_QUERY)
    {
      ::SendMessage(HWND(pData), WM_APP_STUDY_LOAD_STEP_NOTIFY,
        MAKEWPARAM(ePrevStep, eNextStep), LPARAM(eNewOrientation));  
    }
    else
    {
      ::PostMessage(HWND(pData), WM_APP_STUDY_LOAD_STEP_NOTIFY,
        MAKEWPARAM(ePrevStep, eNextStep), LPARAM(eNewOrientation));
      ::PostMessage(HWND(pData), FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS,
        int4(fPercentComplete), 0);
    }
  }

  return 1;
} // StudyLoadNotify()

/////////////////////////////////////////////////////////////////////////////



/**
 * Checks the hazard files for the study
 *
 * @param sFilePrefix   File path prefix
 * @param eDatasets     Datasets to check hazards on
 * @return              False if user answered 'No' to a hazards prompt.
 */
bool MainAppFrame::CheckHazards( StudyLoadParams & params )
{
  LogDbg("checking hazards", "MainAppFrame", "CheckHazards");
 
  // retrieve the hazards
  std::list<Hazards::HazardEnum> supineHazards, proneHazards;
  
  try
  {
    if( params.GetFlags() & StudyLoadParams::PRIMARY_DATASET_VALID )
    {
      supineHazards = rdrGlobal.GetHazards( DatasetLoadParams::FormatPrefixPath( params.GetPrimaryDataset().GetPreprocessedDirectory(), params.GetPrimaryDataset().GetPrefix() ) );
    }

    if( params.GetFlags() & StudyLoadParams::SECONDARY_DATASET_VALID )
    {
      proneHazards = rdrGlobal.GetHazards( DatasetLoadParams::FormatPrefixPath( params.GetSecondaryDataset().GetPreprocessedDirectory(), params.GetSecondaryDataset().GetPrefix() ) );
    }
  }
  catch( ex::FileNotFoundException )
  {
  }
  

  // convert the hazards into the local language
  if (supineHazards.size() + proneHazards.size() > 0)
  {
    // concatenate all the hazard strings into one long prompt for the user
    CString sHazardPromptString, sTemp;
    std::list<Hazards::HazardEnum>::iterator it;
    
    // loop over all the supine hazards
    if (supineHazards.size() > 0)
    {
      sHazardPromptString += LoadResourceString(IDS_SUPINE_HAZARDS) + ":\n";
    }
    for (it = supineHazards.begin(); it != supineHazards.end(); it++)
    {
      sTemp.LoadString(IDS_HAZARD001 + int4(*it));
      sHazardPromptString += "  " + sTemp + "\n";
    } // loop over all supine hazards

    // add a blank line in between supine and prone
    if (supineHazards.size() > 0 && proneHazards.size() > 0)
    {
      sHazardPromptString += "\n";
    }

    // loop over all the prone hazards
    if (proneHazards.size() > 0)
    {
      sHazardPromptString += LoadResourceString(IDS_PRONE_HAZARDS) + ":\n";
    }
    for (it = proneHazards.begin(); it != proneHazards.end(); it++)
    {
      sTemp.LoadString(IDS_HAZARD001 + int4(*it));
      sHazardPromptString += "  " + sTemp + "\n";
    } // loop over all prone hazards

    // Ask the user to continue
    sTemp.LoadString(IDS_HAZARD_CONTINUE);
    sHazardPromptString += "\n" + sTemp;
    int4 iResult = AfxMessageBox(sHazardPromptString, MB_YESNO | MB_ICONQUESTION);
    if (iResult == IDNO)
    {
      return false;
    } // early return if the user wishes to quit after reading hazards

  } // if there are any hazards to display

  return true;
} // CheckHazards()


#define sEXCEPTION_NAME_SEP std::string("-_;_-")

/**
 * Message WM_APP_LOAD_VIATRON_STUDY_DONE: Handles message return back at end of background procedure for loading a study.
 *
 * @param bSuccess   Non-zero if load ended successfully, zero otherwise.
 * @param lParam     Pointer to std::string, detailing error information, if supplied, or NULL if no extra information supplied.
 *                           Format is: typeid(class).name(), followed by separator (sEXCEPTION_NAME_SEP), followed by error text.
 * @return           Value of bSuccess
 */
LRESULT MainAppFrame::OnLoadViatronStudyProcDone(WPARAM bSuccess, LPARAM lParam)
{
  if (bSuccess != 0)
  {
    // Success!

    // check for an incompatible number of registration features and warn
    if ( rdrGlobal.m_bDualDataLoaded &&
         rdrGlobal.m_prone .m_session.m_vUserFeatures.size() !=
         rdrGlobal.m_supine.m_session.m_vUserFeatures.size() )
    {
      rdrGlobal.m_supine.m_session.m_vUserFeatures.clear();
      rdrGlobal.m_prone.m_session.m_vUserFeatures.clear();
      AfxMessageBox( IDS_STUDYLOAD_MISMATCHED_REGISTRATION );
    }

  }
  else
  {
    // failed or aborted
    if (rdrGlobal.m_bLoadAborted == false)
    {
      CString sLoadError;
      std::string * pErrorString = (std::string *) lParam;

      if (pErrorString != NULL)
      {
        const std::string::size_type uFindColon(pErrorString->find(sEXCEPTION_NAME_SEP));
        const std::string::size_type uSep((uFindColon != std::string::npos) ? uFindColon : 0);
        const std::string sExceptionName(pErrorString->substr(0, uSep));

        if ((sExceptionName == typeid(ex::OutOfMemoryException).name()))
        {
          sLoadError = LoadResourceString(IDS_ERROR_INSUFFICIENT_RESOURCES_FOR_STUDY);
        }
        else
        {
          // Display exception text, minus class name, if separator found; otherwise show all text
          sLoadError.Format(IDS_STUDYLOAD_ERROR_FORMAT,
            pErrorString->substr((uSep>0)?(uSep+sEXCEPTION_NAME_SEP.length()):0).c_str());

					/// The folowing code is hacked for vc 4.1.2 release. We need to fix this for vc 4.2
					std::string ssss = pErrorString->substr((uSep>0)?(uSep+sEXCEPTION_NAME_SEP.length()):0);

					if( (ssss.find("launchable data") != std::string::npos) ||
						  (ssss.find("Study is being processed") != std::string::npos))
					{
						sLoadError = "The selected study/series has not completed processing. This may take several minutes to complete. Please try again.";
					}
					else if( (ssss.find("combination is not supported") != std::string::npos) ||
						       (ssss.find("Multiple studies") != std::string::npos) )
					{
						sLoadError = "Launching from two study uids is not supported. Please contact your local administrator for assistance.";
					}
					else if( ssss.find("Series not found") != std::string::npos )
					{
						sLoadError = "The selected study/series was not found. It may not have been received by the V3D System. Please send the study/series to the V3D System or contact the local administrator for assistance.";
					}
        }

        AfxMessageBox(sLoadError);
        delete pErrorString;
      }
      else
      {
        // We'll reserve this for cases where the user does not need another message.
        // One example is when 'No' is the answer at the Hazards prompt.
      } // if no error info
    } // if load failed but not aborted
    else
    {
      // Do nothing, just return silently.  Do not pop up a message box, just continue quietly.
      // There's no need to annoy the user with a message telling them they aborted.  They know it!
    } // if load aborted

    // TODO: Query a flag to determine whether or not a study was already opened,
    //        and if so, show the view windows
    PostMessage(WM_CLOSE);
  }

	/// This is for disable Option, Notations, and Report menu items in the Verifiy tab.
	EnableMenuBar( false );

  return (bSuccess == TRUE);
} // OnLoadViatronStudyProcDone()

/// parameters for thread
struct StudyProcThreadParams
{
  StudyLoadParamsEx m_params;
  HWND m_hNotify;
  std::function< void( StudyLoadParamsEx & )> m_reportFunction;
};


/**
 * opens a study
 *
 * @param params   study load params
 * @return         whether the study was opened successfully
 */
bool MainAppFrame::Open( StudyLoadParamsEx & params  )
{
  StudyProcThreadParams * pThreadParams = new StudyProcThreadParams();
  pThreadParams->m_params = params;
  pThreadParams->m_hNotify = GetSafeHwnd();
  pThreadParams->m_reportFunction = [&]( StudyLoadParamsEx & params )
  { 
    // set the report prefix so it can be loaded later
    m_sReportPrefix = ReaderLib::DatasetLoadParams::FormatPrefixPath( params.GetSessionDirectory(), 
      ( params.GetFlags() & ReaderLib::StudyLoadParams::PRIMARY_DATASET_VALID ) ? params.GetPrimaryDataset().GetPrefix() : params.GetSecondaryDataset().GetPrefix() );
  };

  /*/ Set the readonly flag for the study
  // TODO: I'm almost certain this needs implementation!!!
  rdrGlobal.m_bReadOnly = (params.m_eDbLocation == DBLOC_CD) || (vx::File(params.m_sFileName).GetDriveType() == DRIVE_CDROM);
  if (rdrGlobal.m_bReadOnly)
  {
    LogDbg("DBLOC_CD detected", "MainAppFrame", "OpenStudyFile");
    ReportColonWnd::SetDBLocation(DBLOC_CD);
  }  */

  BackgroundProcess::Run(LoadViatronStudyProc2, pThreadParams, "LoadViatronStudyProc", eBKHIDE_MENU, THREAD_PRIORITY_HIGHEST);

  StandardLayout::ResetGlobalMainWindow();

  // check the hazards and continue (or not)
  if( !CheckHazards( params ) )
  {
    // Log the message, but do not report it to the user
    ex::VException(LogRec("Study hazards, aborted by user", "MainAppFrame", "LoadViatronStudyProc"));
    // TODO: send abort load message here
    OnClose();
    return false;
  }

  return true;
} // Open( StudyLoadParam & params )


/**
 * Background process to load a study.
 *
 * @param pParam   Handle to app main window.
 * @return         TRUE/FALSE indicating success/failure
 */
UINT MainAppFrame::LoadViatronStudyProc2( LPVOID pParam )
{
  StudyProcThreadParams * pThreadParams = reinterpret_cast< StudyProcThreadParams * >( pParam );
  SCOPE_EXIT 
  { 
    delete pThreadParams; 
    m_loader = nullptr;
  };

  ViatronStudy::Reset();

#ifdef _DEBUG
  GlobalMemoryState.Checkpoint();
#endif

  try
  {
    LoaderFactory::LoaderParams params = { pThreadParams->m_params, StudyLoad::LoadCallback( StudyLoadNotify, pThreadParams->m_hNotify ) };
    m_loader = LoaderFactory::Create( params );
    m_loader->Load();

    // set report
    pThreadParams->m_reportFunction( m_loader->GetParams() );
  }
  catch( ex::OutOfMemoryException & memErr )
  {
    LogRec(memErr.GetLogRecord().GetTextMessage(), "MainAppFrame", "LoadViatronStudyProc");

    return ::PostMessage(pThreadParams->m_hNotify, WM_APP_LOAD_VIATRON_STUDY_DONE, FALSE, 
      (LPARAM) (new std::string(typeid(memErr).name() + sEXCEPTION_NAME_SEP + memErr.GetLogRecord().GetTextMessage())));
  }
  catch( ex::AbortLoadException & )
  {
    return ::PostMessage(pThreadParams->m_hNotify, WM_APP_LOAD_VIATRON_STUDY_DONE, FALSE, 0);
  }
  catch( ex::LoadFailedException & )
  {
    LogRec( "Error while loading studies", "MainAppFrame", "LoadViatronStudyProc" );

    return ::PostMessage(pThreadParams->m_hNotify, WM_APP_LOAD_VIATRON_STUDY_DONE, FALSE,
      (LPARAM) (new std::string(typeid(ex::VException).name() + sEXCEPTION_NAME_SEP + "Error while loading studies")));
  }
  catch( ex::VException & error )
  {
    LogRec(error.GetLogRecord().GetTextMessage(), "MainAppFrame", "LoadViatronStudyProc");

    return ::PostMessage( pThreadParams->m_hNotify, WM_APP_LOAD_VIATRON_STUDY_DONE, FALSE, 
      (LPARAM) (new std::string(typeid(error).name() + sEXCEPTION_NAME_SEP + error.GetLogRecord().GetTextMessage())));
  }

  return ::PostMessage( pThreadParams->m_hNotify, WM_APP_LOAD_VIATRON_STUDY_DONE, TRUE, 0 );
} // LoadViatronStudyProc2( pParam )
