// $Id: vxMessageBox.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: george econmos (george@viatronix.com)

#ifndef vxMessageBox_h
#define vxMessageBox_h


class VX_VIEWER_DLL vxMessageBox  
{
// typedefs
private:

  /// function pointer for information function
  typedef void ( * ShowInfoFuncPtr )( const std::string & sMessage );

  /// function pointer for error function
  typedef void ( * ShowErrorFuncPtr )( const std::string & sMessage );
  
  /// function pointer for question function
  typedef bool ( * ShowQuestionFuncPtr )( const std::string & sMessage );

  /// function pointer for modeless information function
  typedef void ( * ShowModelessInfoFuncPtr )( const std::string & sMessage, const uint4 uSeconds );

// functions
public:

  /// displays an information message box
  static void ShowInfo( const std::string & sMessage );

  /// displays an error message box
  static void ShowError( const std::string & sMessage );
 
  /// Displays a question message box
  static bool ShowQuestion( const std::string & sMessage );

  /// Displays a modeless information message box
  static void ShowModelessInformation( const std::string & sMessage, const uint4 uSeconds );

// data
public:

  /// information function pointer
  static ShowInfoFuncPtr m_infoFuncPtr;

  /// error function pointer
  static ShowErrorFuncPtr m_errorFuncPtr;

  /// question function pointer
  static ShowQuestionFuncPtr m_questionFuncPtr;

  /// question function pointer
  static ShowModelessInfoFuncPtr m_modelessInfoFuncPtr;

}; // class vxMessageBox


#endif // vxMessageBox_h


// $Log: vxMessageBox.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/12/15 16:11:31  frank
// Working on showing a modeless information dialog box
//
// Revision 1.6  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.5  2003/04/09 19:50:10  michael
// added comment correction ')'
//
// Revision 1.4  2003/04/09 18:04:07  michael
// put code into source file (more than a simple assignment/return)
//
// Revision 1.3  2003/04/08 15:21:03  michael
// added log information
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxMessageBox.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxMessageBox.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
