// $Id: Registry.h,v 1.4 2006/05/31 17:54:30 mkontak Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Mark Kontak  mkontak@viatronix.com


// Pragmas
#pragma once
#pragma warning (disable : 4503)

using namespace System;

// Namespaces
OPEN_REGISTRY_NS

// Enumerator for Registry Key Roots
public enum class KeyRoot : unsigned long
{  
  ROOT = (unsigned long)HKEY_CLASSES_ROOT, 
  CONFIG = (unsigned long)HKEY_CURRENT_CONFIG,
  USER = (unsigned long)HKEY_CURRENT_USER,
  MACHINE = (unsigned long)HKEY_LOCAL_MACHINE,
  USERS = (unsigned long)HKEY_USERS,
  PERFDATA = (unsigned long)HKEY_PERFORMANCE_DATA 
};

/**
 * Implements a base Registry Class 
 */
public ref  class  Registry abstract
{

public:

  /// Constructor
  Registry(KeyRoot eKeyRoot ) :   m_sComputer(System::Windows::Forms::SystemInformation::ComputerName), m_hkRoot((HKEY)(eKeyRoot)) { }

  /// Constructor
  Registry(System::String ^ sComputer, KeyRoot eKeyRoot );

  ///Destructor
  virtual ~Registry() { this->!Registry(); }

  !Registry();

  /// Concatenates to keys
  static inline System::String ^ Concat(System::String ^ sKey1, System::String ^ sKey2) 
  { return System::String::Concat(sKey1,"\\\\",sKey2); }


  /// Returns the specified registry value
  virtual System::String ^ GetValue(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sDefaultValue);

  /// Pure virtual function must be defined in the super class
  virtual System::String ^ GetValue(System::String ^ sEntry, System::String ^ sDefaultValue) = 0;

  /// Sets the specified registry value
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sValue );

  /// Pure virtual function must be defined in the super class
  virtual bool SetValue(System::String ^ sEntry, System::String ^ sValue ) = 0;

  /// Gets an integer value from the registry
  virtual int GetValue(System::String ^ sSubKey, System::String ^ sEntry, const int iDefaultValue);

  /// Pure virtual function must be defined in the super class
  virtual int GetValue(System::String ^ sEntry, const int iDefaultValue) = 0;

  /// Writes an integer value to the registry
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, const int iValue);

  /// Pure virtual function must be defined in the super class
  virtual bool SetValue(System::String ^ sEntry, const int iValue) = 0;

  /// Returns the specified registry value
  virtual float GetValue(System::String ^ sSubKey, System::String ^ sEntry, const float  fDefaultValue);

  /// Pure virtual function must be defined in the super class
  virtual float GetValue(System::String ^ sEntry, const float  fDefaultValue) = 0;

  /// Sets the specified registry value
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, const float  fValue );

  /// Pure virtual function must be defined in the super class
  virtual bool SetValue(System::String ^ sEntry, const float  fValue ) = 0;

  /// Removes the specified entry from the registry
  virtual bool DeleteValue(System::String ^ sEntry) = 0;

  /// Removes the specified entry from the registry
  virtual bool DeleteValue(System::String ^ sSubKey, System::String ^ sEntry);

  /// Returns true if the specified entry is present
  virtual bool IsEntryPresent(System::String ^ sEntry) = 0;

  /// Returns true if the specified section is present in the registr
  virtual bool IsSubKeyPresent(System::String ^ sSubKey);


protected:


  /// Returns true is the entry is present ubder the section
  bool IsEntryPresent(System::String ^ sSubKey, System::String ^ sEntry);

  /// Opens key and if the create
  bool OpenKey(HKEY & hKey, System::String ^ sKey, bool bCreate);

  /// Creates the speicified key
  bool CreateKey(System::String ^ sKey);

  /// Creates the speicified key and returns the opened handle
  bool CreateKey(HKEY & hKey, System::String ^ sKey);

  /// Deletes the specified key
  bool DeleteKey(System::String ^ sKey);

public:

  /// Returns the registries directories delimiter
  property static String ^ Delimiter 
  { 
    String ^ get() { return(L"\\"); } 
  }

  /// Returns the system connected to
  property String ^ Computer 
  { 
    String ^ get() { return m_sComputer; } 
  }
  

