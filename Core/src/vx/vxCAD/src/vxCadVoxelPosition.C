// $Id: vxCadVoxelPosition.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


/**
 * A simple voxel position for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadVoxelPosition.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


// namespaces
using namespace vxCAD;

/**
 * Default constructor
 */
vxCadVoxelPosition::vxCadVoxelPosition() : 
m_iX( 0 ),
m_iY( 0 ),
m_iZ( 0 )
{
} // vxCadVoxelPosition


/**
 * Constructor (supplied in voxel coordinates)
 *
 * @param x   the x component
 * @param y   the y component
 * @param z   the z component
 */
vxCadVoxelPosition::vxCadVoxelPosition( const int4 x, const int4 y, const int4 z ) :
m_iX( x ),
m_iY( y ),
m_iZ( z )
{
} // vxCadVoxelPosition( x, y, z )


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadVoxelPosition::vxCadVoxelPosition( const vxCadVoxelPosition & other ) :
m_iX( other.m_iX ),
m_iY( other.m_iY ),
m_iZ( other.m_iZ )
{
} // vxCadVoxelPosition( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadVoxelPosition & vxCadVoxelPosition::operator =( const vxCadVoxelPosition & other )
{
  m_iX = other.m_iX;
  m_iY = other.m_iY;
  m_iZ = other.m_iZ;
  return * this;
} // operator =( other )


/**
 * Serialize object to XML
 *
 * @return the serialized object in XML format
 */
void vxCadVoxelPosition::ToXml( vxDOMElement & element ) const
{
  element.SetAttribute( "x", ToAscii( m_iX ) );
  element.SetAttribute( "y", ToAscii( m_iY ) );
  element.SetAttribute( "z", ToAscii( m_iZ ) );
  element.SetAttribute( "version", "1.0" );
} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadVoxelPosition vxCadVoxelPosition::FromXml( vxDOMElement & element )
{
  int iX(0);
  int iY(0);
  int iZ(0);
  try
  {
    // check version
    std::string sVersion = element.GetAttribute( "version" );
    if ( sVersion != "1.0" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadVoxelPosition XML version: " + sVersion, "vxCadVoxelPosition", "FromXml" ) );

    // read components
    iX = atoi( element.GetAttribute( "x" ).c_str() );
    iY = atoi( element.GetAttribute( "y" ).c_str() );
    iZ = atoi( element.GetAttribute( "z" ).c_str() );
  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadVoxelPosition", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadVoxelPosition from string: '" + element.GetNodeValue() + "'", "vxCadVoxelPosition", "FromXml" ) );
  }

  return vxCadVoxelPosition( iX, iY, iZ );
} // FromXml( xml )


// undefines
#undef FILE_REVISION

// $Log: vxCadVoxelPosition.C,v $
// Revision 1.4  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.3  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.2  2005/09/16 14:15:59  vxconfig
// added to/from file, added clear/remove voxel/finding
//
// Revision 1.1  2005/09/15 16:38:50  geconomos
// moved from vxColon project
//
// Revision 1.2  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.1  2005/09/14 19:43:48  vxconfig
// finishing up serialization
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadVoxelPosition.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
// $Id: vxCadVoxelPosition.C,v 1.4 2007/03/12 19:43:27 jmeade Exp $
