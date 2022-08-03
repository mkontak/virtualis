// $Id: vxCadVector.C,v 1.7 2007/03/12 19:43:27 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A simple vector for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadVector.h"


// defines
#define FILE_REVISION "$Revision: 1.7 $"

// namespaces

using namespace vxCAD;


/**
 * Default Constructor
 */
vxCadVector::vxCadVector() :
m_eUnits( MILLIMETER )
{
} // vxCadVector( fX, fY, fZ )


/**
 * Constructor
 *
 * @param fX  the x component
 * @param fY  the y component
 * @param fZ  the z component
 */
vxCadVector::vxCadVector( const float4 fX, const float4 fY, const float4 fZ ) :
m_vVector( fX, fY, fZ ),
m_eUnits( MILLIMETER )
{
} // vxCadVector( fX, fY, fZ )


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadVector::vxCadVector( const vxCadVector & other ) :
m_vVector( other.m_vVector),
m_eUnits( other.m_eUnits )
{
} // vxCadVector( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadVector & vxCadVector::operator =( const vxCadVector & other )
{
  m_vVector = other.m_vVector;
  m_eUnits = other.m_eUnits;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadVector::ToXml( vxDOMElement & element ) const
{
  switch ( m_eUnits )
  {
  case VOXELS:
    element.SetAttribute( "units", "voxels" );
    break;
  case MILLIMETER:
    element.SetAttribute( "units", "millimeters" );
    break;
  default:
    throw ex::IllegalArgumentException( LogRec( "Unsupported units: " + m_eUnits, "vxCadVector", "ToXml" ) );
    break;
  }
  element.SetAttribute( "x", ToAscii( m_vVector.m_x ) );
  element.SetAttribute( "y", ToAscii( m_vVector.m_y ) );
  element.SetAttribute( "z", ToAscii( m_vVector.m_z ) );
  element.SetAttribute( "version", "1.0" );
} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadVector vxCadVector::FromXml( vxDOMElement & element )
{
  float4 fX(0);
  float4 fY(0);
  float4 fZ(0);
  enum Units eUnits( MILLIMETER );
  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( sVersion != "1.0" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadVector XML version: " + sVersion, "vxCadVector", "FromXml" ) );

    // check units
    std::string sUnits = element.GetAttribute( "units" );
    if ( sUnits == "voxels" )
      eUnits = VOXELS;
    else if ( sUnits == "millimeters" )
      eUnits = MILLIMETER;
    else
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadVector units: " + sUnits, "vxCadVector", "FromXml" ) );

    // read components
    fX = static_cast< float4 >( atof( element.GetAttribute( "x" ).c_str() ) );
    fY = static_cast< float4 >( atof( element.GetAttribute( "y" ).c_str() ) );
    fZ = static_cast< float4 >( atof( element.GetAttribute( "z" ).c_str() ) );
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadVector", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadVector from string: '" + element.GetNodeValue() + "'", "vxCadVector", "FromXml" ) );
  }

  vxCadVector vector( fX, fY, fZ );
  vector.SetUnits( eUnits );
  return vector;
} // FromXml( xml )

  
// undefines
#undef FILE_REVISION


// $Log: vxCadVector.C,v $
// Revision 1.7  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.6  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.5  2005/10/03 17:18:18  vxconfig
// fixed reading units
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
// Revision 1.4  2005/09/14 19:42:52  vxconfig
// finishing up serialization
//
// Revision 1.3  2005/09/14 18:21:40  vxconfig
// working on serialization, added some more constructors
//
// Revision 1.2  2005/09/14 14:28:44  geconomos
// added deafult constructor
//
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadVector.C,v 1.7 2007/03/12 19:43:27 jmeade Exp $
// $Id: vxCadVector.C,v 1.7 2007/03/12 19:43:27 jmeade Exp $
