// $Id: vxCadROI.C,v 1.6.2.2 2008/01/18 18:25:23 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

using namespace vxCAD;

/**
 * A region of interest for use in the CAD interface
 */


// includes
#include "StdAfx.h"
#include "vxCadROI.h"
#include "Base64.h"
#include "vxBuffer.h"


// defines
#define FILE_REVISION "$Revision: 1.6.2.2 $"

// namespaces
using namespace vxCAD;

/**
 * Default constructor
 */
vxCadROI::vxCadROI() :
m_fAverageHU( 0 ),
m_fStandardDeviationHU( 0 ),
m_uLabelIndex(0)
{
  m_fVolumeValue = 0.0F;
} // vxCadROI()


/**
 * Copy constructor
 *
 * @param other  the other object
 */
vxCadROI::vxCadROI( const vxCadROI & other ) :
m_fAverageHU( other.m_fAverageHU ),
m_fStandardDeviationHU( other.m_fStandardDeviationHU ),
m_voxels( other.m_voxels ),
m_fVolumeValue( other.m_fVolumeValue ),
m_uLabelIndex( other.m_uLabelIndex )
{
} // vxCadVector( other )


/**
 * Assignment operator
 *
 * @param other  the other object
 *
 * @return itself
 */
vxCadROI & vxCadROI::operator =( const vxCadROI & other )
{
  m_fAverageHU = other.m_fAverageHU;
  m_fStandardDeviationHU = other.m_fStandardDeviationHU;
  m_voxels = other.m_voxels;
  m_fVolumeValue = other.m_fVolumeValue;
  m_uLabelIndex = other.m_uLabelIndex;
  return * this;
} // operator =( other )


/**
 * add a voxel
 *
 * @param voxel   the voxel to add
 */
void vxCadROI::AddVoxel( const vxCadVoxelPosition & voxel )
{
  m_voxels.push_back( voxel );
} // AddVoxel( voxel )


/**
 * Remove a voxel
 *
 * @param index the index of the voxel to remove
 */
void vxCadROI::RemoveVoxel( const int4 index )
{
  std::vector< vxCadVoxelPosition >::iterator iter = m_voxels.begin();
  std::advance( iter, index );
  m_voxels.erase( iter );
} // RemoveVoxel( index )


/**
 * Serialize object to XML
 */
void vxCadROI::ToXml( vxDOMElement & element ) const
{
  element.SetAttribute( "version", "1.1" );
  element.SetAttribute( "units", "voxels" );
  element.SetAttribute( "average_HU", ToAscii( m_fAverageHU ) );
  element.SetAttribute( "stddev_HU", ToAscii( m_fStandardDeviationHU ) );
  element.SetAttribute( "volumeValue", ToAscii( m_fVolumeValue ) );
  element.SetAttribute( "labelIndex", ToAscii(m_uLabelIndex) );

  // Grab a pointer to the underyling memory of the vector 
  //
  // NOTE: This assumption is based on the following: 
  //        (1) A vector's underlying memory is contiguous 
  //        (2) vxCadVoxelPosition doesn't contain a vtable
	uint1 * pVoxelData;

	if (m_voxels.size() < 1)
	{ // This must be handled since the FromXml() assumes the field exist. Otherwise, crash happens.
		vxCadVoxelPosition vv(vxCadVoxelPosition(1, 1, 1));
		pVoxelData = reinterpret_cast<uint1 *>(const_cast<vxCadVoxelPosition *>(&vv));
	}
	else
	{
	  pVoxelData = reinterpret_cast< uint1 *>( const_cast< vxCadVoxelPosition * >( &m_voxels[ 0 ] ) );
	}

  // encode the memory to base64
  std::string sEncoded = Base64::Encode( pVoxelData, (uint4)( sizeof( vxCadVoxelPosition ) * m_voxels.size() ))->ToString();

  // add the encoded string as it's own element
  vxDOMElement voxelsElement = element.GetOwnerDocument().CreateElement( "voxels" );
  voxelsElement.SetNodeValue( sEncoded );
  element.AppendChild( voxelsElement );
 
} // ToXml()


/**
 * Deserialize object from XML
 *
 * @param xml the serialized object in XML format
 *
 * @return the object
 */
