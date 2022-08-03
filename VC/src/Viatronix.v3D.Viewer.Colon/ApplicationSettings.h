// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ApplicationSettings.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include <strstream>
#include <vector>
#include "Exception.h"
#include <functional>
#include "ObservableObject.h"
#include "AppSettings.h"


class ApplicationSettings
{
public:

  class ApplicationState
  {
  private:

    /// constructor
    ApplicationState( const std::string & section, bool bDefaultOnly );

  public:

    /// destructor
    ~ApplicationState();

  public:
  
    /// returns the string value for the specified name (throws an exception if it does not exist )
    std::string Get( const std::string & key ); 

    /// try's to get the value of the specified name and return it in the value parameter
    bool TryGet( const std::string & name, std::string & value ); 

    /// sets the value for the specified name
    void Set( const std::string & name, const std::string & value );

    /// sets the value for the specified name
    void SetInt( const std::string & name, int4 valuee );

    /// sets the value for the specified name
    void SetFloat( const std::string & name, float4 value );

    /// sets the value for the specified name
    void SetBool( const std::string & name, bool value );

    /// loads the settings
    void Load();

    /// clears out the application state
    void Reset();

    /// saves the settings
    void Save();

  public:

    /// returns the value for the specified name (throws an exception if it does not exist )
    template <class TValue>
    TValue Get( const std::string & name ) { return TValue( Get<int4>( name )); }

    /// try's to get the value of the specified name and return it in the value parameter
    template <class TValue>
    bool TryGet( const std::string & name, TValue & value ) 
    { 
      int4 temp;
      bool success = TryGet<int4>( name, temp );
      if( success )
        value = TValue( temp );
      return success;
    }
  
    /// returns the integer value for the specified name (throws an exception if it does not exist )
    template<>
    int4 Get<int4>( const std::string & name ) { return std::stoi( Get( name )); }

    /// try's to get the value of the specified name and return it in the value parameter
    template <>
    bool TryGet<int4>( const std::string & name, int4 & value ) 
    { 
      try
      {
        std::string temp;
        bool success = TryGet( name, temp );
        if( success )
          value = std::stoi( temp ); 
        return success;
      }
      catch( ... )
      {
        return false;
      }
    } //TryGet<int4>


    /// returns the integer value for the specified name (throws an exception if it does not exist )
    template<>
    bool Get<bool>( const std::string & name ) 
    {
      std::string temp( Get( name ));
      if( temp == "true" )
        return true; 
      else if( temp == "false" )
        return false;

      throw ex::IllegalArgumentException( LogRec( "Invalid configuration value [" + name + ", " + "]", "ApplicationSettings", "Get<bool>" ));
    } // Get<bool>( name )


    /// try's to get the value of the specified name and return it in the value parameter
    template <>
    bool TryGet<bool>( const std::string & name, bool & value ) 
    { 
      try
      {
        std::string temp;
        bool success = TryGet( name, temp );
        if( success && temp == "true" )
          value = true; 
        else if( success && temp == "false" )
          value = false;
        else
          success = false;
        return success;
      }
      catch( ... )
      {
        return false;
      }
    } // TryGet<bool>( name, value )

  private:

    typedef std::pair< std::string, std::string > SettingsPair;
    //typedef std::vector< SettingsPair > SettingsMap;
    typedef std::map<std::string, std::string> SettingsMap;
		/// parse xml
    static void ParseXml( const std::string & xml, SettingsMap & settings );

  private:

    class SettingsPredicate : std::binary_function< SettingsPair, SettingsPair, bool >
    {
    public:
			/// operator
      bool operator()( const SettingsPair & first, const SettingsPair & second ) const;
    };

  private:

    /// application section name
    std::string m_section;

    /// application settings for the section
    SettingsMap m_settings;

    /// default values only
    bool m_bDefaultsOnly;

    /// values initialized
    bool m_bInitialized;
    
		/// friend class
    friend class ApplicationSettings;

  };


public:

  /// user settings
  static ApplicationState & UserSettings();

  /// creates an application state
  static ApplicationState Create( const std::string & sSection );

  /// creates an application state
  static ApplicationState CreateDefaults( const std::string & sSection );

private:

  /// constructor
  ApplicationSettings();

};

