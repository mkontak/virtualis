// $Id: CommandLineArguments.cpp,v 1.3 2006/01/17 19:38:16 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


// Include
#include "stdafx.h"
#include "CommandLineArguments.h"


// Namespace
using namespace System;
using namespace System::Text;

USING_UTILITIES_NS


/**
 * Constructor
 *
 * @param pCommandLine    String array of command line arguments
 */
CommandLineArguments::CommandLineArguments(array<String ^> ^ sCommandLine )
{
  Init();
  ParseCommandLine(sCommandLine);
} // CommandLineArguments()


/**
 * Initializes the Command Line data members
 */
void CommandLineArguments::Init()
{

  m_cSwitch = '-';

  m_pArguments = gcnew System::Collections::Hashtable();

  m_pNoSwitchArguements = gcnew System::Collections::ArrayList();

  ////////////////////////////////////////
  // Make sure the new was successful
  ////////////////////////////////////////
  if ( m_pArguments == __nullptr )
  {
    throw gcnew System::OutOfMemoryException("Could not create command line hash table");
  } // END ... If the new failed

} // Init()

/**
 * Parses the command line
 *
 * @param sCommandLine  String array of augruments
 */
void CommandLineArguments::ParseCommandLine(array<String ^> ^ sCommandLine)
{

  m_pArguments->Clear();
      
  String ^ sLastSwitch = L"";

  for( int i(0); i < sCommandLine->Length; i++ )
  {
    String ^ sArg = sCommandLine[i];


    if( String::Compare(sLastSwitch,L"") == 0 && (sArg[0] == '/' || sArg[0] == '-') )
    {
      m_cSwitch = sArg[0];
      sLastSwitch = sArg->Substring( 1 );
      m_pArguments->Add( sLastSwitch, String::Empty );
    }
    else
    {
      if( String::Compare(sLastSwitch,L"") != 0 )
      {
        m_pArguments[sLastSwitch] = sArg;
      }
      else
      {
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// No longer error with invalid arguement but no have a list of arguements with no switches in the order 
        /// in which they were read.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        m_pNoSwitchArguements->Add(sArg);
        //throw gcnew System::ArgumentException(String::Concat("Invalid argument [", sArg, "]"));
      }
      sLastSwitch = L"";
    }
  } // foreach commandline argument


} // ParseCommandLine()


/**
 * Returns the command line as a single string
 *
 * @return Returns the command line as a string
 */
String ^ CommandLineArguments::ToString()
{
  StringBuilder ^ pBuilder = gcnew StringBuilder();

  array<String ^> ^ sKeys = gcnew array<String ^>(m_pArguments->Count);
  array<String ^> ^ sValues = gcnew array<String ^>(m_pArguments->Count);

  m_pArguments->Keys->CopyTo(sKeys,0);
  m_pArguments->Values->CopyTo(sValues,0);

  for ( int i(0); i < m_pArguments->Count; i++ )
  {
    pBuilder->Append(String::Concat(System::Convert::ToString(m_cSwitch),sKeys[i]));
    pBuilder->Append(" ");
    pBuilder->Append(sValues[i]);
    pBuilder->Append(" ");
  } 

  return pBuilder->ToString();
} // ToString()


// $Log: CommandLineArguments.cpp,v $
// Revision 1.3  2006/01/17 19:38:16  mkontak
// Allow for non-switch command line parameters.
//
// Revision 1.2  2004/05/17 20:25:04  mkontak
// Minor changes for the new WQ handler
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/06/30 18:21:02  mkontak
// Added parenthesis around condictional
//
// Revision 1.3  2003/06/23 15:51:36  dongqing
// code review
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/CommandLineArguments.cpp,v 1.3 2006/01/17 19:38:16 mkontak Exp $
// $Id: CommandLineArguments.cpp,v 1.3 2006/01/17 19:38:16 mkontak Exp $
