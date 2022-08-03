#include "Stdafx.h"
#include "ConfigSettings.h"
#include "Exception.h"
#include <iostream>
#include "File.h"

USING_VC_NS
using namespace Viatronix::Colon::Configuration;
using namespace Viatronix::Colon::Data;

bool ConfigSettings::SetValue( const std::string & sSection, const std::string & sEntry, const std::string & sValue )
{
  try
  {
    System::String ^ mpSection( gcnew System::String( sSection.c_str() ));
    RegistrySection::GetSection( mpSection )->Keys[ gcnew System::String( sEntry.c_str() ) ]->Value = gcnew System::String( sValue.c_str() );
    RegistrySection::GetSection( mpSection )->Save( mpSection );
    return true;
  }
  catch( System::Exception ^ mpE )
  {
    LogWrn( ss( mpE->Message ), "ConfigSettings", "SetValue" );
    return false;
  }
} // SetValue( sSection, sEntry, sValue )


bool ConfigSettings::SetValue( const std::string & sSection, const std::string & sEntry, const int4 iValue )
{
  try
  {
    System::String ^ mpSection( gcnew System::String( sSection.c_str() ));
    RegistrySection::GetSection( mpSection )->Keys[ gcnew System::String( sEntry.c_str() ) ]->Set( iValue );
    RegistrySection::GetSection( mpSection )->Save( mpSection );
    return true;
  }
  catch( System::Exception ^ mpE )
  {
    LogWrn( ss( mpE->Message ), "ConfigSettings", "SetValue" );
    return false;
  }
} // SetValue( sSection, sEntry, sValue )


int ConfigSettings::GetValue( const std::string & sSection, const std::string & sEntry, const int4 iDefaultValue)
{
  try
  {
    return RegistrySection::GetSection( gcnew System::String( sSection.c_str() ))->Keys[ gcnew System::String( sEntry.c_str() ) ]->As< int4 >();
  }
  catch( System::Exception ^ mpE )
  {
    LogWrn( ss( mpE->Message ), "ConfigSettings", "SetValue" );
    LogErr( sEntry, "", "" );
    return iDefaultValue;
  }
}


std::string ConfigSettings::GetValue( const std::string & sSection, const std::string & sEntry, const std::string & sDefaultValue)
{
  try
  {
    return ss( RegistrySection::GetSection( gcnew System::String( sSection.c_str() ))->Keys[ gcnew System::String( sEntry.c_str() ) ]->Value );
  }
  catch( System::Exception ^ mpE )
  {
    LogWrn( ss( mpE->Message ), "ConfigSettings", "SetValue" );
    LogErr( sEntry, "", "" );
    return sDefaultValue;
  }
}


std::string ConfigSettings::GetTranslucency()
{
  if( DatabaseGateway::IsEnabled )
  {
    // read translucency from database
    System::String ^ mpXml;
    if( DatabaseGateway::TryGetTranslucency( mpXml ))
      return ss( mpXml );
  }
  
  // read translucency from common user data
  std::string sFile( ss( VCSettingsSection::CommonApplicationData ) + "TranslucencySettings.xml" );
  if( !vx::File::Exists( sFile ))
  {
    // read translucency from execution directory
    sFile = ss( System::AppDomain::CurrentDomain->BaseDirectory ) + "config\\TranslucencySettings.xml";
    if( !vx::File::Exists( sFile ))
    {
      // can't find settings
      return "";
    }
  }

  std::ifstream ifs( sFile );    
  std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  return str;
}

bool ConfigSettings::SetTranslucency( const std::string & sXml )
{
  try
  {
    if( DatabaseGateway::IsEnabled )
      DatabaseGateway::SetTranslucency( gcnew System::String( sXml.c_str() ));
    else
    {
      std::string sFile( ss( VCSettingsSection::CommonApplicationData ) + "TranslucencySettings.xml" );
      std::ofstream ofs( sFile, std::ios::out );
      ofs << sXml;

      ofs.close();
    }

    return true;
  }
  catch( ... )
  {
    return false;
  }
}