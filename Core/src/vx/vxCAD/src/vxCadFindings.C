// $Id: vxCadFindings.C,v 1.18 2007/03/12 19:43:27 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A set of findings for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadFindings.h"
#include <algorithm>
#include "FilePtrFactory.h"
#include "FilePtr.h"

// defines
#define FILE_REVISION "$Revision: 1.18 $"

using namespace vxCAD;

/**
 * Constructor
 */
vxCadFindings::vxCadFindings() :
m_sCadVendor( "unspecified" ), m_iCadThresholdValue(-1)
{
  m_iSelectedIndex = -1;
} // vxCadFindings()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadFindings::vxCadFindings( const vxCadFindings & other ) :
m_sCadVendor( other.m_sCadVendor ),
m_sCadApiVersion( other.m_sCadApiVersion ),
m_findings( other.m_findings ),
m_vendorSpecificVariablesMap( other.m_vendorSpecificVariablesMap ),
m_iCadThresholdValue( other.m_iCadThresholdValue )
{
} // vxCadFindings( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadFindings & vxCadFindings::operator =( const vxCadFindings & other )
{
  m_sCadVendor = other.m_sCadVendor;
  m_sCadApiVersion = other.m_sCadApiVersion;
  m_findings = other.m_findings;
  m_vendorSpecificVariablesMap = other.m_vendorSpecificVariablesMap;
  m_iCadThresholdValue = other.m_iCadThresholdValue;
  return * this;
} // operator =( other )


/**
 * Add a finding
 *
 * @param finding a new finding to add
 */
void vxCadFindings::AddFinding( const vxCadFinding & finding )
{
  m_findings.push_back( finding );
} // AddFinding( finding )


/**
 * sort the findings
 *
 * @param fnCompare   Compare function used to sort the findings
 */
void vxCadFindings::SortFindings(PFNFINDINGCOMPARE pFnCompare)
{
  std::sort( m_findings.begin(), m_findings.end(), pFnCompare);
} // SortFindings( function )


/**
 * get an individual finding
 *
 * @param sID   ID of the finding
 * @return      Finding
 */
vxCadFinding & vxCadFindings::GetFinding( const std::string & sID )
{
  try
  {
    for (uint4 i(0); i < m_findings.size(); i++)
    {
      if (m_findings[i].GetId() == sID)
      {
        return m_findings[i];
      }
    }

    throw ex::IllegalArgumentException( LogRec( "Unable to find specified CAD finding in vxCadFindings: '" + sID + "'", "vxCadFindings", "GetFinding" ) );
    static vxCadFinding nullfind;
    return nullfind;
  } // try
  catch ( ... )
  {
    LogWrn( "unhandled exception", "vxCadFindings", "GetFinding" );
    throw;
  }
} // GetFinding( string )


/**
 * Remove a finding
 *
 * @param index the index of the finding to remove
 */
