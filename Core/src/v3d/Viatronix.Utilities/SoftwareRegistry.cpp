// $Id: SoftwareRegistry.cpp,v 1.6 2006/05/31 17:54:30 mkontak Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SoftwareRegistry.cpp,v 1.6 2006/05/31 17:54:30 mkontak Exp $
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
#include "SoftwareRegistry.h"
#include "V3DSystemSettings.h"
#include <windows.h>



// Namespaces
using namespace System;
using namespace Viatronix::Utilities::Registry;
using namespace System::Windows::Forms;



/**
 * Constructor.
 *
 * @param sCompanyName        Company name
 * @param sProductname        Product name
 */
SoftwareRegistry::SoftwareRegistry(String ^ sCompanyName, String ^ sProductName ) : 
/**********************************************************************************************************/
Registry(KeyRoot::MACHINE)
{
  Init(sCompanyName, sProductName);
} // SoftwareRegistry()

/**
* Constructor.
*
* @param sComputer             Remote System to connect to
* @param sCompanyName        Company name
* @param sProductname        Product name
*/
SoftwareRegistry::SoftwareRegistry(String ^ sComputer, String ^ sCompanyName, String ^ sProductName ) : 
/***********************************************************************************************************/
Registry(sComputer, KeyRoot::MACHINE)
{
  Init(sCompanyName, sProductName);
} // SoftwareRegistry()



/** 
 * Initializes the object
 */
void SoftwareRegistry::Init(String ^ sCompanyName, String ^ sProductName )
{
  // List of registry entries
  m_pList = gcnew EntryList();

  //////////////////////////////////////
  // Make sure the list was allocated
  //////////////////////////////////////
  if ( m_pList == __nullptr )
  {
    throw gcnew OutOfMemoryException(L"Could no allocate entry list");
  } // END ... If the list was not allocated

  m_sProduct = sProductName;
  m_sCompany = sCompanyName;


  // Reads the registry
  SoftwareRegistry::Read();

  // Set the product key
  String ^ sProductKey = String::Concat(sCompanyName , this->Delimiter, sProductName );

  // Set the product key's path
  m_sProductKey = String::Concat(const_cast<String ^>(m_sSoftwareKey), this->Delimiter, sProductKey, this->Delimiter);  
}

/**
 * Adds a SUB KEY to the parameter list
 *
 * @param hkParent        Parent to add sub key to
 * @paran sSubKey         Sub key to add
 */
void SoftwareRegistry::AddSubKeyValueList(HKEY hkParent, String ^ sSubKey)
{
  HKEY hkSub;

  DWORD dwEntrySize(MAX_PATH);
  char szEntry[MAX_PATH + 1];

  DWORD dwType(0);
  DWORD dwValueSize(MAX_PATH);
  BYTE szValue[MAX_PATH + 1];

  LONG rc(0);

  if ( (rc = RegOpenKey(hkParent, GetUnManagedStdString(sSubKey).c_str(), &hkSub)) == ERROR_SUCCESS )
  {

    /////////////////////////////////////////
    // Get the (default) value for the key
    ////////////////////////////////////////
    if ( RegQueryValueEx(hkSub, NULL, 0, &dwType, szValue, &dwValueSize) == ERROR_SUCCESS )
    {
      m_pList->Add(sSubKey, L"", gcnew String((char *)szValue));
    } // END ... If the the retrieval of the deafult value was successful

    ////////////////////////////////////
    // Get all values for the sub key
    ///////////////////////////////////
    for ( int j(0); true; j++ )
    {
      if ( (rc = RegEnumValue(hkSub, j, (char*)szEntry, &dwEntrySize, 0, &dwType, LPBYTE(&szValue), &dwValueSize)) != ERROR_SUCCESS )
      {
        break;
      }
      m_pList->Add(sSubKey, gcnew String(szEntry), gcnew String((char *)szValue));
    } // END ... for each entry

    RegCloseKey(hkSub);
  } // END ... If the SUB KEY was opened 

} // AddSubKeyToList()


/**
 * Reads the software registry entries
 */
