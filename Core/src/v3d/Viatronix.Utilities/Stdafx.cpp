// $Id: Stdafx.cpp,v 1.1.1.1.28.1 2011/05/18 01:56:58 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Stdafx.cpp,v 1.1.1.1.28.1 2011/05/18 01:56:58 mkontak Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com


// stdafx.cpp : source file that includes just the standard includes
// Viatronix.Utilities.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


/*
 * Logs the Exception in to an error file.
 */
void ExceptionToFile( System::String ^ message, System::Exception ^ ex )
{
  try
  {
   // ExceptionToFile( gcnew System::Exception( System::String::Concat( ex->ToString(),System::String::Concat( "Error in reading Install Folder", ex->StackTrace )) ));

    System::String ^ errorFile  = "v3dLoggerException.txt";
    System::IO::TextWriter ^ tw = System::IO::File::AppendText( errorFile );

    // write a line of text to the file
    System::String ^ dateTimeString = System::DateTime::Now.ToString();
    System::String ^ messageHeader  = System::String::Concat( dateTimeString, " Error in creating log" );
    System::String ^ errorMessage   = System::String::Concat( messageHeader, message );
    errorMessage   = System::String::Concat( errorMessage, ex->StackTrace );

    tw->WriteLine( errorMessage );
    tw->Close();
  }
  catch ( System::Exception ^ ex )
  {
    System::String ^ exceptionMessage = System::String::Concat( "Exception occurred in writing error logs", ex->Message );
    throw gcnew System::Exception ( exceptionMessage );
  }
}//LogException( System::Exception ^ ex )

// Revision History:
// $Log: Stdafx.cpp,v $
// Revision 1.1.1.1.28.1  2011/05/18 01:56:58  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/12 14:46:07  mkontak
// Cosmetic
//
// Revision 1.3  2002/08/05 21:00:17  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Stdafx.cpp,v 1.1.1.1.28.1 2011/05/18 01:56:58 mkontak Exp $
// $Id: Stdafx.cpp,v 1.1.1.1.28.1 2011/05/18 01:56:58 mkontak Exp $