void vxCadFindings::RemoveFinding( const int4 index )
{
  std::vector< vxCadFinding >::iterator iter = m_findings.begin();
  std::advance( iter, index );
  m_findings.erase( iter );
} // RemoveFinding( index )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadFindings::ToXml( vxDOMElement & element ) const
{

  element.SetAttribute( "version", "1.1" );
  element.SetAttribute( "cad_vendor", ToAscii( m_sCadVendor ) );
  element.SetAttribute( "cad_API_version", ToAscii( m_sCadApiVersion ) );

  // write findings
  for ( uint4 i(0); i<m_findings.size(); ++i )
  {
    vxDOMElement findingElement = element.GetOwnerDocument().CreateElement( "finding" );
    m_findings[i].ToXml( findingElement );
    element.AppendChild( findingElement );
  }

  // write vendor-specific variables map
  vxDOMElement variablesMapElement = element.GetOwnerDocument().CreateElement( "vendor_specific_variables" );
  variablesMapElement.SetAttribute( "version", "1.0" );
  for ( std::map< std::string, std::string >::const_iterator iter = m_vendorSpecificVariablesMap.begin();
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
vxCadFindings vxCadFindings::FromXml( vxDOMElement & element )
{

  std::string sCadVendor( "" );
  std::string sCadApiVersion( "" );
  std::vector< vxCadFinding > findings;
  std::map< std::string, std::string > variables;

  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( sVersion == "1.0" )
    {
      // version 1.0 does not have the API version, 1.1 does
      // read CAD vendor
      sCadVendor = element.GetAttribute( "cad_vendor" );
    }
    else if ( sVersion == "1.1" )
    {
      // read CAD vendor
      sCadVendor = element.GetAttribute( "cad_vendor" );

      // read CAD API version
      sCadApiVersion = element.GetAttribute( "cad_API_version" );
    }
    else
    {
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadFindings XML version: " + sVersion, "vxCadFindings", "FromXml" ) );
    }

    // read findings
    vxDOMNodeList findingNodeList = element.GetElementsByTagName( "finding" );
    for ( int4 i(0); i<findingNodeList.GetLength(); ++i )
      findings.push_back( vxCadFinding::FromXml( findingNodeList.Item( i ).ToElement() ) );

    // read vendor-specific variables map
    vxDOMNodeList childNodeList = element.GetChildNodes(); //  
    // Search for "vendor_specific_variables" in the top-level nodes.
    // If we simply use the GetElementsByTagName(x), we get all nodes x found in
    // a recursive search of ALL nodes in this tree, rather than just the
    // top-level ones.
    for ( int4 i=0; i<childNodeList.GetLength(); i++ )
    {
      if ( childNodeList.Item( i ).GetNodeName() == "vendor_specific_variables" )
      {
        vxDOMElement allVariablesElement = childNodeList.Item( i ).ToElement();
        vxDOMNodeList variablesNodeList = allVariablesElement.GetElementsByTagName( "variable" );
        for ( int4 i=0; i<variablesNodeList.GetLength(); i++ )
        {
          vxDOMElement variableElement = variablesNodeList.Item( i ).ToElement();
          std::string sKey = variableElement.GetAttribute( "key" );
          std::string sValue = variableElement.GetAttribute( "value" );
          variables[ sKey ] = sValue;
        }
      }
    }
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadFindings", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadFindings: '" + element.GetNodeValue() + "'", "vxCadFindings", "FromXml" ) );
  }

  // create the findings
  vxCadFindings result;
  result.SetCadVendor( sCadVendor );
  result.SetCadApiVersion( sCadApiVersion );
  result.SetVendorSpecificVariablesMap( variables );
  for ( uint4 i(0); i<findings.size(); ++i )
    result.AddFinding( findings[ i ] );


  //{{ initialize vendor-specific CAD slider
  const std::string sTheSpecialVendor("Medicsight");
  if (sTheSpecialVendor.compare(0, sTheSpecialVendor.length(), result.GetCadVendor(), 0, sTheSpecialVendor.length()) == 0)
  {
    // overwrite CAD slider with CAD vendor default if available
    std::map< std::string, std::string >::const_iterator findingsVarIter = result.GetVendorSpecificVariablesMap().find( "SphericityDefault" );
    if ( findingsVarIter != result.GetVendorSpecificVariablesMap().end() )
    {
      result.m_iCadThresholdValue = atoi( findingsVarIter->second.c_str() );
      LogDbg( "Read slider default from CAD vendor: " + ToAscii( result.m_iCadThresholdValue ), "vxCadFindings", "FromXml" );
    }
    else
    {
      result.m_iCadThresholdValue = -2;
    }
  }
  else
  {
    result.m_iCadThresholdValue = -1;
  }
  //}} initialize the CAD slider


  return result;

} // FromXml( xml )


/**
 * Serialize object to disk
 *
 * @param sFilePath the input file path
 */
void vxCadFindings::ToFile( const std::string & sFilePath ) const
{

  std::shared_ptr<io::FilePtr> filePtr = io::FilePtrFactory::CreateFilePtr(sFilePath);

  filePtr->Open( GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS );


  // create the document
  vxDOMDocument document( "unused" );

  // write the element data
  vxDOMElement element = document.GetDocumentElement();
  this->ToXml( element );
  element.SetTagName( "findings" );

  std::string sXml(document.ToString());

  filePtr->Write(sXml.c_str(), (uint4)sXml.size());

  // write to disk
  //document.Save( sFilePath );

} // ToFile( sFilePath )


/**
 * Deserialize object from disk
 *
 * @param sFilePath the output file path
 */