void SoftwareRegistry::Read()
{

  HKEY hkSoftware = NULL;
  LONG rc(0);

  //////////////////////////////////////////////////
  // attempt to open the SOFTWARE registry SUBKEY
  /////////////////////////////////////////////////
  if ( (rc = RegOpenKey(m_hkRoot, GetUnManagedStdString(const_cast<String ^>(m_sSoftwareKey)).c_str(), &hkSoftware)) == ERROR_SUCCESS )
  {

    HKEY hkProduct = NULL;
    String ^ sProductKey = String::Concat(m_sCompany, this->Delimiter, m_sProduct);

    ////////////////////////////////////////////////////////
    // Attempt to create the COMPANY\PRODUCT
    ////////////////////////////////////////////////////////
    if ( (rc = RegOpenKey(hkSoftware, GetUnManagedStdString(sProductKey).c_str(), &hkProduct)) != ERROR_SUCCESS ) 
    {

      ////////////////////////////////////////////////////////
      // Attempt to create the COMPANY\PRODUCT
      ////////////////////////////////////////////////////////
      if ( (rc = RegCreateKeyEx(hkSoftware, GetUnManagedStdString(sProductKey).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkProduct, NULL)) != ERROR_SUCCESS )
      {
        hkProduct = NULL;
      } // END ... If the open of the 
    } // END ... If open of the software key failed


    //////////////////////////////////////////////////////////////////////////////
    // If we were able to open or create the product key then read the values
    /////////////////////////////////////////////////////////////////////////////
    if ( hkProduct != NULL )
    {

      DWORD dwSubKeySize(MAX_PATH);
      char szSubKey[MAX_PATH + 1];

      DWORD dwEntrySize(MAX_PATH);
      char szEntry[MAX_PATH + 1];

      DWORD dwType(0);
      DWORD dwValueSize(MAX_PATH);
      BYTE szValue[MAX_PATH + 1];

   

      /////////////////////////////
      // Loop through all sub keys
      /////////////////////////////
      for ( int i(0); true; i++ )
      {

        //////////////////////////////////////////////////
        // If we reach the END then break out of the loop
        //////////////////////////////////////////////////
        if ( (rc = RegEnumKeyEx(hkProduct, i, (char *)szSubKey, &dwSubKeySize, 0, NULL, NULL, NULL)) != ERROR_SUCCESS )
        {
          break;
        }

        AddSubKeyValueList(hkProduct, gcnew String(szSubKey));

      } // END ... for each sub key in the company/product

      //////////////////////////////////////////////////
      // Get all values under the company/product key
      //////////////////////////////////////////////////
      for ( int i(0); true; i++ )
      {
        if ( (rc = RegEnumValue(hkProduct, i, (char*)szEntry, &dwEntrySize, 0, &dwType, LPBYTE(&szValue), &dwValueSize)) != ERROR_SUCCESS )
        {
          break;
        }

        m_pList->Add(gcnew String(szEntry), gcnew String((char *)szValue));
      } // END ... for each entry


      RegCloseKey(hkProduct);
    }
    else
    {
      array<String ^> ^ sErrorMessage = gcnew array<String ^>(9);
      sErrorMessage[0] = L"Could not open/create the ";
      sErrorMessage[1] = const_cast<String ^>(m_sSoftwareKey);
      sErrorMessage[2] = this->Delimiter;
      sErrorMessage[3] = m_sCompany;
      sErrorMessage[4] = this->Delimiter;
      sErrorMessage[5] = m_sProduct;
      sErrorMessage[6] = L" registry subkey [";
      sErrorMessage[7] = Convert::ToString((long)GetLastError());
      sErrorMessage[8] = L"]";

      throw gcnew Exception(String::Concat(sErrorMessage));
    }
   
    RegCloseKey(hkSoftware);
  }
  else
  {
    array<String ^> ^ sErrorMessage = gcnew array<String ^>(5);
    sErrorMessage[0] = L"Could not open the ";
    sErrorMessage[1] = const_cast<String ^>(m_sSoftwareKey);
    sErrorMessage[2] = L" registry subkey [";
    sErrorMessage[3] = Convert::ToString((long)GetLastError());
    sErrorMessage[4] = L"]";

    throw gcnew Exception(String::Concat(sErrorMessage));
  }

} // Read()

