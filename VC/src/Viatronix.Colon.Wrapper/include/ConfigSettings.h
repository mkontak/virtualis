#pragma once

#include <string>

OPEN_VC_NS

class VX_WRAPPER_DLL ConfigSettings
{
public:
  static bool SetValue( const std::string & sSection, const std::string & sEntry, const std::string & sValue );

  static bool SetValue( const std::string & sSection, const std::string & sEntry, const int4 iValue );

  static int4 GetValue( const std::string & sSection, const std::string & sEntry, const int4 iDefaultValue);

  static std::string GetValue( const std::string & sSection, const std::string & sEntry, const std::string & sDefaultValue);

  static std::string GetTranslucency();

  static bool SetTranslucency( const std::string & sXml );

};

CLOSE_VC_NS