vxCadROI vxCadROI::FromXml( vxDOMElement & element )
{
  float fAverage( 0.0F );
  float fStdDev( 0.0F );
  float fVolume( 0.0F );
  uint2 uLabelIndex(0);
  
  std::vector< vxCadVoxelPosition > voxels;
  std::string sVersion;
  try
  {
    // check version
    sVersion = element.GetAttribute( "version" );
    if ( sVersion != "1.0" && sVersion != "1.1" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadROI XML version: " + sVersion, "vxCadROI", "FromXml" ) );

    // read units
    std::string sUnits = element.GetAttribute( "units" );
    if ( sUnits != "voxels" )
      throw ex::IllegalArgumentException( LogRec( "Unsupported vxCadROI units: " + sUnits, "vxCadROI", "FromXml" ) );

    // read average
    std::string sAverage = element.GetAttribute( "average_HU" );
    fAverage = static_cast< float4 >( atof( sAverage.c_str() ) );

    // read standard deviation
    std::string sStdDev = element.GetAttribute( "stddev_HU" );
    fStdDev = static_cast< float4 >( atof( sStdDev.c_str() ) );

    // read volume value
    std::string sVolumeValue = element.GetAttribute( "volumeValue" );
    fVolume = static_cast< float4 >( atof( sVolumeValue.c_str() ) );

    // read label index
    std::string sLabelIndex = element.GetAttribute( "labelIndex" );
    uLabelIndex =  atoi( sLabelIndex.c_str() );

    // read voxels
    if( sVersion == "1.0" ) // old slow method
    {
      vxDOMNodeList voxelNodeList = element.GetElementsByTagName( "voxel" );
      for ( int i(0); i<voxelNodeList.GetLength(); ++i )
        voxels.push_back( vxCadVoxelPosition::FromXml( voxelNodeList.Item( i ).ToElement() ) );
    }
    else if( sVersion == "1.1" )
    {
      // grab voxels element (it is required to be there)
      vxDOMNodeList voxelsNodeList = element.GetElementsByTagName ( "voxels" );

      // check that the encoded string has data in it
      std::string sEncoded = voxelsNodeList.Item( 0 ).GetNodeValue();
      if( !sEncoded.empty() )
      {
        // decode the string
        std::string sDecoded = Base64::Decode( sEncoded );

        // determine the number of voxels bases on the size of the decoded string
        const uint4 uNumberOfVoxels = static_cast< uint4 >( sDecoded.size() ) / sizeof( vxCadVoxelPosition );

        // get the vector ready for the copy
        voxels.resize( uNumberOfVoxels );

        // copy of the voxel data
        memcpy( &voxels[0], sDecoded.c_str(), sDecoded.size() );
      }
    } // else if( sVersion == "1.1" )

  }
  catch ( ex::VException & error )
  {
    LogRec( error.GetLogRecord().GetTextMessage(), "vxCadROI", "FromXml" );
    throw;
  }
  catch ( ... )
  {
    throw ex::IllegalArgumentException( LogRec( "Unable to read vxCadROI: '" + element.GetNodeValue() + "'", "vxCadROI", "FromXml" ) );
  }

  // create the ROI
  vxCadROI roi;
  roi.SetAverageHU( fAverage );
  roi.SetStandardDeviationHU( fStdDev );
  roi.SetVolumeValue( fVolume );
  roi.SetLabelIndex( uLabelIndex );

  if ( sVersion == "1.0" )
  {
    for ( uint4 i(0); i<voxels.size(); ++i )
      roi.AddVoxel( voxels[ i ] );
  }
  else if ( sVersion == "1.1" )
  {
    // TODO: make this more efficent by getting rid of this copy
    roi.m_voxels = voxels;
  }
  return roi;
} // FromXml( xml )


// undefines
#undef FILE_REVISION


// $Log: vxCadROI.C,v $
// Revision 1.6.2.2  2008/01/18 18:25:23  geconomos
// added base64 encoding for voxel positions
//
// Revision 1.6.2.1  2008/01/16 22:04:46  dongqing
// add m_uLabelIndex for the ROI
//
// Revision 1.6  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.5  2007/02/07 22:21:34  dongqing
// move the ROI voxel ToXML after all others
//
// Revision 1.4  2006/09/14 17:46:43  dongqing
// add volume value Set/Get
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
// Revision 1.4  2005/09/15 12:50:43  vxconfig
// fixed round trip XML support
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/src/vxCadROI.C,v 1.6.2.2 2008/01/18 18:25:23 geconomos Exp $
// $Id: vxCadROI.C,v 1.6.2.2 2008/01/18 18:25:23 geconomos Exp $