/**
 * Writes the software registry entries
 */
void SoftwareRegistry::Write()
{

  for ( int i(0); i < m_pList->SubKeyCount; i++ )
  {
    StringDictionary ^ pEntryList = m_pList->GetEntryList(i);
    for ( int j(0); j < pEntryList->Count; j++ )
    {
      
    }
  }

} // Write()

/**
 * Retrieves the string value for the entry under the sub key specified
 *
 * @param sSubKey        The sub key to open
 * @param sEntry         The entry name
 * @param sDefaultValue  The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
String ^ SoftwareRegistry::GetValue(String ^ sSubKey, String ^ sEntry, String ^ sDefaultValue )
{
  return(Registry::GetValue(String::Concat(m_sProductKey, sSubKey), sEntry, sDefaultValue));
} // GetValue()

/**
 * Retrieves the string value for the entry 
 *
 * @param sEntry         The entry name
 * @param sDefaultValue  The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
String ^ SoftwareRegistry::GetValue(String ^ sEntry, String ^ sDefaultValue )
{
  return(Registry::GetValue(m_sProductKey, sEntry, sDefaultValue));
} // GetValue()

/**
 * Sets a string registry entry 
 *
 * @param sSubKey       Sub key to find entry in
 * @paran sEntry        Entry name
 * @param sValue        New value
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sSubKey, String ^ sEntry, String ^ sValue )
{
  return(Registry::SetValue(String::Concat(m_sProductKey, sSubKey), sEntry, sValue));
} // SetValue()

/**
 * Sets a string registry entry 
 *
 * @paran sEntry        Entry name
 * @param sValue        New value
 *
 * @return Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sEntry, String ^ sValue )
{
  return(Registry::SetValue(m_sProductKey, sEntry, sValue));
} // SetValue()

/**
 * Retrieves the integer value for the entry under the sub key specified
 *
 * @param sSubKey         The sub key to open
 * @param sEntry          The entry name
 * @param sDefaultValue   The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
int SoftwareRegistry::GetValue(String ^ sSubKey, String ^ sEntry, const int iDefaultValue)
{
  return(Registry::GetValue(String::Concat(m_sProductKey, sSubKey), sEntry, iDefaultValue));
} // GetValue()

/**
 * Retrieves the integer value for the entry 
 *
 * @param sEntry          The entry name
 * @param sDefaultValue   The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
int SoftwareRegistry::GetValue(String ^ sEntry, const int iDefaultValue)
{
  return(Registry::GetValue(m_sProductKey, sEntry, iDefaultValue));
} // GetValue()

/**
 * Sets a string registry entry 
 *
 * @param sSubKey       Sub key to find entry in
 * @paran sEntry        Entry name
 * @param iValue        New value
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sSubKey, String ^ sEntry, const int iValue)
{
  return(Registry::SetValue(String::Concat(m_sProductKey, sSubKey), sEntry, iValue));
} // SetValue()

/**
 * Sets a string registry entry 
 *
 * @paran sEntry        Entry name
 * @param sValue        New value
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sEntry, const int iValue)
{
  return(Registry::SetValue(m_sProductKey, sEntry, iValue));
} // SetValue()


/**
 * Retrieves the float value for the entry under the sub key specified
 *
 * @param sSubKey         The sub key to open
 * @param sEntry          The entry name
 * @param fDefaultValue   The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
float SoftwareRegistry::GetValue(String ^ sSubKey, String ^ sEntry, const float  fDefaultValue)
{
  return(Registry::GetValue(String::Concat(m_sProductKey, sSubKey), sEntry, fDefaultValue));
} // GetValue()

/**
 * Retrieves the integer value for the entry 
 *
 * @param sEntry          The entry name
 * @param fDefaultValue   The default value for the entry.
 *
 * @return  Returns the entries value or the default value if the entry was not found
 */
float SoftwareRegistry::GetValue(String ^ sEntry, const float  fDefaultValue)
{
  return(Registry::GetValue(m_sProductKey, sEntry, fDefaultValue));
} // GetValue()

