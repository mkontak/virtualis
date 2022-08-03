// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Wmi.h
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#pragma once


#include <string>
#include <wbemidl.h>


namespace sys
{

/**
 * Class the wraps the WMI calls
 */
class VX_BASE_DLL Wmi
{
public:

  // Constructor
  Wmi();

  // Destructor
  ~Wmi();

  // Gets the property value
  std::string GetPropetryValue(const std::string & sSource, const std::wstring & sPropertyName);
  
 // Gets the system Id
  static std::string GetSystemId();

private:

  // Initialize flag
  bool m_bComInitilized;

  // Locator
  IWbemLocator * m_pLocator;

  // Service
  IWbemServices * m_pService;

};  // class Wmi

};  // namespace sys