vxCadFindings vxCadFindings::FromFile( const std::string & sFilePath )
{

  std::shared_ptr<io::FilePtr> filePtr = io::FilePtrFactory::CreateFilePtr(sFilePath);

  filePtr->Open( GENERIC_READ, OPEN_EXISTING );

  const uint4 uSize(filePtr->Size());
  const uint4 uBufferSize(uSize + 1);

   std::shared_ptr<char> buffer =  std::shared_ptr<char>(new char [uBufferSize],  [] ( char * value ) { delete [] value; } );      
   RtlZeroMemory(buffer.get(),uBufferSize);

   // Read the file
   filePtr->Read(buffer.get(), uSize);

  // read the document
  vxDOMDocument document = vxDOMDocument::FromXml( buffer.get() );

  // return the findings
  return vxCadFindings::FromXml( document.GetDocumentElement() );

} // FromFile( sFilePath )


/**
 * go through the findings and convert voxel positions to world positions
 *
 * @param voxelUnits  the unit size of a voxel
 */
void vxCadFindings::ResolveUnits( const Triple<float4> & voxelUnits )
{

  for ( uint4 i(0); i<m_findings.size(); i++ )
  {
    vxCadFinding & finding = m_findings[i];
    if ( finding.GetCentroidMM().GetUnits() == vxCadVector::VOXELS )
    {
      vxCadVector centroid = finding.GetCentroidMM();
      centroid.SetVector( centroid.GetVector().GetScaledAnIso( voxelUnits ) );
      centroid.SetUnits( vxCadVector::MILLIMETER );
      finding.SetCentroidMM( centroid );
    }
  }

} // ResolveUnits( voxelUnits )


/**
 *  set the selected index
 *
 *  @param index
 */
void vxCadFindings::SetSelectedIndex( int4 index ) 
{
  if ( index > m_findings.size() )
    m_iSelectedIndex = -1; 
  else
   m_iSelectedIndex = index; 

}//SetSelectedIndex( int4 index ) 


// undefines
#undef FILE_REVISION


// $Log: vxCadFindings.C,v $
// Revision 1.18  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.17  2007/02/27 21:50:17  jmeade
// Issue 5424: ensure cad threshold default is read when necessary.
//
// Revision 1.16  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.15  2006/10/04 23:15:30  jmeade
// code standards.
//
// Revision 1.14  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.13  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.12  2006/01/27 20:36:14  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.11  2005/11/16 05:04:07  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.10.2.5  2006/03/22 20:42:49  frank
// moved the skeleton index to another data structure in the session
//
// Revision 1.10.2.4  2006/01/10 14:14:28  frank
// properly retrieved the findings variables node
//
// Revision 1.10.2.3  2006/01/09 19:43:42  frank
// added variable map to findings object
//
// Revision 1.10.2.2  2006/01/05 18:08:37  geconomos
// Issue# 4588: CAD vendor needs to be displayed on about box
//
// Revision 1.10.2.1  2005/11/15 13:14:05  frank
// additional error handling
//
// Revision 1.10  2005/11/01 09:50:39  frank
// fixed scaling problem for mm centroids
//
// Revision 1.9  2005/10/07 19:41:29  vxconfig
// added api version
//
// Revision 1.8  2005/10/04 16:13:13  jmeade
// set units for cad finding vector regardless of units type.
//
// Revision 1.7  2005/09/30 22:21:54  jmeade
// sorting
//
// Revision 1.6  2005/09/28 16:59:38  vxconfig
// added sorting, needs more work
//
// Revision 1.5  2005/09/28 14:36:42  jmeade
// get finding by id; sort function (shell).
//
// Revision 1.4  2005/09/23 12:02:41  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.3  2005/09/20 19:30:58  vxconfig
// added voxel units on vector
//
// Revision 1.2  2005/09/16 14:15:59  vxconfig
// added to/from file, added clear/remove voxel/finding
//
// Revision 1.1  2005/09/15 16:38:50  geconomos
// moved from vxColon project
//
// Revision 1.3  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.2  2005/09/14 19:42:52  vxconfig
// finishing up serialization
//
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadFindings.C,v 1.18 2007/03/12 19:43:27 jmeade Exp $
// $Id: vxCadFindings.C,v 1.18 2007/03/12 19:43:27 jmeade Exp $
