//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Mark Kontak (mailto:mkontak@viatronix.com)


// pragmas
#pragma once

#include <string>

namespace sys
{

#pragma region AccessOptions

  // open options
  typedef enum AccessOptions
  {
    ACCESS_OPTION_READ = KEY_READ,
    ACCESS_OPTION_WRITE = KEY_WRITE,
    ACCESS_OPTION_EXECUTE = KEY_EXECUTE,
    ACCESS_OPTION_ALL = KEY_ALL_ACCESS
  } AccessOptions;

#pragma endregion

#pragma region class Registry

/*
*  Implements a base Registry class
*/
class VX_BASE_DLL Registry
{

#pragma region constructors and destructors


protected:

  Registry() { }


protected:

  /// Constructor 
  Registry(HKEY hkey, const std::string & sSubKey, AccessOptions access, DWORD keySet = KEY_WOW64_64KEY );

  // Destructor
  virtual ~Registry();

#pragma endregion

#pragma region methods


private:

    /// Creates this sub-key if it does not exist and returns the key (You must close it)
  void Create(AccessOptions access, bool closeAfterCreate = true);

  /// Creates this sub-key if it does not exist and returns the key (You must close it)
  void Open(AccessOptions access, bool alwaysCreate);

public:

  virtual bool Exists();

  /// Creates this sub-key if it does not exist and returns the key (You must close it)
  virtual void Create() { Create(AccessOptions::ACCESS_OPTION_ALL); }

  /// Opens the registry
  virtual void Open(AccessOptions access) {  Open(access, true); }

  /// Closes the registry
  virtual void Close();

public:

  // Tries to get the QUAD-WORD value from the registry
  virtual bool TryGetQWORD(const std::string & sEntry, __int64 * value);

  // Tries to get the DWORD value from the registry
  virtual bool TryGetDWORD(const std::string & sEntry, DWORD * value);

  // Tries to get the string from the registry
  virtual bool TryGetString(const std::string & sEntry, std::string & value);

  // Tries to get the binary buffer from the registry
  virtual bool TryGetBinary(const std::string & sEntry, LPBYTE buffer, LPDWORD size);
 
  // Set a string value
  virtual void SetString(const std::string & sEntry, const std::string & value);

  // Set a binary value
  virtual void SetBinary(const std::string & sEntry, LPBYTE buffer, DWORD size);


  // Set a Quad-Word
  virtual void SetQWORD(const std::string & sEntry, __int64 iValue);

  // Set a Double-Word
  virtual void SetDWORD(const std::string & sEntry, DWORD iValue);

#pragma endregion

#pragma region properties

public:


  virtual const std::string & GetSubKey() { return m_sSubKey; }


#pragma endregion

#pragma region fields

private:

  HKEY m_hkey;

  HKEY m_subkey;

  /// Sub Key
  std::string m_sSubKey;

  /// Access options
  AccessOptions m_access;

protected:

  DWORD m_keySet;

#pragma endregion


};  // class Registry

#pragma endregion


#pragma region class Registry32

/*
*  Implements a software registry class
*/
class VX_BASE_DLL Registry32 : public Registry
{

protected:

  Registry32()  { }

  // functions
public:

  /// Constructor 
  Registry32(HKEY hkey, const std::string & sSubKey, AccessOptions access = AccessOptions::ACCESS_OPTION_ALL) :
    Registry(hkey, sSubKey, access, KEY_WOW64_32KEY)
  {
  }

  // Destructor
  virtual ~Registry32() {}


}; // class SoftwareRegistry32

#pragma endregion

#pragma region class Registry64


/*
*  Implements a software registry class
*/
class VX_BASE_DLL Registry64 : public Registry
{

protected:

  Registry64() { }

  // functions
public:

  /// Constructor 
  Registry64(HKEY hkey, const std::string & sSubKey, AccessOptions access = AccessOptions::ACCESS_OPTION_ALL) :
    Registry(hkey, sSubKey, access) { }

  // Destructor
  virtual ~Registry64() {}


}; // class SoftwareRegistry32

#pragma endregion


} // namespace sys


