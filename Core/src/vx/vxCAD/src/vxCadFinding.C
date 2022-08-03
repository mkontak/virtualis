// $Id: vxCadFinding.C,v 1.13 2007/03/12 19:43:27 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A finding for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadFinding.h"
#include "vxCadDimensions.h"


// defines
#define FILE_REVISION "$Revision: 1.13 $"

using namespace vxCAD;

/**
 * Constructor
 */
vxCadFinding::vxCadFinding():
m_uSegment( 0 ),
m_centroidMM( 0, 0, 0 ),
m_normalMM( 1, 0, 0 )
{

  // create uuid for the id
  UUID uuid;
  UuidCreate( &uuid );
  
  // convert the uuid to a string
  uint1 * pUUID;  
  UuidToString( &uuid, &pUUID );

  // store the uuid as the id
  m_sId = ( char * )pUUID;

  // free the temporary string
  RpcStringFree( &pUUID );

} // vxCadFinding()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadFinding::vxCadFinding( const vxCadFinding & other ) :
m_sId( other.m_sId ),
m_uSegment( other.m_uSegment ),
m_centroidMM( other.m_centroidMM ),
m_normalMM( other.m_normalMM ),
m_surfaceROI( other.m_surfaceROI ),
m_volumeROI( other.m_volumeROI ),
m_dimensions( other.m_dimensions ),
m_vendorSpecificVariablesMap( other.m_vendorSpecificVariablesMap )
{
} // vxCadFinding( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadFinding & vxCadFinding::operator =( const vxCadFinding & other )
{
  m_sId = other.m_sId;
  m_uSegment = other.m_uSegment;
  m_centroidMM = other.m_centroidMM;
  m_normalMM = other.m_normalMM;
  m_surfaceROI = other.m_surfaceROI;
  m_volumeROI = other.m_volumeROI;
  m_dimensions = other.m_dimensions;
  m_vendorSpecificVariablesMap = other.m_vendorSpecificVariablesMap;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadFinding::ToXml( vxDOMElement & element ) const
{
  element.SetAttribute( "version", "1.1" );
  element.SetAttribute( "GUID", m_sId );
  element.SetAttribute( "segment", ToAscii( int4( m_uSegment ) ) );

  // write centroid
  vxDOMElement centroidElement = element.GetOwnerDocument().CreateElement( "centroid" );
  m_centroidMM.ToXml( centroidElement );
  element.AppendChild( centroidElement );

  // write normal
  vxDOMElement normalElement = element.GetOwnerDocument().CreateElement( "normal" );
  m_normalMM.ToXml( normalElement );
  element.AppendChild( normalElement );

  // write surface ROI
  vxDOMElement surfaceElement = element.GetOwnerDocument().CreateElement( "surface_ROI" );
  m_surfaceROI.ToXml( surfaceElement );
  element.AppendChild( surfaceElement );

  // write volume ROI
  vxDOMElement volumeElement = element.GetOwnerDocument().CreateElement( "volume_ROI" );
  m_volumeROI.ToXml( volumeElement );
  element.AppendChild( volumeElement );

  // write dimensions
  vxDOMElement dimensionsElement = element.GetOwnerDocument().CreateElement( "dimensions" );
  m_dimensions.ToXml( dimensionsElement );
  element.AppendChild( dimensionsElement );

  // write vendor-specific variables map
  vxDOMElement variablesMapElement = element.GetOwnerDocument().CreateElement( "vendor_specific_variables" );
  variablesMapElement.SetAttribute( "version", "1.0" );
  for ( VendorSpecificVariablesMap::const_iterator iter = m_vendorSpecificVariablesMap.begin();
        iter != m_vendorSpecificVariablesMap.end();
        ++iter )
  {
    vxDOMElement oneVariableElement = variablesMapElement.GetOwnerDocument().CreateElement( "variable" );
    oneVariableElement.SetAttribute( "key", iter->first );
    oneVariableElement.SetAttribute( "value", iter->second );
    variablesMapElement.AppendChild( oneVariableElement );
  }
  element.AppendChild( variablesMapElement );

} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadFinding vxCadFinding::FromXml( vxDOMElement & element )
{
  vxCadVector centroid, normal;
  vxCadROI surface, volume;
  vxCadDimensions dimensions;
  unsigned char segment = 0;
  VendorSpecificVariablesMap variables;
  float4 fFilterSetting = 0;
  std::string sId;

  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( ! ( sVersion == "1.0" || sVersion == "1.1" ) )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadFinding XML version: " + sVersion, "vxCadFinding", "FromXml" ) );

    // read ID
    sId = element.GetAttribute( "GUID" );

    // read segment
    std::string sSegment = element.GetAttribute( "segment" );
    segment = atoi( sSegment.c_str() );

    // read filter setting
    std::string sFilterSetting = element.GetAttribute( "filter_setting" );
    fFilterSetting = float4( atof( sFilterSetting.c_str() ) );

    // read centroid
    vxDOMNodeList centroidNodeList = element.GetElementsByTagName( "centroid" );
    if ( centroidNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing centroid node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
    centroid = vxCadVector::FromXml( centroidNodeList.Item( 0 ).ToElement() );

    // read normal
    vxDOMNodeList normalNodeList = element.GetElementsByTagName( "normal" );
    if ( normalNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing normal node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
    normal = vxCadVector::FromXml( normalNodeList.Item( 0 ).ToElement() );

    // read surface ROI
    vxDOMNodeList surfaceNodeList = element.GetElementsByTagName( "surface_ROI" );
    if ( surfaceNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing surface node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
    surface = vxCadROI::FromXml( surfaceNodeList.Item( 0 ).ToElement() );

    // read volume ROI
    vxDOMNodeList volumeNodeList = element.GetElementsByTagName( "volume_ROI" );
    if ( volumeNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing volume node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
    volume = vxCadROI::FromXml( volumeNodeList.Item( 0 ).ToElement() );

    // read dimensions
    vxDOMNodeList dimensionsNodeList = element.GetElementsByTagName( "dimensions" );
    if ( dimensionsNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing dimensions node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
    dimensions = vxCadDimensions::FromXml( dimensionsNodeList.Item( 0 ).ToElement() );

    // read vendor-specific variables map
    vxDOMNodeList allVariablesNodeList = element.GetElementsByTagName( "vendor_specific_variables" );
    if ( sVersion == "1.1" )
    {
      if ( allVariablesNodeList.GetLength() != 1 )
        throw ex::IllegalArgumentException( LogRec( "Missing variables node: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
      vxDOMElement allVariablesElement = allVariablesNodeList.Item( 0 ).ToElement();
      vxDOMNodeList variablesNodeList = allVariablesElement.GetElementsByTagName( "variable" );
      for ( int i=0; i<variablesNodeList.GetLength(); i++ )
      {
        vxDOMElement variableElement = variablesNodeList.Item( i ).ToElement();
        std::string sKey = variableElement.GetAttribute( "key" );
        std::string sValue = variableElement.GetAttribute( "value" );
        variables[ sKey ] = sValue;
      }
    }

  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadFinding", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadFinding: '" + element.GetNodeValue() + "'", "vxCadFinding", "FromXml" ) );
  }

  // create the object
  vxCadFinding finding;
  finding.SetId( sId );
  finding.SetCentroidMM( centroid );
  finding.SetNormalMM( normal );
  finding.SetSegment( segment );
  finding.SetVendorSpecificVariablesMap( variables );
  finding.SetSurfaceROI( surface );
  finding.SetVolumeROI( volume );
  finding.SetDimensions( dimensions );
  return finding;

} // FromXml( xml )


// undefines
#undef FILE_REVISION


// $Log: vxCadFinding.C,v $
// Revision 1.13  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.12  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.11  2007/02/02 21:57:50  jmeade
// variable type for vendor map.
//
// Revision 1.10  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.9  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.8  2006/01/27 20:36:14  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.7.2.4  2007/02/12 20:48:14  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.7.2.3.2.1  2007/01/18 22:05:02  jmeade
// reflecting changes to medisight cad.
//
// Revision 1.7.2.3  2006/03/22 20:42:49  frank
// moved the skeleton index to another data structure in the session
//
// Revision 1.7.2.2  2005/12/07 20:39:06  frank
// updated vendor-specific variable storage
//
// Revision 1.7.2.1  2005/12/07 17:20:44  frank
// added filter setting
//
// Revision 1.7  2005/11/03 21:25:51  frank
// properly set the dimensions
//
// Revision 1.6  2005/09/28 16:59:19  vxconfig
// name change: element -> index
//
// Revision 1.5  2005/09/23 12:02:41  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.4  2005/09/23 03:20:23  vxconfig
// working on display of cad findings
//
// Revision 1.3  2005/09/22 17:00:34  jmeade
// store cad finding locations along centerline.
//
// Revision 1.2  2005/09/16 14:15:59  vxconfig
// added to/from file, added clear/remove voxel/finding
//
// Revision 1.1  2005/09/15 16:38:50  geconomos
// moved from vxColon project
//
// Revision 1.5  2005/09/15 12:50:43  vxconfig
// fixed round trip XML support
//
// Revision 1.4  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.3  2005/09/14 19:42:52  vxconfig
// finishing up serialization
//
// Revision 1.2  2005/09/14 14:27:31  geconomos
// assigned a uuid to the id property in constructor
//
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadFinding.C,v 1.13 2007/03/12 19:43:27 jmeade Exp $
// $Id: vxCadFinding.C,v 1.13 2007/03/12 19:43:27 jmeade Exp $
