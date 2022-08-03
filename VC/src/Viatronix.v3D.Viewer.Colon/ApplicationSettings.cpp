// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ApplicationSettings.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "ApplicationSettings.h"
#include "Viatron.h"
#include <algorithm>
#include "ServiceLocator.h"
#include <memory>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <regex>


/**
 * constructor
 *
 * @param     section         section name
 */
ApplicationSettings::ApplicationState::ApplicationState( const std::string & section, bool bDefaultsOnly ) :
  m_section( section ),
  m_bDefaultsOnly( bDefaultsOnly ),
  m_bInitialized( false )
{
	 
} // ApplicationState


/**
 * destructor
 */
ApplicationSettings::ApplicationState::~ApplicationState()
{
  m_settings.clear();
} // ~ApplicationState()


/**
 * Returns the value associated with the specified name
 *
 * @param       name          value's name
 * @exception   XmlException  if the name-value pair doesn't exist
 */
std::string ApplicationSettings::ApplicationState::Get( const std::string & name ) 
{

 SettingsMap::iterator it = m_settings.find(name);

  if ( it == m_settings.end() )
  {
    Load();

    if ( (it = m_settings.find(name)) == m_settings.end() )
      throw ex::IllegalArgumentException( LogRec( "Unknown application setting in " + m_section + " [KEY=" + name + "]", "ApplicationState", "Get" ));

  }

  return it->second;

  //auto range = std::equal_range( m_settings.begin(), m_settings.end(), std::make_pair( name, "" ), SettingsPredicate() );
  //if( range.first == range.second )
  //{
  //  Load();
  //  range = std::equal_range( m_settings.begin(), m_settings.end(), std::make_pair( name, "" ), SettingsPredicate() );
  //  if( range.first == range.second )
  //    throw ex::IllegalArgumentException( LogRec( "Unknown application setting in " + m_section + " [KEY=" + name + "]", "ApplicationState", "Get" ));
  //}

  //return range.first->second;
} // Get( name ) 


/**
 * Attempts to retrieve the value for the specified name
 *
 * @param       name          value's name
 * @param       value         out parameter containing the value for the name if it exists; otherwise the value is unchanged
 * @returns     true if the value exists; otherwise false   
 */
bool ApplicationSettings::ApplicationState::TryGet( const std::string & name, std::string & value )
{
  try
  {
    value = Get( name );
    return true;
  }
  catch( ... )
  {
    return false;
  }
} // TryGet( name, value )


/**
 * Sets the value associated with the name
 *
 * @param     name          value's name
 * @param     value         string value
 */
void ApplicationSettings::ApplicationState::Set( const std::string & name, const std::string & value ) 
{ 
  if( !m_bInitialized )
    Load();
  

  SettingsMap::iterator it = m_settings.find(name);

  if ( it == m_settings.end() )
    m_settings.insert( std::make_pair( name, value ));
  else
    m_settings[name] = value;


  //auto range = std::equal_range( m_settings.begin(), m_settings.end(), std::make_pair( name, "" ), SettingsPredicate() );
  //if( range.first == range.second )
  //{
  //  m_settings.push_back( std::make_pair( name, value ));
  //  std::sort( m_settings.begin(), m_settings.end() );
  //}
  //else
  //  range.first->second = value;
} // Set( name, string, persist ) 


/**
 * Sets the value associated with the name
 *
 * @param     name          value's name
 * @param     value         int value
 */
void ApplicationSettings::ApplicationState::SetInt( const std::string & name, int4 value )
{ 
  std::stringstream stream;
  stream << value;
  Set( name, stream.str() );
} // SetInt( name, int4, persist )


/**
 * Sets the value associated with the name
 *
 * @param     name          value's name
 * @param     value         bool value
 */
void ApplicationSettings::ApplicationState::SetBool( const std::string & name, bool value ) 
{ 
  std::stringstream stream;
  stream << std::boolalpha << value;
  Set( name, stream.str() );
} // SetBool( name, bool )



/**
 * Sets the value associated with the name
 *
 * @param     name          value's name
 * @param     value         float4 value
 * @param     persist       if true save to roaming profile
 */
void ApplicationSettings::ApplicationState::SetFloat( const std::string & name, float4 value ) 
{ 
  std::stringstream stream;
  stream << value;
  Set( name, stream.str() );
} // SetFloat( name, bool )


/**
 * Loads the settings
 */
