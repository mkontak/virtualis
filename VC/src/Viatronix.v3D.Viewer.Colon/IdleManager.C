// $Id: IdleManager.C,v 1.4.2.4 2008/01/16 23:13:18 dongqing Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


#include "StdAfx.h"
#include "IdleManager.h"
#include "ReaderGlobal.h"
#include "Viatron.h"
#include "Timer.h"
#include "ViatronSplash.h"
#include "User.h"
#include "Crypt.h"
#include "AppSettings.h"
#include "ApplicationSettings.h"

#define FILE_REVISION "$Revision: 1.4.2.4 $"

/**
 * default constructor
 */
IdleManager::IdleManager() :
  m_iIdleInterval( 1800.0 )   // Set to 30 minutes since the elapsed timer returns seconds
{

  ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL ));

  float8 fIdelIntervale(0.0);   
  state.TryGet<float8>( APP_SETTING_IDLE_INTERVAL, m_iIdleInterval );
 

} // constructor



/**
 * Locks the application after a specified amount of time.
 *
 * MKK: Modified to catch security exception returned from the Security Provider. These exceptions are raised in the database when 
 *      we attempt to authenticate the user during a idle timeout event. I left the catch of the AccountExpiredException and 
 *      LicneseExpiredException just in case we want to close out the application and save the session as discussed. We implement
 *      the austerity solution where we authenticate, but ignore these exceptions when performing the authentication in the Database.
 *      The exception should never be thrown unless we change the implementation usage in the Security Provider.
 */
void IdleManager::Monitor()
{
  if( m_iIdleInterval > 0 && m_timer.ElapsedTime() >= m_iIdleInterval )
  {
    //LogDbg("App timed out: " + ToAscii(m_iIdleInterval) + " " + ToAscii(m_timer.ElapsedTime()), "IdleManager", "Monitor");
    AfxGetMainWnd()->ShowWindow(SW_HIDE);  

    // attempt to log user in when OK button is clicked
    ColonViews::Dialogs::ViatronSplash::FunctionPtr okPtr = []( ColonViews::Dialogs::ViatronSplash & dlg ) -> bool
    {

      dlg.SetUsername("");

      CString loginFailed;
      loginFailed.LoadString(IDS_LOGIN_FAILED);

      CString closeViewer;
      closeViewer.LoadString(IDS_CLOSE_VIEWER);

 
      try
      {
  
        // =========================================================================================
        // Re-authenticate the user using the newly entered password from the ViatronSplash screen.
        // =========================================================================================
        ViatronApp::GetViatronApp()->ReAuthenticate( dlg.GetUsername(), dlg.GetPassword() );

        dlg.SetCloseViewer(false);

        return true;
      }
      // ===================================================================================================================
      // Catch the Account Expired Exception so that an error message can be displayed asking the user to close the viewer
      // ===================================================================================================================
      catch ( ex::AccountExpiredException & ex )
      {
        std::stringstream msg;
        msg <<  ex.GetLogRecord().GetTextMessage() << ", " << (LPCTSTR)closeViewer;

        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);
        dlg.SetCloseViewer(true);
        
        return true;
 
      }
      // ===================================================================================================================
      // Catch the License Expired Exception so that an error message can be displayed asking the user to close the viewer
      // ===================================================================================================================
      catch ( ex::LicenseExpiredException & ex )
      {
        std::stringstream msg;
        msg <<  ex.GetLogRecord().GetTextMessage() << ", "  << (LPCTSTR)closeViewer;

  
        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);
        dlg.SetCloseViewer(true);
     
     
        return true;
 
      }
      // ===============================
      // Catch any security exceptions
      // ===============================
      catch ( ex::SecurityException & ex )
      {
 
        std::stringstream msg;
        msg << (LPCTSTR)loginFailed << " : " << ex.GetLogRecord().GetTextMessage();

        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);
        dlg.SetUsername( ( User::GetCurrentUser().GetLogin() ));
        dlg.Reset();
       
        return false;
      }
      // ===============================
      // Catch any remaining exceptions
      // ===============================
      catch ( ex::VException & ex )
      {
 
        std::stringstream msg;
        msg <<  ex.GetLogRecord().GetTextMessage() << ", "  << (LPCTSTR)closeViewer;

  
        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);
        dlg.SetCloseViewer(true);
     
     
        return true;
      }
      catch ( ... )
      {
        std::stringstream msg;
        msg <<  "Unknown error received trying to login, " << (LPCTSTR)closeViewer;

  
        AfxMessageBox( msg.str().c_str(), MB_OK | MB_ICONERROR);
        dlg.SetCloseViewer(true);
     
     
        return true;

      }

    };

    // prompt for confirmation before closing
    ColonViews::Dialogs::ViatronSplash::FunctionPtr cancelPtr = []( ColonViews::Dialogs::ViatronSplash & dlg ) -> bool
    {
      return ( AfxMessageBox( "Are you sure you want to exit?", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES );
    };

    ColonViews::Dialogs::ViatronSplash dlg( true, okPtr, cancelPtr );
    dlg.SetUsername( ( User::GetCurrentUser().GetLogin() ));
    if( dlg.DoModal() == IDOK )
    {

      if ( dlg.GetCloseViewer() == false )
      {
        AfxGetMainWnd()->ShowWindow(SW_SHOW);
        m_timer.Reset();
      }
      else
        AfxGetMainWnd()->PostMessage( WM_CLOSE );

    }
    else
      AfxGetMainWnd()->PostMessage( WM_CLOSE );
  }
} // Monitor()



/**
 * Processes the window message
 *
 * @param     pMsg            windows message
 */
void IdleManager::ProcessMessage( MSG * pMsg )
{
  const bool bIsFlying = ReaderLib::rdrGlobal.m_pCurrDataset->m_submarine.IsFlying();

  const bool bIsUserInteracting = ( pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST ) ||
                                  ( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST );
  if( bIsFlying || bIsUserInteracting )
    Reset();
} // ProcessMessage( pMsg )


/**
 * Resets the timer
 */
void IdleManager::Reset()
{
  //LogDbg("Reset: " + ToAscii(m_iIdleInterval) + " " + ToAscii(m_timer.ElapsedTime()), "IdleManager", "Monitor");
  m_timer.Reset();
} // Reset()


// $Log: IdleManager.C,v $
// Revision 1.4.2.4  2008/01/16 23:13:18  dongqing
// comments out debug info output
//
// Revision 1.4.2.3  2007/10/30 16:49:13  jmeade
// Issue 5848: user login should not be encrypted.
//
// Revision 1.4.2.2  2007/10/22 23:35:12  jmeade
// debug text
//
// Revision 1.4.2.1  2007/04/03 20:29:53  gdavidson
// Issue #5558: Set the username before displaying login dialog
//
// Revision 1.4  2007/03/13 18:36:38  jmeade
// code standards.
//
// Revision 1.3  2007/03/07 20:47:44  jmeade
// code standards.
//
// Revision 1.2  2006/12/21 16:44:57  gdavidson
// Lock console before displaying dialog
//
// Revision 1.1  2006/12/19 15:35:05  gdavidson
// Issue #5019: Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/IdleManager.C,v 1.4.2.4 2008/01/16 23:13:18 dongqing Exp $
// $Id: IdleManager.C,v 1.4.2.4 2008/01/16 23:13:18 dongqing Exp $
