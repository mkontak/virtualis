// $Id: SoftwareRegistry.h,v 1.4 2004/04/12 14:21:59 mkontak Exp $
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

// Includes
#include "Registry.h"


// Namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Specialized;

OPEN_REGISTRY_NS


/**
 * Implements a Software Registry (HKEY_LOCAl_MACHINE\SOFTWARE) access point
 */
public ref class SoftwareRegistry : public Registry
{
public:

  /// Constructor
  SoftwareRegistry(System::String ^ sCompanyName, System::String ^ sProductName);

  /// Constructor
  SoftwareRegistry(System::String ^ sSystem, System::String ^ sCompanyName, System::String ^ sProductName);

  /// Destructor
  virtual ~SoftwareRegistry() { }

  /// Returns the specified string entry value under the section
  virtual System::String ^ GetValue(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sDefaultValue) override;

  /// Returns the specified string entry value
  virtual System::String ^ GetValue(System::String ^ sEntry, System::String ^ sDefaultValue) override;

  /// Sets the specified string entry value under the section
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sValue ) override;

  /// Set the specified string entry value
  virtual bool SetValue(System::String ^ sEntry, System::String ^ sValue)  override;
  
  /// Gets the specified interger entry value under the section
  virtual int GetValue(System::String ^ sSubKey, System::String ^ sEntry, const int iDefaultValue)  override;

  /// Gets the specified interger entry value
  virtual int GetValue(System::String ^ sEntry, const int iDefaultValue)  override;

  /// Sets the specifeid integer entry value under the section
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, const int iValue)  override;

  /// Sets the specifeid integer entry value under the section
  virtual bool SetValue(System::String ^ sEntry, const int iValue)  override;

  /// Returns the specified registry value
  virtual float GetValue(System::String ^ sSubKey, System::String ^ sEntry, const float  fDefaultValue)  override;

  /// Pure virtual function must be defined in the super class
  virtual float GetValue(System::String ^ sEntry, const float  fDefaultValue)  override;

  /// Sets the specified registry value
  virtual bool SetValue(System::String ^ sSubKey, System::String ^ sEntry, const float  fValue )  override;

  /// Pure virtual function must be defined in the super class
  virtual bool SetValue(System::String ^ sEntry, const float  fValue )  override;

  /// Deletes the value
  virtual bool DeleteValue(System::String ^ sSubKey, System::String ^ sEntry)  override;

  /// Deletes the value
  virtual bool DeleteValue(System::String ^ sEntry)  override;

  /// Returns true if the sub key exits
  virtual bool IsSubKeyPresent(System::String ^ sSubKey)  override;

  /// Returns true if the entry is present
  virtual bool IsEntryPresent(System::String ^ sEntry)  override;

  /// Deletes the specified subkey
  virtual void DeleteSubKey(System::String ^ sSubKey);

  /// Creates the specified subkey
  virtual void CreateSubKey(System::String ^ sSubKey);

  /// Clears the list of parameters
  void Clear() { m_pList->Clear(); }

  /// Reads the registry values
  virtual void Read();

  /// Writes the registry values
  virtual void Write();

public:

  /// Returns the product key directory
  property System::String ^ ProductKey { String ^ get() { return(m_sProductKey); } }

  /// Returns the company 
  property System::String ^ Company { String ^ get() { return(m_sCompany); } }

  /// Returns the product
  property System::String ^ Product { String ^ get() { return(m_sProduct); } }

