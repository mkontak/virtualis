// $Id: Registry.cpp,v 1.3 2005/04/26 19:16:23 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Registry.cpp,v 1.3 2005/04/26 19:16:23 mkontak Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak mkontak@viatronix.net


// Includes
#include "Stdafx.h"
#include "Registry.h"
#include <windows.h>



// Namespaces
using namespace System;
using namespace Viatronix::Utilities::Registry;
using namespace System::Windows::Forms;

/**
 * Constructor
 *
 * @param sComputer     Remote system to connect to
 * @param eRoot       Root to start
 */
Registry::Registry(String ^ sComputer, KeyRoot eRoot) :
m_sComputer(sComputer)
{

  HKEY hkey;

  if ( m_sComputer->Length > 0 && String::Compare(m_sComputer, System::Windows::Forms::SystemInformation::ComputerName) != 0  )
  {
    RegConnectRegistry(GetUnManagedStdString(m_sComputer).c_str(), (HKEY)eRoot, &hkey);
    m_hkRoot = hkey;
  }
  else
  {
    m_hkRoot = (HKEY)eRoot;
  }

} // Registry()


/**
 * Dispose of object
 */
void Registry::!Registry()
{


  RegCloseKey(m_hkRoot);


  System::GC::SuppressFinalize(this);
} // Dispose()


/**
 *  Sets a registry string value
 *
 * @param sSubKey   Sub Key off the root
 * @param sEntry    The name/tag used for the value
 * @param sValue    The value to set the name to 
 */
bool Registry::SetValue(String ^ sSubKey, String ^ sEntry, String ^ sValue )
{

  HKEY hkSub;
  bool bResult(false);

  //////////////////////
  // Create/Open key
  //////////////////////
  if ( RegCreateKeyEx(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), 0, __nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, __nullptr, &hkSub, __nullptr) == ERROR_SUCCESS )
  {

    /////////////////////////
    // Set the entry value
    /////////////////////////
    if ( RegSetValueEx(hkSub,(String::Compare(sEntry,L"") == 0 ? __nullptr : GetUnManagedStdString(sEntry).c_str()),0, REG_SZ, reinterpret_cast<const BYTE *>(GetUnManagedStdString(sValue).c_str()), (DWORD)sValue->Length) == ERROR_SUCCESS ) 
    {
      bResult = true;
    }

    // Close the section
    RegCloseKey(hkSub);

  } // END ... 

  return(bResult);

} // SetValue()


/**
 * Returns the string registry value
 *
 * @param sSubKey         Sub key to look into for value
 * @param sEntry          The name/tag of value to retrieve
 * @param sDefaultValue   The default value to be returned if the name 
 *                                was not found.
 * @return  The string value of the name/tag or the default value if the name 
 *          was not found.
 */
String ^ Registry::GetValue(String ^ sSubKey, String ^ sEntry, String ^ sDefaultValue )
{
  String ^ sResult = sDefaultValue;
  HKEY hkSub;
  unsigned long uSize(512);
  BYTE szValue[512] = { NULL };
  DWORD dwType = REG_SZ;

  int iRc(ERROR_SUCCESS);
  
  //////////////////////////////////////////
  // Open the key for the section specified
  //////////////////////////////////////////
  if ( (iRc = RegOpenKey(m_hkRoot,  GetUnManagedStdString(sSubKey).c_str(), &hkSub)) == ERROR_SUCCESS  )
  {
  
    //////////////////////////////////
    // Retrieve the entries value 
    ////////////////////////////////
    if ( RegQueryValueEx(hkSub,(String::Compare(sEntry,L"") == 0 ? __nullptr : GetUnManagedStdString(sEntry).c_str()), __nullptr, &dwType, szValue, &uSize) == ERROR_SUCCESS )
    {
      sResult = gcnew String(reinterpret_cast<char *>(szValue));
    }

    // Close the section
    RegCloseKey(hkSub);

  } // END ... If the section exists
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to access ", sSubKey));
  }


  return(sResult);
  
} // GetValue()


/**
 *  Sets a registry string value
 *
 * @param sSubKey   Sub Key off the root
 * @param sEntry    The name/tag used for the value
 * @param fValue    The value to set the name to 
 */
