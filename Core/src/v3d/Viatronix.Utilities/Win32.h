// $Id: Win32.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Steve Gagliardo sgagliardo@viatronix.com
//
// Complete history on bottom of file


// Includes
#include "Richedit.h"
#include "Windows.h"

// Namespaces
using namespace System;
OPEN_UTILITIES_NS

/**
 * Implements Win32 function
 */
public ref class Win32
{

// memeber functions
public:

  // Win32 functions

  /// Returns true if the text selection is bold
  static bool IsSelectionBold( IntPtr hwnd );

  /// Returns true if the text selection is underlined
  static bool IsSelectionUnderline( IntPtr hwnd );

  /// Returns true if the text selection is italic
  static bool IsSelectionItalic( IntPtr hwnd );

  /// Sets the text to bold
  static void SetBold( IntPtr hwnd );

  /// Sets the text to underline
  static void SetUnderline( IntPtr hwnd );

  /// Sets the text to italic
  static void SetItalic( IntPtr hwnd );

  /// Sets the text fint
  static void SetFont( IntPtr hwnd, System::String ^ font, float size );

private:

  /// Gets the Character format (CHARFORMAT *)cf
  static void Win32::GetCharFormat( HWND hWnd, CHARFORMAT * cf );

}; // class Win32

CLOSE_UTILITIES_NS

// $Log: Win32.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/06/26 14:33:14  mkontak
// Coding standards
//
// Revision 1.3  2003/04/07 17:44:00  sgagliardo
// new function SetFont.
//
// Revision 1.2  2003/03/28 18:04:48  sgagliardo
// correct header info.
//
// Revision 1.1  2003/03/28 17:59:24  sgagliardo
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Win32.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: Win32.h,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $


