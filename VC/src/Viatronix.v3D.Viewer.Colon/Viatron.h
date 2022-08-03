// $Id: Viatron.h,v 1.13 2007/03/08 17:09:48 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Jeff Meade  jmeade@viatronix.net


// pragmas
#pragma once



// includes
#include "StudyLoad.h"
#include "V3DAppMutex.h"
#include "ColonLicense.h"
#include "CommandLineInfoEx.h"
#include "IdleManager.h"
#include "StudyLoadParamsEx.h"


// class declaration
class ViatronApp : public CWinApp
{
// member functions
public:
  
  /// Constructor
  ViatronApp();

  /// hides the splash screen
  void HideSplashScreen();

  inline const std::string  GetContextId()
  { return m_licenseClient.GetContextId();  }

  /// Gets the transaction is associated with this launch instance
  std::string & GetTransactionId()
  { return m_sTransactionId; }

  /// Sets the transaction is associated with this launch instance
  void SetTransactionId(const std::string & sTransactionId)
  {   m_sTransactionId = sTransactionId;  }

  /// Gets the flag indicating if we have an license or not
  inline bool HasLicense()
  { return m_licenseClient.HasLicense();}
  
  /// gets the command line
  CommandLineInfoEx & GetCommandLine() { return m_commandline; }

  /// gets the idle manager
  IdleManager & GetIdleManager() { return m_idleManager; }

  /// gets a type-casted pointer to the current application
  static inline ViatronApp* GetViatronApp() { return (ViatronApp*) AfxGetApp(); }

  /// Returns the registry section for user settings for this application
  static std::string GetUserRegistrySection();

  /// 'new' exception handler (not 'new' like 'brand new', 'new' as in the memory allocation 'new')
  static int AFX_CDECL VtxNewHandler(size_t uSize);

  /// application mutex handle
  static bool CloseAppMutexHandle();


  /// Closes the session (Release the license)
  void CloseSession();

  /// Releases the applications license
  void ReleaseLicense();

  /// Renews the applications license
  bool RenewLicense();

  /// Re-authenticates the user
  void ReAuthenticate(const std::string & sLogin, const std::string & sPassword);


protected:

  /// Application class initialization
  virtual BOOL InitInstance();

  /// Called upon app exit
  virtual int ExitInstance();

  /// On idle application event handler (redraws views)
  virtual BOOL OnIdle( LONG lCount );

  /// MFC message map
  DECLARE_MESSAGE_MAP()

  /// Runs the about modal dialog
  void OnAppAbout();

  /// Launches help.
  void OnHelpContents();

  /// Updates the on app about menu.
  void OnUpdateAppAbout( CCmdUI * pCmdUI );

  /// registers the com server
  void RegisterServer();

  /// unregisters the com server
  void UnregisterServer();

private:

  /// generates a pdf from the specified vrx file
  static bool GeneratePdf( const std::string & sVrxFileName, const std::string & sPdfFileName );

  /// generates pdf and snapshots
  static bool GenerateOutputFiles( const std::string & sVrxFileName, const std::string & sOutputPath );

  /// send to pacs
  static bool TransferToPacs( const std::string & sVrxFileName, const std::string & uid, uint4 iSendOptions );

  /// licenses the application
  void LicenseApplication();

  /// login from commandline
  bool LoginFromCommandLine();

  /// login from commandline
  bool AuthenticateFromCommandLine();

	/// set display position based on the input datasets' body position
	void SetDisplayPosition( StudyLoadParamsEx & params);

	/// determine the body position from the volume header
	std::string GetBodyPosition(const std::string & sVolumeFilePath);

	/// Remove all temporary report vrx files
	void CleanseTempReportFile();


private:

  /// returns the path to the license file
  std::string GetLicenseFilePath() const;

private:

  /// gdi+ startup token
  ULONG_PTR m_pToken;