private:

  /// Initialize the object
  void Init(System::String ^ sCompanyName, System::String ^ sProductName);

  /**
   * Implements a registry entry list
   */
  ref class EntryList
  {
  public:
    EntryList() 
    {
      m_pSubKeys = gcnew StringCollection();
      m_pEntries = gcnew ArrayList();
    }

    /// Clears List
    void Clear() { m_pSubKeys->Clear(); m_pEntries->Clear(); }

    ///
    void Add(System::String ^ sEntry, System::String ^ sValue ) { Add(L"", sEntry, sValue); }

    ///
    void Add(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sValue)
    {

      StringDictionary ^ dict = GetEntries(sSubKey);

      if ( dict == __nullptr )
      {
        m_pSubKeys->Add(sSubKey);
        dict = gcnew StringDictionary();
        m_pEntries->Add(dict);
      }

      try
      {
        dict->Add(sEntry, sValue);
      }
      catch ( System::ArgumentException ^ )
      {
        // Ignore
      }

    } // Add()

    /// Gets an entry list for the index specified
    StringDictionary ^ GetEntryList(int iIndex) { return(dynamic_cast<StringDictionary ^>(m_pEntries[iIndex])); }

    /// Returns the SUBKEY Count
    property int SubKeyCount { int get() { return(m_pSubKeys->Count); } }

    /// Returns the entry count for the main key
    property int Count { int get() { return(GetCount(L"")); } }

    /// Returns the entry count for the specified subkey
    int GetCount(System::String ^ sSubKey) 
    {
      int iCount(0);

      StringDictionary ^ dict = GetEntries(sSubKey);

      if ( dict != __nullptr )
      {
        iCount = dict->Count;
      }

      return(iCount);
    } // get_Count()

    /// Returns the entries value
    String ^ GetValue(String ^ sEntry) { return(GetValue(L"",sEntry)); }
    

    /// Returns the entries value under the specified sub ket
    System::String ^ GetValue(System::String ^ sSubKey, System::String ^ sEntry)
    {
      
      
      StringDictionary ^ dict = GetEntries(sSubKey);

      if ( dict == __nullptr )
      {
        throw  gcnew Exception(L"Sub key does not exists");
      }

      return(dict[sEntry]);
    } // get_Value()
    

    /// Sets the entries value
    void SetValue(System::String ^ sSubKey, System::String ^ sEntry, System::String ^ sValue)
    {

      StringDictionary ^ dict = GetEntries(sSubKey);

      if ( dict == __nullptr )
      {
        m_pSubKeys->Add(sSubKey);
        dict = gcnew StringDictionary();
        m_pEntries->Add(dict);
        dict->Add(sEntry, sValue);
      }
      else
      {
        dict[sEntry] = sValue;
      }

    } // set_Value

    /// Returns an entry list
    property StringDictionary ^ Item[int]
    {
      StringDictionary ^ get(int iIndex) { return(dynamic_cast<StringDictionary ^>(m_pEntries[iIndex])); }
    }

    /// Returns an entry list
    property StringDictionary ^ Item[String ^]
    {
      StringDictionary ^ get(String ^ sSubKey) { return(GetEntries(sSubKey)); }
    }


    /// Returns all entries for the subkey
    StringDictionary ^ GetEntries(System::String ^ sSubKey)
    {
      StringDictionary ^ dict = __nullptr;

      for ( int i(0); i < m_pSubKeys->Count; i++ )
      {
        if ( m_pSubKeys[i] == sSubKey )
        {
          dict = dynamic_cast<StringDictionary ^>(m_pEntries[i]);
        }
      }

      return(dict);  

    } // GetEntries()

  private:

    /// Sub key list
    StringCollection ^ m_pSubKeys;

    /// Entry list
    ArrayList ^ m_pEntries;

  };    // class EntryList

  ///  Add a sub key
  void AddSubKeyValueList(HKEY hkParent, System::String ^ sSubKey);

private:

  /// Software KEY
  static const System::String ^ m_sSoftwareKey = L"SOFTWARE\\Wow6432Node";

  /// Product Key (Company\Product)
  System::String ^ m_sProductKey;

  /// Company Name
  System::String ^ m_sCompany;

  /// Product Name
  System::String ^ m_sProduct;

  ///
  EntryList ^ m_pList;
  
}; // class SoftwareRegistry




CLOSE_REGISTRY_NS


// Revision History:
// $Log: SoftwareRegistry.h,v $
// Revision 1.4  2004/04/12 14:21:59  mkontak
// Reorg of registry and split out to seperate files
//
// Revision 1.3  2004/03/29 21:56:48  mkontak
// Changes to allow for multiple dicom servers
//
// Revision 1.2  2004/03/24 21:33:24  frank
// removed hard-coded paths to installation, log, license, and config directories
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.7  2003/08/05 19:02:15  mkontak
// Added Console Registry
//
// Revision 1.6  2003/08/05 12:43:09  mkontak
// Added entries for GET AETItle and Port
//
// Revision 1.5  2003/07/22 18:34:39  mkontak
// Fixes for configuration
//
// Revision 1.4  2003/07/09 12:27:15  mkontak
// Added flag for ignoring acquisition number as a datasets key
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/SoftwareRegistry.h,v 1.4 2004/04/12 14:21:59 mkontak Exp $
// $Id: SoftwareRegistry.h,v 1.4 2004/04/12 14:21:59 mkontak Exp $
