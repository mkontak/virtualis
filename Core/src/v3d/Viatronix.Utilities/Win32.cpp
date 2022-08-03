// $Id: Win32.cpp,v 1.4 2007/01/31 15:52:01 gdavidson Exp $
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
#include "stdafx.h"
#include "Windows.h"
#include "Win32.h"
#include "Richedit.h"

// Namespaces
USING_UTILITIES_NS

/*
 * Sets Bold on
 *
 * @param pointer to a handle to a rich edit control
 */
void Win32::SetBold( IntPtr hwnd )
{
  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the current character formatting for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFE_BOLD)
  {
    cf.dwMask = CFM_BOLD;
    cf.dwEffects = 0;
  }
  else
  {
    cf.dwMask |= CFM_BOLD;
    cf.dwEffects |= CFE_BOLD;
  }

  // sets character formatting in a rich edit control for the selected text
  SendMessage( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf );

} // SetBold()

/*
 * Sets Underline on
 *
 * @param pointer to a handle to a rich edit control
 */
void Win32::SetUnderline( IntPtr hwnd )
{
  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the current character formatting for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFM_UNDERLINE)
  {
    cf.dwMask = CFM_UNDERLINE;
    cf.dwEffects = 0;
  }
  else
  {
    cf.dwMask |= CFM_UNDERLINE;
    cf.dwEffects |= CFE_UNDERLINE;
  }

  // sets character formatting in a rich edit control for the selected text
  SendMessage( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf );

} // SetUnderline()

/*
 * Sets the italic on
 *
 * @param pointer to a handle to a rich edit control
 */
void Win32::SetItalic( IntPtr hwnd )
{
  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the current character formatting for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFE_ITALIC)
  {
    cf.dwMask = CFM_ITALIC;
    cf.dwEffects = 0;
  }
  else
  {
    cf.dwMask |= CFM_ITALIC;
    cf.dwEffects |= CFE_ITALIC;
  }

  // sets character formatting in a rich edit control for the selected text
  SendMessage( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf );

} // SetItalic()

/*
 * Returns true if the selection is BOLD
 *
 * @param pointer to a handle to a rich edit control
 *
 * @return true if bold, false otherwise
 */
bool Win32::IsSelectionBold( IntPtr hwnd )
{
  bool retValue = false;

  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the character formatting in a rich edit control for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFE_BOLD)
    retValue = true;

  return retValue;
} // IsSelectionBold()

/*
 * Returns true of the selection is underlined
 *
 * @param pointer to a handle to a rich edit control
 *
 * @return true if underline, false otherwise
 */
bool Win32::IsSelectionUnderline( IntPtr hwnd )
{
  bool retValue = false;

  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the character formatting in a rich edit control for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFE_UNDERLINE)
    retValue = true;

  return retValue;
} // IsSelectionUnderline()

/**
 * Returns true of the selection is italic
 *
 * @param pointer to a handle to a rich edit control
 *
 * @return true if italic, false otherwise
 */
bool Win32::IsSelectionItalic( IntPtr hwnd )
{
  bool retValue = false;

  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the character formatting in a rich edit control for the selected text
  GetCharFormat( hWnd, &cf );

  if (cf.dwEffects & CFE_ITALIC)
    retValue = true;

  return retValue;
} // IsSelectionItalic()

/**
 * Returns the character format
 *
 * @param handle to a rich edit control
 *
 * @param pointer to a character formatting structure in a rich edit control.
 */
void Win32::GetCharFormat( HWND hWnd, CHARFORMAT * cf )
{
  memset( cf, 0, sizeof( CHARFORMAT ) );

  cf->cbSize = sizeof( CHARFORMAT );

  // message determines the current character formatting in a rich edit control for the selected text
  SendMessage( hWnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) cf );

} // getChharFormat()

/*
 * Sets the foont
 *
 * @param handle to a rich edit control
 *
 * @param pointer to a managed string object.
 */
void Win32::SetFont( IntPtr hwnd, System::String ^ font, float size )
{
  HWND hWnd = static_cast< HWND >( hwnd.ToPointer() );

  CHARFORMAT cf;

  // get the character formatting in a rich edit control for the selected text 
  GetCharFormat( hWnd, &cf );

  using namespace System::Runtime::InteropServices;

  // copy the contents of a managed string into native heap
  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( font );

  // copy font into character format structure
  strcpy_s( cf.szFaceName, static_cast< const char * >( buffer.ToPointer() ) );

  cf.yHeight = size * 20; // Character height, in twips (1/1440 of an inch or 1/20 of a printer's point). 

  // sets character formatting in a rich edit control for the selected text
  SendMessage( hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf );

  Marshal::FreeHGlobal( buffer );
} // SetFont()


// $Log: Win32.cpp,v $
// Revision 1.4  2007/01/31 15:52:01  gdavidson
// Issue #5357: Fixed an issue when changing the font
//
// Revision 1.3  2006/06/02 12:58:21  mkontak
// Fixed deprecated code
//
// Revision 1.2  2006/06/01 19:05:39  mkontak
// VS 2005 conversion
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/06/26 14:33:14  mkontak
// Coding standards
//
// Revision 1.2  2003/04/07 17:44:00  sgagliardo
// new function SetFont.
//
// Revision 1.1  2003/03/28 17:59:24  sgagliardo
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Win32.cpp,v 1.4 2007/01/31 15:52:01 gdavidson Exp $
// $Id: Win32.cpp,v 1.4 2007/01/31 15:52:01 gdavidson Exp $