  /// single mutex for this application
  static VxConsole::SingleMutex * m_pAppSingleMutex;

  // command line arguments
  CommandLineInfoEx m_commandline;

  /// Transaction Id associated with this launch
  std::string m_sTransactionId;

  /// Instance of the license
  ColonLicense m_licenseClient;

  /// idle manager
  IdleManager m_idleManager;

}; // class Viatron


// $Log: Viatron.h,v $
// Revision 1.13  2007/03/08 17:09:48  jmeade
// code standards.
//
// Revision 1.12  2007/03/07 20:49:01  jmeade
// code standards.
//
// Revision 1.11  2006/12/19 15:28:10  gdavidson
// Issue #5019: Added IdleManager
//
// Revision 1.10  2006/06/12 19:00:45  gdavidson
// Changed License to ColonLicense
//
// Revision 1.9  2006/03/24 21:35:38  gdavidson
// Changed the class LicenseClient to License
//
// Revision 1.8  2006/02/07 21:29:39  gdavidson
// Code Review
//
// Revision 1.7  2005/12/20 20:36:03  geconomos
// added ability to export vrx from pdf using the command line
//
// Revision 1.6  2005/11/18 16:51:14  geconomos
// Writing server registration results to cout
//
// Revision 1.5  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.4  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.3  2005/11/07 20:38:56  geconomos
// moved nested classes to own files
//
// Revision 1.2.4.1  2005/11/10 15:47:30  geconomos
// issue# 4529: only vxservice user can log in
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.18.2.4.2.1.6.1  2005/06/17 13:15:37  geconomos
// removed brush initialzation routine and registered background images
//
// Revision 3.18.2.4.2.1  2005/03/28 20:44:26  frank
// logged hResults for aid future debugging
//
// Revision 3.18.2.4  2005/02/15 20:00:08  jmeade
// merge from vc1.4 branch.
//
// Revision 3.18.2.3.2.2  2004/12/17 03:33:24  jmeade
// Better notification of errors to user and in log.
//
// Revision 3.18.2.3.2.1  2004/12/14 19:15:20  jmeade
// new licensing.
//
// Revision 3.18.2.3  2004/09/16 22:07:38  jmeade
// Provide a splash screen outside the scope of InitInstance.
//
// Revision 3.18.2.2  2004/01/30 00:33:20  jmeade
// Issue 3669: Interface for specifying anonymization of report for preview.
//
// Revision 3.18.2.1  2003/07/08 22:55:13  jmeade
// Code standards, comments.
//
// Revision 3.18  2002/11/21 02:58:55  jmeade
// Moved app mutex code to separate file.
//
// Revision 3.17  2002/09/17 22:30:50  jmeade
// Moved v3D application interface classes and methods to ConsoleLib.
//
// Revision 3.16  2002/09/16 23:00:01  jmeade
// Moving transfer and background process functionality to ConsoleLib.
//
// Revision 3.15  2002/09/10 19:36:12  geconomos
// Moved application mutex checking to static function.
//
// Revision 3.14  2002/09/09 19:26:05  jmeade
// Merged with XP_1-0 branch (tag: XP_1-0_Joined_On_09-06-2002)
//
// Revision 3.13  2002/08/26 16:25:11  jmeade
// Proper include files.
//
// Revision 3.12.2.1  2002/08/16 16:17:35  geconomos
// Made calcium scoring viewer management like the colon viewer.
//
// Revision 3.12  2002/05/10 21:54:54  jmeade
// Issue 2096:  Button to return to current v3D app.
//
// Revision 3.11  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.10  2002/03/28 18:53:51  jmeade
// Store app-specific settings in separate registry section;
// Check location lock when opening from MRU menu.
//
// Revision 3.9  2002/03/13 22:01:16  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.8.2.4  2002/04/11 21:55:12  jmeade
// App-specific text in about menu.
//
// Revision 3.8.2.3  2002/04/02 16:58:02  jmeade
// Issue 2026: Allow open of single dataset from Study Info.
//
// Revision 3.8.2.2  2002/03/28 22:31:18  jmeade
// Store app-specific settings in separate registry section;
// Check location lock when opening from MRU menu.
//
// Revision 3.8.2.1  2002/03/06 01:33:01  jmeade
// GetAppPathname() fn.
//
// Revision 3.8  2002/01/20 00:59:21  jmeade
// Issue 1525:  Better use of mutex locks to keep track of current viewer/console instances.
//
// Revision 3.7  2002/01/16 18:42:21  jmeade
// Issue 1441:  No recent file list in colon viewer, ensure recent list pointer is non-NULL before accessing.
//
// Revision 3.6  2002/01/12 06:09:35  jmeade
// RemoveFilePathExternal function.
//
// Revision 3.5  2001/12/29 00:11:31  jmeade
// Merge from 1.1 branch.
//
// Revision 3.4  2001/12/18 17:37:24  jmeade
// Ensure you'll only run one instance of console and colon viewer.
//
// Revision 3.3  2001/12/12 21:52:47  jmeade
// Cosmetic:  Splitting Study Browser off as its own app.
//
// Revision 3.2  2001/11/17 03:43:00  jmeade
// Threw out the old carpet.
//
// Revision 3.1  2001/11/09 01:17:50  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.3  2001/11/15 19:19:29  jmeade
// Launching our help.
//
// Revision 3.0.2.2  2001/10/30 23:10:42  jmeade
// Comments and conventions
//
// Revision 3.0.2.1  2001/10/29 22:35:39  jmeade
// Comment
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:43:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:29:18   ingmar
// Initial revision.
// Revision 1.14  2001/08/23 22:01:44  jmeade
// Store parameter count, handle case where command line consists only of study name
//
// Revision 1.13  2001/08/01 20:37:28  jmeade
// intermediate checkin for new database changes
//
// Revision 1.12  2001/07/27 23:00:55  jmeade
// Inclusion of 40_database
//
// Revision 1.11  2001/06/08 01:06:12  jmeade
// coding conventions; changes for new database too numerous to mention
//
// Revision 1.10  2001/04/13 02:35:22  jmeade
// headers and footers
//
// Revision 1.9  2001/04/06 20:07:10  jmeade
// embed helper CCommandLineInfo class in CViatronApp
//
// Revision 1.8  2001/04/05 18:49:47  jmeade
// Coding standards!!!
//
// Revision 1.7  2001/03/30 22:54:18  jmeade
// codings standards; tabs to spaces
//
// Revision 1.6  2001/03/07 15:09:01  jmeade
// bookmarks menu only for standard view
//
// Revision 1.5  2001/02/21 17:41:21  jeff
// new process must keep same logged in user
//
// Revision 1.4  2001/02/09 14:29:33  geconomos
// Changed to terminate study selection process from spawned process to
// reduce flicker.
//
// Revision 1.3  2001/02/08 17:18:10  geconomos
// Launch each study in a new process.
//
// Revision 1.2  2000/12/27 23:49:03  jeff
// global GetViatronApp() ==> static CViatronApp::GetViatronApp()
//
// Revision 1.1.1.1  2000/09/11 00:33:30  ingmar
// unit tests complete
//
// 
// 6     8/28/00 2:42p Antonio
// updated header and footer
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  8/17/00  Time: 10:01p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   AddBookmark menu option
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  8/14/00  Time:  2:32p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  8/14/00  Time:  1:27p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  8/11/00  Time:  7:02p
// Checked in $/ViatronMDI/src/Viatron
// Comment:
//   
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  8/11/00  Time:  3:44p
// Created Viatron.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/Viatron.h,v 1.13 2007/03/08 17:09:48 jmeade Exp $
// $Id: Viatron.h,v 1.13 2007/03/08 17:09:48 jmeade Exp $
