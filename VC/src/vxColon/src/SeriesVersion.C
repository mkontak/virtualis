// $Id: SeriesVersion.C,v 1.4 2006/10/03 20:02:03 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * Handles the version of a series stored in an XML file
 */


// includes
#include "StdAfx.h"
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "SeriesVersion.h"
#include "FilePtrFactory.h"
#include "FilePtr.h"

// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Default constructor
 */
SeriesVersion::SeriesVersion() :
m_sVersion( "-1.0" )
{
} // SeriesVersion()


/**
 * Constructor
 */
SeriesVersion::SeriesVersion( const std::string & sVersion ) :
m_sVersion( sVersion )
{
} // SeriesVersion()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
SeriesVersion::SeriesVersion( const SeriesVersion & other ) :
m_sVersion( other.m_sVersion )
{
} // SeriesVersion( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
SeriesVersion & SeriesVersion::operator =( const SeriesVersion & other )
{
  m_sVersion = other.m_sVersion;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void SeriesVersion::ToXml( vxDOMElement & element ) const
{

  element.SetAttribute( "version", m_sVersion );

} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
SeriesVersion SeriesVersion::FromXml( vxDOMElement & element )
{

  std::string sVersion( "" );

  try
  {
    // retrieve version
    sVersion = element.GetAttribute( "version" );
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "SeriesVersion", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read SeriesVersion: '" + element.GetNodeValue() + "'", "SeriesVersion", "FromXml" ) );
  }

  // create the findings
  SeriesVersion result( sVersion );
  return result;

} // FromXml( xml )


/**
 * Serialize object to disk
 *
 * @param sFilePath the input file path
 */
void SeriesVersion::ToFile( const std::string & sFilePath ) const
{
  // create the document
  vxDOMDocument document( "unused" );

  // write the element data
  vxDOMElement element = document.GetDocumentElement();
  this->ToXml( element );
  element.SetTagName( "SeriesVersion" );

  // write to disk
  document.Save( sFilePath );
} // ToFile( sFilePath )


/**
 * Deserialize object from disk
 *
 * @param sFilePath the output file path
 */
SeriesVersion SeriesVersion::FromFile( const std::string & sFilePath )
{
  try
  { // read the document


    std::shared_ptr<io::FilePtr> filePtr = io::FilePtrFactory::CreateFilePtr(sFilePath);

    filePtr->Open(GENERIC_READ, OPEN_EXISTING);


    const uint4 iSize(filePtr->Size());       // Actual size of the file
    const uint4 iBufferSize(iSize + 1);       // Add one to the buffer for a NULL terminator so that it can be converted to a string

    // Allocate and initialize buffer
    std::shared_ptr<char> buffer =  std::shared_ptr<char>(new char [iBufferSize],  [] ( char * value ) { delete [] value; } );      
    RtlZeroMemory(buffer.get(),iBufferSize);

    // Read the file
    uint4 uBytesRead = filePtr->Read(buffer.get(), iSize);

    // Load the xml
    vxDOMDocument document = vxDOMDocument::FromXml(buffer.get());

    
    //vxDOMDocument document = vxDOMDocument::FromFile( sFilePath );
	
    // return the findings
    return SeriesVersion::FromXml( document.GetDocumentElement() );
  }
  catch( ... )
  {
    LogErr("Fail to read from the file", "FromFile", "SeriesVersion");
    return SeriesVersion("-1.0");
  }
} // FromFile( sFilePath )

// undefines
#undef FILE_REVISION


// $Log: SeriesVersion.C,v $
// Revision 1.4  2006/10/03 20:02:03  jmeade
// return default instance on error in FromFile() method.
//
// Revision 1.3  2006/09/26 19:33:42  frank
// added default constructor
//
// Revision 1.2  2006/06/22 13:56:57  dongqing
// add try-catch for file read since the read used 3rd party codes
//
// Revision 1.1  2006/06/07 14:00:10  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/SeriesVersion.C,v 1.4 2006/10/03 20:02:03 jmeade Exp $
// $Id: SeriesVersion.C,v 1.4 2006/10/03 20:02:03 jmeade Exp $
