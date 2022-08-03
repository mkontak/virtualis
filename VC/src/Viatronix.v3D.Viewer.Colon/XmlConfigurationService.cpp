// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: XmlConfigurationService.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net


#include "StdAfx.h"
#include "XmlConfigurationService.h"
#include "VCSettings.h"
#include "Exception.h"


/**
 * destructor
 */
XmlConfigurationService::~XmlConfigurationService()
{
} // ~XmlConfigurationService()


/** 
  * get Profile
  *
  * @param     sProfile            Profile name 
  * @return    xml
  */
std::string XmlConfigurationService::GetConfiguration( const std::string & sContextId,const std::string & sConfigName )
{
  throw ex::VException( LogRec( "Not implemented.", "XmlConfigurationService", "GetConfiguration" ));
} // GetConfiguration( sConfigName )


/** 
  * get Profile
  *
  * @param     sProfile            Profile name 
  * @return    xml
  */
std::string XmlConfigurationService::GetDefaultConfiguration( const std::string & sContextId,const std::string & sConfigName )
{
  // extract settings from Profile file
  const std::string & sPath = VCSettings::GetInstance().GetSettingsPath();
  vxDOMDocument doc( vxDOMDocument::FromFile( sPath ));
  vxDOMNodeList rootNodeList( doc.GetDocumentElement().GetElementsByTagName( sConfigName ));
  if( rootNodeList.GetLength() == 1 )
  {
    vxDOMElement rootNode = rootNodeList.Item( 0 ).ToElement();
    return rootNode.ToString();
  }

  return "";
} // GetDefaultProfile( sConfigName )


/**
  * Sets the Profile
  *
  * @param       sXMl                    xml
  */
void XmlConfigurationService::SetConfiguration( const std::string & sContextId,const std::string & sXml )
{
  throw ex::VException( LogRec( "Not implemented.", "XmlConfigurationService", "SetConfiguration" ));
} // SetConfiguration( sXml )


/**
  * Deletes the Profile
  *
  * @param     sConfigName               Profile name
  */
void XmlConfigurationService::DeleteConfiguration( const std::string & sContextId,const std::string & sConfigName )
{
  throw ex::VException( LogRec( "Not implemented.", "XmlConfigurationService", "DeleteConfiguration" ));
} // DeleteConfiguration( sConfigName )


