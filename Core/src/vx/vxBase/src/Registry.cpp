//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] (mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "Registry.h"
#include "SoftwareRegistry.h"


// namespaces
using namespace std;
using namespace sys;

/**
 * Registry constructors
 *
 * @param hkey      Base section 
 * @param sSubKey   Sub-key in section
 * @param Type  64 bit or 32 bit
 * @param access    Core access to entries, ACCESS_OPTION_READ, ACCESS_OPTION_WRITE, ACCESS_OPTION_READ | ACCESS_OPTION_WRITE or ACCESS_OPTION_ALL 
 *
 **/
Registry::Registry(HKEY hkey, const std::string & sSubKey, AccessOptions access, DWORD keySet) :
m_hkey(hkey),
m_sSubKey(sSubKey),
m_access(access),
m_keySet(keySet),
m_subkey(NULL)
{
  
}

/**
 * Destructor
 **/
Registry::~Registry()
{
  Close();
}

/**
* Returns the registry key for this sub key
*
* @param options      ACCESS options
*
* @return Returns the registry key for this sub key
*/
void Registry::Open(AccessOptions access, bool alwaysCreate )
{

  if (m_subkey == NULL)
  {

    long rc(ERROR_SUCCESS);

    if ((rc = RegOpenKeyEx(m_hkey, (LPCSTR)m_sSubKey.c_str(), 0, (DWORD)(access | m_keySet), &m_subkey)) != ERROR_SUCCESS)
    {
      if (rc == 2 && alwaysCreate)
        Create(access, false);
      else
        throw ex::VException(LogRec(util::Strings::Format("Could not open registry key for SystemSettings [rc = %d]", rc), "Registry", "Open"));
    }

  }

} // Open()

/**
* Closes the supplied key
*
* @param hkey  Registry key to close
*/
void Registry::Close()
{
  if (m_subkey != NULL)
  {

    RegCloseKey(m_subkey);
    m_subkey = NULL;

  }
} // Close(HKEY hkey)


/**
* Determines if the sub-key exists
*
* @param hkey     Base sub-key
* @param sSubKey  Sub key
* @param Type Type of the key (32BIT or 64BIT)
*
* @return Returns true if the sub-key exists false otherwise
*/
bool Registry::Exists()
{

  bool bReturn(true);

  if (m_subkey == NULL)
  {

    SCOPE_EXIT
    {
      Close();
    };


    try
    {

      Open(AccessOptions::ACCESS_OPTION_READ, false);

      bReturn = true;
    }
    catch (...)
    {
      bReturn = false;
    }

  }

  return bReturn;

} // Exist()

/**
* Creates the sub-key
*
* @param hKey       Base section
* @param sSubKey    Sub-key off the above section
* @param Type   Type of source 64BIT or 32BIT
*/
void Registry::Create(AccessOptions access, bool closeAfterCreate)
{


  long rc(ERROR_SUCCESS);

  SCOPE_EXIT
  {
    if (closeAfterCreate)
     Close();
  };

  try
  {
    if ((rc = RegCreateKeyEx(m_hkey, (LPCSTR)m_sSubKey.c_str(), 0, NULL, 0, (DWORD)(access | m_keySet), NULL, &m_subkey, NULL)) != ERROR_SUCCESS)
      throw ex::VException(LogRec(util::Strings::Format("Could not create registry key  %s [error =%d]", m_sSubKey.c_str(), rc), "Registry", "OpenKey"));

  }
  catch (...)
  {
    throw;
  }



} // Create()


/**
* Attempts to read the DWORD from the registry
*
* @param sEntry      Name of entry
* @param value       Value to be returned
**/
bool Registry::TryGetQWORD(const std::string & sEntry, __int64 * value)
{
  bool found = false;

  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_READ);

    DWORD size(sizeof(DWORD));
    DWORD type(REG_QWORD);
    long rc(ERROR_SUCCESS);

    if ((rc = RegQueryValueEx(m_subkey, sEntry.c_str(), NULL, &type, (LPBYTE)value, &size)) == ERROR_SUCCESS)
      found = true;
    else
      LogErr(util::Strings::Format("Could not retrieve DWORD value from registry [entry = %s] [error = %d]", sEntry.c_str(), rc), "Registry", "GetValue");

  }
  catch (...)
  {
    LogErr(util::Strings::Format("Could not retrieve DWORD value from registry [entry = %s], unspecified error", sEntry.c_str()), "Registry", "GetValue");
    found = false;
  }


  return found;

}




/**
 * Attempts to read the DWORD from the registry
 *
 * @param sEntry      Name of entry
 * @param value       Value to be returned
 **/
bool Registry::TryGetDWORD(const std::string & sEntry, DWORD * value)
{
  bool found = false;

  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_READ);

    DWORD size(sizeof(DWORD));
    DWORD type(REG_DWORD);
    long rc(ERROR_SUCCESS);

    if ((rc = RegQueryValueEx(m_subkey, sEntry.c_str(), NULL, &type, (LPBYTE)value, &size)) == ERROR_SUCCESS)
      found = true;
    else
      LogErr(util::Strings::Format("Could not retrieve DWORD value from registry [entry = %s] [error = %d]", sEntry.c_str(), rc), "Registry", "GetValue");

  }
  catch (...)
  {
    LogErr(util::Strings::Format("Could not retrieve DWORD value from registry [entry = %s], unspecified error", sEntry.c_str()), "Registry", "GetValue");
    found = false;
  }

  return found;

}