bool Registry::SetValue(String ^ sSubKey, String ^ sEntry, const float  fValue )
{

  HKEY hkSub;
  bool bResult(false);

  //////////////////////
  // Create/Open key
  //////////////////////
  if ( RegCreateKeyEx(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), 0, __nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, __nullptr, &hkSub, __nullptr) == ERROR_SUCCESS )
  {

    String ^ sValue = Convert::ToString(fValue);

    /////////////////////////
    // Set the entry value
    /////////////////////////
    if ( RegSetValueEx(hkSub,GetUnManagedStdString(sEntry).c_str(),0, REG_SZ, reinterpret_cast<const BYTE *>(GetUnManagedStdString(sValue).c_str()), (DWORD)sValue->Length) == ERROR_SUCCESS ) 
    {
      bResult = true;
    }

    // Close the section
    RegCloseKey(hkSub);

  } // END ... 

  return(bResult);

} // SetValue()


/**
 * Returns the string registry value
 *
 * @param sSubKey        Sub key to look into for value
 * @param sEntry         The name/tag of value to retrieve
 * @param sDefaultValue  The default value to be returned if the name 
 *                                was not found.
 * @return  The string value of the name/tag or the default value if the name 
 *          was not found.
 */
float Registry::GetValue(String ^ sSubKey, String ^ sEntry, const float  fDefaultValue )
{
  float fResult = fDefaultValue;
 
  HKEY hkSub;
  unsigned long uSize(512);
  BYTE szValue[512] = { NULL };
  DWORD dwType = REG_SZ;

  int iRc(ERROR_SUCCESS);
  
  //////////////////////////////////////////
  // Open the key for the section specified
  //////////////////////////////////////////
  if ( (iRc = RegOpenKey(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), &hkSub)) == ERROR_SUCCESS  )
  {
  
    //////////////////////////////////
    // Retrieve the entries value 
    ////////////////////////////////
    if ( RegQueryValueEx(hkSub,GetUnManagedStdString(sEntry).c_str(), __nullptr, &dwType, szValue, &uSize) == ERROR_SUCCESS )
    {
      fResult = (float)atof(reinterpret_cast<char *>(szValue));
    }

    // Close the section
    RegCloseKey(hkSub);

  } // END ... If the section exists
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to access ", sSubKey));
  }


  return(fResult);
  
} // GetValue()




/**
 * Writes/Sets an integer value to the registry
 * 
 * @param sSubKey     Registry section
 * @param sEntry      Registry entry
 * @param iValue      Value of entry
 * @return Returns tru if the value was set successfully
 */
bool Registry::SetValue(String ^ sSubKey, String ^ sEntry, const int iValue)
{
  HKEY hkSub;
  bool bResult(false);

  /////////////////////////
  // Create/Open section
  ////////////////////////
  if ( RegCreateKeyEx(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), 0, __nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, __nullptr, &hkSub, __nullptr) == ERROR_SUCCESS )
  {

    DWORD dwValue(iValue);

    if ( RegSetValueEx(hkSub, GetUnManagedStdString(sEntry).c_str(), 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS )
    {
      bResult = true;
    }

    // Close the section
    RegCloseKey(hkSub);

  } // END ... If the Create/Open was successful

  return(bResult);
} // SetValue()


/**
 * Gets an integer value from the registry
 * 
 * @param sSubKey         Registry section
 * @param sEntry          Registry entry
 * @param iDefaultValue   Value returned if entry not found
 *
 * @return The entry value if found or the default value.
 */
int Registry::GetValue(String ^ sSubKey, String ^ sEntry, const int iDefaultValue)
{
  int iResult = iDefaultValue;
  HKEY hkSub;
  DWORD dwValue(iDefaultValue);
  DWORD dwType = REG_DWORD;
  DWORD dwSize = sizeof(DWORD);

  int iRc(ERROR_SUCCESS);

  ///////////////////////
  // Open the section
  ///////////////////////
  if ( (iRc = RegOpenKey(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), &hkSub)) == ERROR_SUCCESS  )
  {
  
    if ( RegQueryValueEx(hkSub,GetUnManagedStdString(sEntry).c_str(), __nullptr, &dwType, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS )
    {
      iResult = (int)dwValue;
    }

    RegCloseKey(hkSub);

  } // END ... If the open section was successful
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to access ", sSubKey));
  }


  return(iResult);
} // GetValue()

/**
 * Deletes the specified entry from the registry
 */
bool Registry::DeleteValue(String ^ sSubKey, String ^ sEntry)
{
  HKEY hkSub;
  bool bReturn(false);
  
  int iRc(ERROR_SUCCESS);

  ///////////////////////
  // Open the section
  ///////////////////////
  if ( (iRc = RegOpenKey(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), &hkSub)) == ERROR_SUCCESS  )
  {

    if ( RegDeleteValue(hkSub, GetUnManagedStdString(sEntry).c_str()) == ERROR_SUCCESS )
    {
      bReturn = true;
    }
    RegCloseKey(hkSub);

  } // END ... If the open section was successful
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to delete ", sSubKey));
  }


  return(bReturn);

} // RemoveEntry()