/**
 * Sets a string registry entry 
 *
 * @param sSubKey       Sub key to find entry in
 * @paran sEntry        Entry name
 * @param fValue        New value
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sSubKey, String ^ sEntry, const float  fValue)
{
  return(Registry::SetValue(String::Concat(m_sProductKey, sSubKey), sEntry, fValue));
} // SetValue()

/**
 * Sets a string registry entry 
 *
 * @paran sEntry        Entry name
 * @param sValue        New value
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::SetValue(String ^ sEntry, const float  fValue)
{
  return(Registry::SetValue(m_sProductKey, sEntry, fValue));
} // SetValue()

/**
 * Delete a  registry entry 
 *
 * @param sSubKey       Sub key to find entry in
 * @paran sEntry        Entry name
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::DeleteValue(String ^ sSubKey, String ^ sEntry)
{
  return(Registry::DeleteValue(String::Concat(m_sProductKey, sSubKey), sEntry));
} // DeleteValue()

/**
 * Deletes a registry entry 
 *
 * @paran sEntry        Entry name
 *
 * @returns Returns true if success
 */
bool SoftwareRegistry::DeleteValue(String ^ sEntry)
{
  return(Registry::DeleteValue(m_sProductKey, sEntry));
} // DeleteValue()

/**
 * Returns true of the sub key is present
 *
 * @param sSubKey       Sub key to find entry in
 *
 * @returns Returns true if the sub key specified is present.
 */
bool SoftwareRegistry::IsSubKeyPresent(String ^ sSubKey)
{
  return(Registry::IsSubKeyPresent(String::Concat(m_sProductKey, sSubKey)));
} // IsSubKeyPresent()

/**
 * Returns true of the entry is present
 *
 * @param sEntry       Entry to find
 *
 * @returns Returns true if the entry specified is present.
 */
bool SoftwareRegistry::IsEntryPresent(String ^ sEntry)
{
  return(Registry::IsEntryPresent(m_sProductKey, sEntry));
} // IsEntryPresent()

/**
 * Deletes the specified sub-key
 *
 * @param sSubKey     Sub key to be deleted
 *
 * @return void
 */
void SoftwareRegistry::DeleteSubKey(String ^ sSubKey)
{
  DeleteKey(String::Concat(m_sProductKey, sSubKey));
} // DeletesSubKey()

/**
 * Creates the specified sub-key
 *
 * @param sSubKey     Sub key to be created
 *
 * @return void
 */
void SoftwareRegistry::CreateSubKey(String ^ sSubKey)
{
  CreateKey(String::Concat(m_sProductKey, sSubKey));
} // DeletesSubKey()




// $Log: SoftwareRegistry.cpp,v $
// Revision 1.6  2006/05/31 17:54:30  mkontak
// Change to VS 2005
//
// Revision 1.5  2004/05/11 19:13:18  mkontak
// Changes for the WQ
//
// Revision 1.4  2004/04/12 14:21:59  mkontak
// Reorg of registry and split out to seperate files
//
// Revision 1.3  2004/03/29 21:56:48  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.2  2004/03/24 21:32:55  frank
// removed hard-coded paths to installation, log, license, and config directories
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/10/31 15:17:44  mkontak
// Added CopyFiles method to IO::File
//
// Revision 1.9  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.8  2003/08/05 19:02:15  mkontak
// Added Console Registry
//
// Revision 1.7  2003/08/05 14:09:46  mkontak
// Requery Interval
//
// Revision 1.6  2003/08/05 12:43:09  mkontak
// Added entries for GET AETItle and Port
//
// Revision 1.5  2003/07/22 18:34:39  mkontak
// Fixes for configuration
//
// Revision 1.4  2003/07/07 19:12:12  mkontak
// Issue 3097
//
// Revision 1.3  2003/07/07 19:00:03  mkontak
// Issue 3097
//
// Revision 1.2  2003/06/20 18:33:09  mkontak
// Coding standards
//
// Revision 1.1  2003/05/05 12:24:16  mkontak
// Reorganized registry added new funtionality
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SoftwareRegistry.cpp,v 1.6 2006/05/31 17:54:30 mkontak Exp $
// $Id: SoftwareRegistry.cpp,v 1.6 2006/05/31 17:54:30 mkontak Exp $
