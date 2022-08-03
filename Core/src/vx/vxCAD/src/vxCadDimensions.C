// $Id: vxCadDimensions.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A set of three orthogonal dimensions for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadDimensions.h"



// defines
#define FILE_REVISION "$Revision: 1.4 $"


// namespaces
using namespace vxCAD;


/**
 * Default constructor
 */
vxCadDimensions::vxCadDimensions() :
m_largest( vxCadDimension( vxCadVector( 0, 0, 0 ), vxCadVector( 0, 0, 0 ) ) ),
m_perpendicular( vxCadDimension( vxCadVector( 0, 0, 0 ), vxCadVector( 0, 0, 0 ) ) ),
m_depth( vxCadDimension( vxCadVector( 0, 0, 0 ), vxCadVector( 0, 0, 0 ) ) )
{
} // vxCadDimensions()


/**
 * Constructor
 *
 * @param largest         the largest dimension
 * @param perpendicular   the perpendicular dimension
 * @param depth           the depth dimension
 */
vxCadDimensions::vxCadDimensions( const vxCadDimension & largest, const vxCadDimension & perpendicular, const vxCadDimension & depth ) :
m_largest( largest ),
m_perpendicular( perpendicular ),
m_depth( depth )
{
} // vxCadDimensions( largest, perpendicular, depth )


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadDimensions::vxCadDimensions( const vxCadDimensions & other ) :
m_largest( other.m_largest ),
m_perpendicular( other.m_perpendicular ),
m_depth( other.m_depth )
{
} // vxCadDimensions( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadDimensions & vxCadDimensions::operator =( const vxCadDimensions & other )
{
  m_largest = other.m_largest;
  m_perpendicular = other.m_perpendicular;
  m_depth = other.m_depth;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadDimensions::ToXml( vxDOMElement & element ) const
{
  element.SetAttribute( "version", "1.0" );

  // write largest dimension
  vxDOMElement largestElement = element.GetOwnerDocument().CreateElement( "largest" );
  m_largest.ToXml( largestElement );
  element.AppendChild( largestElement );

  // write perpendicular dimension
  vxDOMElement perpendicularElement = element.GetOwnerDocument().CreateElement( "perpendicular" );
  m_perpendicular.ToXml( perpendicularElement );
  element.AppendChild( perpendicularElement );

  // write depth dimension
  vxDOMElement depthElement = element.GetOwnerDocument().CreateElement( "depth" );
  m_depth.ToXml( depthElement );
  element.AppendChild( depthElement );

} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadDimensions vxCadDimensions::FromXml( vxDOMElement & element )
{
  vxCadDimension largest, perpendicular, depth;
  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( sVersion != "1.0" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadDimensions XML version: " + sVersion, "vxCadDimensions", "FromXml" ) );

    // read largest dimension
    vxDOMNodeList largestNodeList = element.GetElementsByTagName( "largest" );
    if ( largestNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing largest node: '" + element.GetNodeValue() + "'", "vxCadDimensions", "FromXml" ) );
    largest = vxCadDimension::FromXml( largestNodeList.Item( 0 ).ToElement() );

    // read perpendicular dimension
    vxDOMNodeList perpendicularNodeList = element.GetElementsByTagName( "perpendicular" );
    if ( perpendicularNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing perpendicular node: '" + element.GetNodeValue() + "'", "vxCadDimensions", "FromXml" ) );
    perpendicular = vxCadDimension::FromXml( perpendicularNodeList.Item( 0 ).ToElement() );

    // read depth dimension
    vxDOMNodeList depthNodeList = element.GetElementsByTagName( "depth" );
    if ( depthNodeList.GetLength() != 1 )
      throw ex::IllegalArgumentException( LogRec( "Missing depth node: '" + element.GetNodeValue() + "'", "vxCadDimensions", "FromXml" ) );
    depth = vxCadDimension::FromXml( depthNodeList.Item( 0 ).ToElement() );
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadDimensions", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadDimensions: '" + element.GetNodeValue() + "'", "vxCadDimensions", "FromXml" ) );
  }

  return vxCadDimensions( largest, perpendicular, depth );
} // FromXml( xml )


// undefines
#undef FILE_REVISION


// $Log: vxCadDimensions.C,v $
// Revision 1.4  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.3  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.2  2005/11/03 21:25:36  frank
// properly initialized some vectors
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
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadDimensions.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
// $Id: vxCadDimensions.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