/**
 * Opens the specified key
 *
 * @param sKey     key to be opened
 * 
 * @return  the opened key
 */
bool Registry::OpenKey(HKEY & hKey, String ^ sKey, bool bCreate)
{
  
  bool bResult(false);

  //////////////////////////////////
  // If we want to create then create 
  ///////////////////////////////////
  if ( bCreate )
  {
    bResult = CreateKey( hKey, sKey);
  } // END ... If we are to create the key
  else
  {

    int iRc(ERROR_SUCCESS);

    //////////////////////////////////////////////
    // Open/Create the specified section (subkey)
    //////////////////////////////////////////////
    if ( (iRc = RegOpenKey(m_hkRoot, GetUnManagedStdString(sKey).c_str(), &hKey )) == ERROR_SUCCESS )
    {
      bResult = true;
    }
    else if ( iRc == ERROR_ACCESS_DENIED )
    {
      throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to open ", sKey));
    }


  } 

  return(bResult);

} // OpenKey()


/**
 * Creates The section
 *
 * @param sSubKey     SubKey Key to create off the current root
 * 
 * @return  True if successful, false if failure
 */
bool Registry::CreateKey(String ^ sKey)
{
  bool bResult(false);

  HKEY hKey;

  int iRc(ERROR_SUCCESS);

  //////////////////////////////////////////////
  // Open/Create the specified section (subkey)
  //////////////////////////////////////////////
  if ( (iRc = RegCreateKeyEx(m_hkRoot, GetUnManagedStdString(sKey).c_str(), 0, __nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, __nullptr, &hKey, __nullptr)) == ERROR_SUCCESS )
  {
    RegCloseKey(hKey);

    bResult = true;
  }
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to carete ", sKey));
  }


  
  return(bResult);

} // CreateKey()

/**
 * Creates The section
 *
 * @param hKey        Key handle to return
 * @param sSubKey     SubKey Key to create off the current root
 * 
 * @return  True if successful, false if failure
 */
bool Registry::CreateKey(HKEY & hKey, String ^ sKey)
{
  bool bResult(false);

  int iRc(ERROR_SUCCESS);

  //////////////////////////////////////////////
  // Open/Create the specified section (subkey)
  //////////////////////////////////////////////
  if ( (iRc = RegCreateKeyEx(m_hkRoot, GetUnManagedStdString(sKey).c_str(), 0, __nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, __nullptr, &hKey, __nullptr)) == ERROR_SUCCESS )
  {
    bResult = true;
  }
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to create ", sKey));
  }


  
  return(bResult);

} // CreateKey()


/**
 * Deletes the specified section/key
 *
 * @param sSubKey       SubKey to delete
 *
 * @return  True is sucessful, false if failed
 */
bool Registry::DeleteKey(String ^ sKey)
{
  bool bResult(false);

  int iRc(ERROR_SUCCESS);

  if ( (iRc = RegDeleteKey(m_hkRoot, GetUnManagedStdString(sKey).c_str())) == ERROR_SUCCESS )
  {
    bResult = true;
  }
  else if ( iRc == ERROR_ACCESS_DENIED )
  {
    throw gcnew System::UnauthorizedAccessException(String::Concat("Not authorized to delete ", sKey));
  }


  return(bResult);

} // DeleteKey()

/**
 *  Returns true if the specified section is present
 *
 * @param sSubKey         Subkey to find
 *
 * @return Returns true if the sub keey was found, false otherwise.
 */
bool Registry::IsSubKeyPresent(String ^ sSubKey)
{

  HKEY hkSub;
  
  bool bPresent(false);
  
  ///////////////////////////////////////////
  // Attempt to open the specified section
  ///////////////////////////////////////////
  if ( RegOpenKey(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), &hkSub) == ERROR_SUCCESS  )
  {
    RegCloseKey(hkSub);

    bPresent = true;
  }
  
  return(bPresent);
  
} // IsSubKeyPresent()

/**
 *  Returns true if the specified section is present
 *
 * @param sSubKey         Sub key to look into
 * @paran sEntry          Entries name
 *
 * @return Returns true of the entry was found under the subkey specified, false otherwise.
 */
bool Registry::IsEntryPresent(String ^ sSubKey, String ^ sEntry)
{

  HKEY hkSub;
  
  bool bPresent(false);
  
  ///////////////////////////////////////////
  // Attempt to open the specified section
  ///////////////////////////////////////////
  if ( RegOpenKey(m_hkRoot, GetUnManagedStdString(sSubKey).c_str(), &hkSub) == ERROR_SUCCESS  )
  {

    //////////////////////////////////
    // Retrieve the entries value 
    ////////////////////////////////
    if ( RegQueryValueEx(hkSub,GetUnManagedStdString(sEntry).c_str(), __nullptr, __nullptr, __nullptr, __nullptr) == ERROR_SUCCESS )
    {
      bPresent = true;
    }

    RegCloseKey(hkSub);

  }
  
  return(bPresent);
  
} // IsSubKeyPresent()





