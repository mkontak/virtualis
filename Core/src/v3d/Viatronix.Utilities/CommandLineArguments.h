// $Id: CommandLineArguments.h,v 1.4 2006/01/17 19:38:16 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


// Pragmas
#pragma once


// Namespaces
OPEN_UTILITIES_NS

/**
  * Implements a command line argument list as a hash table
  *
  */
public ref  class CommandLineArguments
{

public:
  /// Constructor
  CommandLineArguments() { Init(); }

  /// Constructor
  CommandLineArguments(array<System::String ^> ^ sCommandLine);


  /// Returns the command line argments as a string
  virtual System::String ^ ToString() override ;

public:

  /// Returns the arguments 
  property System::Collections::Hashtable ^ Arguments
  { 
    System::Collections::Hashtable ^ get() { return m_pArguments; }; 
  }

  /// Returns the switch
  property System::Char Switch
  { 
    System::Char get() { return  m_cSwitch; }
    void set(System::Char cSwitch) { m_cSwitch = cSwitch; }
  } 

  /// Gets the array list of all arguements with no siwtches
  property System::Collections::ArrayList ^ NoSwitchArguements
  { 
    System::Collections::ArrayList ^ get() { return m_pNoSwitchArguements; }
  }

private:

  /// Initializes the classes memebrs
  void Init();

  /// Parses the command line 
  void ParseCommandLine(array<System::String ^> ^ mpCommandLine);

private:

  /// Hashtable
  System::Collections::Hashtable ^ m_pArguments;

  /// Arguemnt switch character
  System::Char  m_cSwitch;

  /// Contains the remaining argumenets that have no switches
  System::Collections::ArrayList ^ m_pNoSwitchArguements;

}; // class CommandLineArguements


CLOSE_UTILITIES_NS

// $Log: CommandLineArguments.h,v $
// Revision 1.4  2006/01/17 19:38:16  mkontak
// Allow for non-switch command line parameters.
//
// Revision 1.3  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.2  2005/03/30 21:45:39  frank
// typo
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/06/23 15:51:36  dongqing
// code review
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/CommandLineArguments.h,v 1.4 2006/01/17 19:38:16 mkontak Exp $
// $Id: CommandLineArguments.h,v 1.4 2006/01/17 19:38:16 mkontak Exp $
