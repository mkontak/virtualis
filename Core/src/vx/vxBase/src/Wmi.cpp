// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Wmi.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#include "stdafx.h"
#include "Wmi.h"
#include "Exception.h"
#include "crypto\CryptHash.h"
#include "crypto\AESCrypt.h"
#include "BitConverter.h"

#include <comdef.h>


// =========================================
// Function to be used by the installer
// ========================================
extern "C" VX_BASE_DLL LPCSTR __stdcall  GetSystemId()
{
  static std::string systemId = sys::Wmi::GetSystemId();
  
  return (LPCSTR)systemId.c_str();
}

using namespace sys;


/**
 * Constructor
 */
Wmi::Wmi() :
m_pLocator(__nullptr),
m_pService(__nullptr),
m_bComInitilized(false)
{
  HRESULT hres;

  // =================
  // Initialize COM
  // ================
  hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 

  // Make sure we were successful
  if (FAILED(hres))
  {
    if ( hres != RPC_E_CHANGED_MODE )
      throw ex::WmiException(LogRec(util::Strings::Format("Failed to initialize COM library [%d]", hres), "Wmi","Wmi")); 
  }
  else
    m_bComInitilized = true;

  // ===================================
  // Set general COM security levels
  // =================================
   hres =  CoInitializeSecurity(
      NULL, 
      -1,                          // COM authentication
      NULL,                        // Authentication services
      NULL,                        // Reserved
      RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
      RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
      NULL,                        // Authentication info
      EOAC_NONE,                   // Additional capabilities 
      NULL                         // Reserved
      );

   
  // Make sure the security setup was successful
  if (FAILED(hres))
    throw ex::WmiException(LogRec(util::Strings::Format("Failed to initialize security [%d]", hres), "Wmi","Wmi"));  
  

  // Create the Locator
  hres = CoCreateInstance( CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &m_pLocator);
  if (FAILED(hres))
     throw ex::WmiException(LogRec(util::Strings::Format("Failed to create IWbemLocator object [%d]", hres), "Wmi","Wmi")); 


  // Connect to the root\cimv2 namespace with
  hres = m_pLocator->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (e.g. Kerberos)
        0,                       // Context object 
        &m_pService              // pointer to IWbemServices proxy
        );
    
  if (FAILED(hres))
     throw ex::WmiException(LogRec(util::Strings::Format("Failed to connect to service [%d]", hres), "Wmi","Wmi")); 
    

  hres = CoSetProxyBlanket( 
                  m_pService,                  // Indicates the proxy to set
                  RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                  RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                  NULL,                        // Server principal name 
                  RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
                  RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                  NULL,                        // client identity
                  EOAC_NONE                    // proxy capabilities 
              );

  if (FAILED(hres))
   throw ex::WmiException(LogRec(util::Strings::Format("Could not set proxy blanket [%d]", hres), "Wmi","Wmi")); 

} // Wmi()


/**
 * Destructor
 */
Wmi::~Wmi()
{

  // Release the service if not NULL
  if ( m_pService != __nullptr )
    m_pService->Release();

  // Release the locator if not NULL
  if ( m_pLocator != __nullptr )
    m_pLocator->Release();

  // Uninitialize COM
  if ( m_bComInitilized )
    CoUninitialize();

} // ~Wm()

/**
 * Gets the specified property value
 *
 * @param sSource       WMI source to search
 * @param sProperty     WMI property to serach for
 *
 * @return Property value of blank f not found
 */
std::string Wmi::GetPropetryValue(const std::string & sSource, const std::wstring & sPropertyName)
{
  HRESULT hres;

  std::wstring sValue;
  std::string sReturn("");

  if ( m_pService != __nullptr )
  {
    std::ostringstream os;

    os << "SELECT * FROM " << sSource;

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator(__nullptr);
    hres = m_pService->ExecQuery( bstr_t("WQL"), bstr_t(os.str().c_str()),  
                                  WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
                                  NULL,
                                  &pEnumerator);
    
    if (FAILED(hres))
      throw ex::WmiException(LogRec(util::Strings::Format("Query for operating system name failed [%d]", hres), "Wmi","GetPropetryValue")); 

 
    IWbemClassObject * pClassObject(__nullptr);
    ULONG uReturn = 0;
   
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObject, &uReturn);

        if(uReturn == 0)
          break;
        
        VARIANT vtProp;

        // Get the value of the Name property
        hr = pClassObject->Get(sPropertyName.c_str(), 0, &vtProp, 0, 0);
        sValue = std::wstring(vtProp.bstrVal);
        VariantClear(&vtProp);

        pClassObject->Release();
        pClassObject = __nullptr;
    } // while

    if ( pClassObject != __nullptr )
      pClassObject->Release();    
   
    for ( std::wstring::iterator it = sValue.begin(); it != sValue.end(); it++ )
    {
      char ch = static_cast<char>(*it);
      sReturn.push_back(ch);
    } // for it
    
  } // if

   return trim_right(sReturn);
} 


/**
 * Gets the system id
 *
 *  @return string of the ID
 */
std::string Wmi::GetSystemId()
{
  static std::string sysID;
  if( sysID.empty() )
  {
    try
    {
      Wmi wmi;

 
      std::string sProcessorId(wmi.GetPropetryValue("Win32_Processor", L"ProcessorId"));

      sysID = crypto::AESCrypt::Encrypt(sProcessorId,false);

    }
    catch ( ex::WmiException &  )
    {
      LogErr("Failed to get the system id", "SystemInformation", "GetProcessorId");
      throw; 
    }
  } // if

  return sysID;
}   // GetSystemId()