void ApplicationSettings::ApplicationState::Load()
{
  if( !m_bInitialized )
  {
    std::shared_ptr<IConfigurationService> service = ServiceLocator::GetConfigurationService();

    // ==============================================================================================================
    // Load the custom settings if the the default only flag is false or there is no context id. No context
    // if means that the viewer has not logged in and all that is available are the defaults
    // ==============================================================================================================
    if( !m_bDefaultsOnly && ! ViatronApp::GetViatronApp()->GetContextId().empty() )
      ParseXml( service->GetConfiguration( ViatronApp::GetViatronApp()->GetContextId(), m_section ), m_settings );

    SettingsMap defaults;
    ParseXml( service->GetDefaultConfiguration( ViatronApp::GetViatronApp()->GetContextId(), m_section ), defaults );


    // ==============================
    // Merge missing default settings
    // ==============================
    std::for_each(defaults.begin(), defaults.end(), [&] ( SettingsPair pair ) 
    {
      if ( m_settings.find(pair.first) == m_settings.end() )
          m_settings.insert(pair);
    });
    
    // merge any missing default settings
    //std::remove_copy_if( defaults.begin(), defaults.end(), std::back_inserter( m_settings ), 
    //  [&]( SettingsPair setting ) -> bool { return m_settings.end() != std::find_if( m_settings.begin(), m_settings.end(),
    //    [ setting ]( const SettingsPair & config ) -> bool { return config.first == setting.first; }); });

    // sort collection
    //std::sort( m_settings.begin(), m_settings.end() );

    m_bInitialized = true;
  }
} // Load()


/**
 * Clears the settings
 */
void ApplicationSettings::ApplicationState::Reset()
{
  m_settings.clear();
} // Reset()


/**
 * Saves the settings
 */
void ApplicationSettings::ApplicationState::Save()
{
  static const std::string sVersion( "1.0" );

  vxDOMDocument doc( m_section );
  doc.GetDocumentElement().SetAttribute( "ver", sVersion );
  for( SettingsMap::const_iterator iter = m_settings.cbegin(); iter != m_settings.cend(); ++iter )
  {
    vxDOMElement element = doc.CreateElement( "key" );
    element.SetAttribute( "name", iter->first );
    element.SetAttribute( "value", iter->second );
    doc.GetDocumentElement().AppendChild( element );
  }

  // update the service
  ServiceLocator::GetConfigurationService()->SetConfiguration( ViatronApp::GetViatronApp()->GetContextId(), doc.ToString() );
} // Save()


/**
 * Parses the xml
 *
 * @param         xml             xml
 * @param         settings        settings vector
 */
void ApplicationSettings::ApplicationState::ParseXml( const std::string & xml, SettingsMap & settings )
{
  // xml encodes "CRLF" as "LF" but vxDOM encodes it as (SPACE)LF
  // to properly decode we must replace (SPACE)LF with CRLF otherwise gui will not display newlines correctly
  static std::regex rx( "\x20\x0A" ); // (SPACE)LF

  if( !xml.empty() )
  {
    std::string regexReplacement( "\x0D\x0A" ); // CRLF

    vxDOMDocument doc( vxDOMDocument::FromXml( xml ));
    vxDOMElement rootNode = doc.GetDocumentElement();
    if( rootNode.HasChildNodes() )
    {
      vxDOMNodeList nodes( rootNode.GetElementsByTagName( "key" ));
      for( int i( 0 ); i < nodes.GetLength(); ++i )
      {
        vxDOMElement child( nodes.Item( i ).ToElement() );
        //settings.push_back( std::make_pair( child.GetAttribute( "name" ), std::regex_replace( child.GetAttribute( "value" ), rx, regexReplacement )));
        settings.insert( std::make_pair( child.GetAttribute( "name" ), std::regex_replace( child.GetAttribute( "value" ), rx, regexReplacement )));


      }
    }

    //std::sort( settings.begin(), settings.end() );
  }
} // ParseXml( xml, settings )


/**
 * comparisson fuction for SettingsPair
 */
bool ApplicationSettings::ApplicationState::SettingsPredicate::operator()( const SettingsPair & first, const SettingsPair & second ) const
{
  return first.first < second.first;
} // operator()( first, second )


/**
 * Returns the user settings
 */
ApplicationSettings::ApplicationState & ApplicationSettings::UserSettings()
{
  static ApplicationState settings( CONFIGURATION_USER, false );
  return settings;
} // UserSettings()


/**
 * Creates an application state
 *
 * @param     sSection                application state section
 * @return    application state
 */
ApplicationSettings::ApplicationState ApplicationSettings::Create( const std::string & sSection )
{
  return ApplicationState( sSection, false );
} // Create( sSection )


/**
 * Creates an application state
 *
 * @param     sSection                application state section
 * @return    application state
 */
ApplicationSettings::ApplicationState ApplicationSettings::CreateDefaults( const std::string & sSection )
{
  return ApplicationState( sSection, true );
} // Create( sSection )