protected:

  // HKEY Root of registry
  HKEY m_hkRoot;        

private:

  /// Remote system computer name
  String ^ m_sComputer;

    
}; // class Registry


CLOSE_REGISTRY_NS


// Revision History:
// $Log: Registry.h,v $
// Revision 1.4  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.3  2004/03/29 21:56:48  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.2  2004/03/05 17:04:54  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.18  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.17  2003/06/20 18:33:09  mkontak
// Coding standards
//
// Revision 1.16  2003/05/05 12:23:30  mkontak
// Reorganized registry added new funtionality
//
// Revision 1.15  2003/04/22 14:00:21  mkontak
// Used #pragma once
//
// Revision 1.14  2003/04/15 14:10:39  mkontak
// Re-Write of CDR
//
// Revision 1.13  2003/04/09 13:59:22  mkontak
// Added DicomMessengerHost to the DicomRegistry to handle remote access
// from the STUDY BROWSER in manual mode.
//
// Revision 1.12  2003/04/02 20:12:23  mkontak
// Added RequeryInterval to registry for console.
//
// Revision 1.11  2003/03/19 18:47:55  mkontak
// Added Dicom Messenger Host/Port to V3DSystemSettings
//
// Revision 1.10  2003/03/14 18:00:56  geconomos
// Qualified Exception class with System::
//
// Revision 1.9  2003/03/12 21:00:21  mkontak
// Added InstaledModle property
//
// Revision 1.8  2003/03/12 16:12:36  geconomos
// Moved inclusion of windows.h to source file.
//
// Revision 1.7  2002/10/15 19:28:25  mkontak
// Extended registry to include services
//
// Revision 1.6  2002/09/09 18:39:47  mkontak
// Added connecting string for SQL server DSN entries
//
// Revision 1.5  2002/08/29 15:30:00  mkontak
// Fixed ReportHeader property
//
// Revision 1.4  2002/08/23 16:54:49  mkontak
// Added additaionl registry supprt
//
// Revision 1.3  2002/08/20 16:00:25  mkontak
// Update for V3DSystemSettings
//
// Revision 1.2  2002/08/16 17:13:08  mkontak
// Enhanced DSNRegistry functionality
//
// Revision 1.1  2002/08/15 21:33:59  mkontak
// Added registry utilties
//
// Revision 3.13  2002/07/23 15:43:27  dmitry
// Overloaded GetValue and Set value to handle 32 bit values in Registry base class and SoftwareRegistry child.
//
// Revision 3.12  2002/07/18 15:11:55  mkontak
// Add VXDLL to class definitions
//
// Revision 3.11  2002/07/02 20:44:12  geconomos
// Exporting classes .
//
// Revision 3.10  2002/06/26 15:08:48  mkontak
// Fixe for Tower, Added DeletEvent() to Events class
//
// Revision 3.9  2002/06/04 12:58:34  mkontak
// no message
//
// Revision 3.8  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.7  2002/03/12 15:14:19  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.4.2.2  2002/03/12 15:13:20  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.4.2.2  2002/03/12 15:13:20  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.4.2.1  2002/02/06 23:20:19  mkontak
// Added VxInstalledModules class to get the default module
//
// Revision 3.4  2002/01/24 02:10:05  jmeade
// List pragma disables prior to any includes.
//
// Revision 3.3  2002/01/21 23:12:16  mkontak
// Added DSNRegistry class to read DSN entries
//
// Revision 3.2  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.1  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.1  2001/01/13 07:47:35  mkontak
// Issue 1247: Added VxServerRegistry class to handle the reading/writing of
//                    registry values for a VX service.
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:30:00   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.3  2001/08/21 14:50:31  mkontak
// Added Sec_1_utils
//
// Revision 1.2  2001/08/21 13:25:16  mkontak
// Updated registry
//
// Revision 1.3  2001/08/07 21:12:34  mkontak
// deleted the SetValue() for a DWORD since it was not implemented
//
// Revision 1.2  2001/08/07 16:23:48  mkontak
// Added comment blocks to the beginning and end of the file
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Registry.h,v 1.4 2006/05/31 17:54:30 mkontak Exp $
// $Id: Registry.h,v 1.4 2006/05/31 17:54:30 mkontak Exp $