////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//                              V3D SYSTEM SETTINGS
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////







// Revision History:
// $Log: Registry.cpp,v $
// Revision 1.3  2005/04/26 19:16:23  mkontak
// no message
//
// Revision 1.2  2004/03/29 21:56:48  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.19  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.18  2003/09/04 14:46:20  mkontak
// Merge with XP_1_1_09042003
//
// Revision 1.17.2.1  2003/09/02 16:18:18  mkontak
// Minor change
//
// Revision 1.17  2003/06/20 18:33:09  mkontak
// Coding standards
//
// Revision 1.16  2003/05/05 12:23:30  mkontak
// Reorganized registry added new funtionality
//
// Revision 1.15  2003/04/15 14:10:38  mkontak
// Re-Write of CDR
//
// Revision 1.14  2003/04/09 13:59:22  mkontak
// Added DicomMessengerHost to the DicomRegistry to handle remote access
// from the STUDY BROWSER in manual mode.
//
// Revision 1.13  2003/04/02 20:12:23  mkontak
// Added RequeryInterval to registry for console.
//
// Revision 1.12  2003/03/19 18:54:48  mkontak
// Fixed DicomMessengerHost tag
//
// Revision 1.11  2003/03/19 18:47:55  mkontak
// Added Dicom Messenger Host/Port to V3DSystemSettings
//
// Revision 1.10  2003/03/12 21:00:21  mkontak
// Added InstaledModle property
//
// Revision 1.9  2003/03/12 16:12:35  geconomos
// Moved inclusion of windows.h to source file.
//
// Revision 1.8  2002/11/25 22:22:11  mkontak
// Fixed memory leak
//
// Revision 1.7  2002/10/15 19:28:25  mkontak
// Extended registry to include services
//
// Revision 1.6  2002/09/13 19:35:45  mkontak
// Issue 2628: Can only set read access for the ODBC and not write. Use open
//                    before create.
//
// Revision 1.5  2002/09/09 18:39:47  mkontak
// Added connecting string for SQL server DSN entries
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
// Revision 3.14  2002/07/23 15:43:28  dmitry
// Overloaded GetValue and Set value to handle 32 bit values in Registry base class and SoftwareRegistry child.
//
// Revision 3.13  2002/06/26 15:08:48  mkontak
// Fixe for Tower, Added DeletEvent() to Events class
//
// Revision 3.12  2002/06/04 12:58:34  mkontak
// no message
//
// Revision 3.11  2002/04/26 15:57:34  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.10  2002/03/12 21:52:01  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.9  2002/03/12 15:14:19  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.5.2.2  2002/03/12 15:13:20  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.5.2.2  2002/03/12 15:13:20  mkontak
// Patch to allow iterating though the installed modules list
//
// Revision 3.5.2.1  2002/02/06 23:20:19  mkontak
// Added InstalledModules class to get the default module
//
// Revision 3.5  2002/01/28 14:23:49  mkontak
// Moved auto and dicomMessengerPort flags to DcmServerService
//
// Revision 3.4  2002/01/21 23:12:16  mkontak
// Added DSNRegistry class to read DSN entries
//
// Revision 3.3  2002/01/02 22:34:37  mkontak
// Added Dicom Messenger Port
//
// Revision 3.2  2001/12/31 20:55:47  mkontak
// Added auto system parameter
//
// Revision 3.1  2001/11/09 01:16:58  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.2  2001/11/02 18:33:50  mkontak
// Fixed issues reading command line options when setting default parameter
// values containing spaces.
//
// Revision 3.0.2.1  2001/01/13 07:47:41  mkontak
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
// Revision 1.1  2001/08/21 15:06:19  mkontak
// Changed cpp to C
//
// Revision 1.2  2001/08/21 13:25:23  mkontak
// Updated registry
//
// Revision 1.3  2001/08/07 21:12:39  mkontak
// deleted the SetValue() for a DWORD since it was not implemented
//
// Revision 1.2  2001/08/07 16:23:57  mkontak
// Added comment blocks to the beginning and end of the file
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/Registry.cpp,v 1.3 2005/04/26 19:16:23 mkontak Exp $
// $Id: Registry.cpp,v 1.3 2005/04/26 19:16:23 mkontak Exp $
