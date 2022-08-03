// $Id: vxCadDimension.C,v 1.3.2.1 2009/08/20 22:03:24 dongqing Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A dimension for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadDimension.h"


// defines
#define FILE_REVISION "$Revision: 1.3.2.1 $"


using namespace vxCAD;

/**
 * Default constructor
 */
vxCadDimension::vxCadDimension() :
m_startPoint( vxCadVector( 0, 0, 0 ) ),
m_endPoint( vxCadVector( 0, 0, 0 ) )
{
} // vxCadDimension()


/**
 * Constructor
 *
 * @param startPoint    The start point for linear dimension
 * @param endPoint      The end point for linear dimension
 */
vxCadDimension::vxCadDimension( const vxCadVector & startPoint, const vxCadVector & endPoint ) :
m_startPoint( startPoint ),
m_endPoint( endPoint )
{
} // vxCadDimension( startPoint, endPoint )


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadDimension::vxCadDimension( const vxCadDimension & other ) :
m_startPoint( other.m_startPoint ),
m_endPoint( other.m_endPoint )
{
} // vxCadDimension( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadDimension & vxCadDimension::operator =( const vxCadDimension & other )
{
  m_startPoint = other.m_startPoint;
  m_endPoint = other.m_endPoint;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadDimension::ToXml( vxDOMElement & element ) const
{
  element.SetAttribute( "version", "1.0" );

  vxDOMElement startElement = element.GetOwnerDocument().CreateElement( "start" );
  m_startPoint.ToXml( startElement );
  element.AppendChild( startElement );

  vxDOMElement endElement = element.GetOwnerDocument().CreateElement( "end" );
  m_endPoint.ToXml( endElement );
  element.AppendChild( endElement );

} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadDimension vxCadDimension::FromXml( vxDOMElement & element )
{
  vxCadVector start, end;
  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( sVersion != "1.0" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadDimension XML version: " + sVersion, "vxCadDimension", "FromXml" ) );

    // read start vector
    vxDOMNodeList startNodeList = element.GetElementsByTagName( "start" );
    if ( startNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing start node: '" + element.GetNodeValue() + "'", "vxCadDimension", "FromXml" ) );
    start = vxCadVector::FromXml( startNodeList.Item( 0 ).ToElement() );

    // read end vector
    vxDOMNodeList endNodeList = element.GetElementsByTagName( "end" );
    if ( endNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing end node: '" + element.GetNodeValue() + "'", "vxCadDimension", "FromXml" ) );
    end = vxCadVector::FromXml( endNodeList.Item( 0 ).ToElement() );
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadDimension", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadDimension: '" + element.GetNodeValue() + "'", "vxCadDimension", "FromXml" ) );
  }

  return vxCadDimension( start, end );
} // FromXml( xml )


// undefines
#undef FILE_REVISION


// $Log: vxCadDimension.C,v $
// Revision 1.3.2.1  2009/08/20 22:03:24  dongqing
// code review update
//
// Revision 1.3  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.2  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.1  2005/09/15 16:38:50  geconomos
// moved from vxColon project
//
// Revision 1.3  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.2  2005/09/14 18:21:40  vxconfig
// working on serialization, added some more constructors
//
// Revision 1.1  2005/09/13 17:06:55  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadDimension.C,v 1.3.2.1 2009/08/20 22:03:24 dongqing Exp $
// $Id: vxCadDimension.C,v 1.3.2.1 2009/08/20 22:03:24 dongqing Exp $