/**
* Attempts to read the binary data from the registry
*
* @param sEntry      Name of entry
* @param value       Value to be returned
**/
bool Registry::TryGetString(const std::string & sEntry, std::string & value )
{
  bool found = false;

  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_READ);

    DWORD size(1024);
    BYTE buffer[1024];
    DWORD type(REG_SZ);
    long rc(ERROR_SUCCESS);

    if ((rc = RegQueryValueEx(m_subkey, sEntry.c_str(), NULL, &type, buffer, &size)) == ERROR_SUCCESS)
    {
      value.clear();
      value.assign(reinterpret_cast<char *>(buffer));
      found = true;
    }
    else
      LogErr(util::Strings::Format("Could not retrieve string value from registry [entry = %s] [error = %d]", sEntry.c_str(), rc), "Registry", "GetValue");

  }
  catch (...)
  {
    LogErr(util::Strings::Format("Could not retrieve string value from registry [entry = %s], unspecified error", sEntry.c_str()), "Registry", "GetValue");
    found = false;
  }

  return found;

}



/**
 * Attempts to read the binary data from the registry
 *
 * @param sEntry      Name of entry
 * @param buffer      Binary buffer
 * @param size        Size of the buffer
 **/
bool Registry::TryGetBinary(const std::string & sEntry, LPBYTE buffer, LPDWORD size)
{
  bool found = false;

  SCOPE_EXIT
  {
    Close();
  };
  
  try
  {
    Open(AccessOptions::ACCESS_OPTION_READ);

    DWORD type(REG_BINARY);
    long rc(ERROR_SUCCESS);

    if ((rc = RegQueryValueEx(m_subkey, sEntry.c_str(), NULL, &type, buffer, size)) == ERROR_SUCCESS)
      found = true;
    else
      LogErr(util::Strings::Format("Could not retrieve binary value from registry [entry = %s] [error= %d]", sEntry.c_str(), rc), "Registry", "GetValue");

  }
  catch (...)
  {
    LogErr(util::Strings::Format("Could not retrieve binary value from registry [entry = %s], unspecified error", sEntry.c_str() ), "Registry", "GetValue");
    found = false;
  }

  return found;

}


/**
 * Sets a binary value
 *
 * @param sEntry        Entry name
 * @param buffer        Buffer to be used
 * @param size          Number of bytes from the buffer to set
 **/
void Registry::SetBinary(const std::string & sEntry, LPBYTE buffer, DWORD size)
{
  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_WRITE);


    int rc(0);

    if ((rc = RegSetValueEx(m_subkey, sEntry.c_str(), 0, REG_BINARY, (LPBYTE)buffer, size)) != ERROR_SUCCESS)
      throw ex::VException(LogRec(util::Strings::Format("Failed to set binary for entry %s [rc = %d]", sEntry.c_str(), rc), "Registry", "SetValue"));
    
  }
  catch (ex::VException)
  {
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec(util::Strings::Format("Failed to set binary for entry %s", sEntry.c_str()), "Registry", "SetValue"));
  }

}   // SetValue

/**
 * Sets a string value
 *
 * @param  sEntry     Entry name
 * @param sValue      String value to be set
 **/
void Registry::SetString(const std::string & sEntry, const std::string & sValue)
{

  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_WRITE);

    int rc(0);

    if ((rc = RegSetValueEx(m_subkey, sEntry.c_str(), 0, REG_SZ, (LPBYTE)sValue.c_str(), sValue.size())) != ERROR_SUCCESS)
      throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s [rc = %d]", sEntry.c_str(), rc), "Registry", "SetValue"));



  }
  catch (ex::VException)
  {
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s", sEntry.c_str()), "Registry", "SetValue"));
  }

}   // SetValue


/**
 * Sets a int value
 *
 * @param sEntry    Entry name
 * @param iValue    DWORD value to set
 **/
void Registry::SetDWORD(const std::string & sEntry, DWORD iValue)
{

  SCOPE_EXIT
  {
    Close();
  };

  try
  {

    Open(AccessOptions::ACCESS_OPTION_WRITE);

    int rc(0);

    if ( ( rc = RegSetValueEx(m_subkey, sEntry.c_str(), 0, REG_DWORD, (LPBYTE)(&iValue), sizeof(DWORD))) != ERROR_SUCCESS)
      throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s [rc = %d]", sEntry.c_str(), rc), "Registry", "SetValue"));



  }
  catch (ex::VException)
  {
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s", sEntry.c_str()), "Registry", "SetValue"));
  }

}   // SetValue

/**
 * Sets a Quad-Word value
 *
 * @param sEntry      Entry name
 * @param iValue      Quad-Word value to set
 **/
void Registry::SetQWORD(const std::string & sEntry, __int64 iValue)
{
  SCOPE_EXIT
  {
    Close();
  };

  try
  {
    Open(AccessOptions::ACCESS_OPTION_WRITE);

    int rc(0);

    if ((rc = RegSetValueEx(m_subkey, sEntry.c_str(), 0, REG_QWORD, (LPBYTE)(&iValue), sizeof(__int64))) != ERROR_SUCCESS)
      throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s [rc = %d]", sEntry.c_str(), rc), "Registry", "SetValue"));



  }
  catch (ex::VException)
  {
    throw;
  }
  catch (...)
  {
    throw ex::VException(LogRec(util::Strings::Format("Failed to set value for entry %s", sEntry.c_str()), "Registry", "SetValue"));
  }

}   // SetValue

