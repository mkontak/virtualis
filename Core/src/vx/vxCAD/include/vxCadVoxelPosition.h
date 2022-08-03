// $Id: vxCadVoxelPosition.h,v 1.4.2.1 2009/07/30 19:26:28 kchalupa Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once



//fwd declaration
class vxDOMElement;


namespace vxCAD
{
  /**
   * A simple voxel position for use in the CAD interface
   */
  class vxCadVoxelPosition
  {
  // construction
  public:

    /// default constructor
    vxCadVoxelPosition();

    /// constructor (supplied in voxel coordinates)
    vxCadVoxelPosition( const int4 iX, const int4 iY, const int4 iZ );

    /// copy constructor
    vxCadVoxelPosition( const vxCadVoxelPosition & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadVoxelPosition FromXml( vxDOMElement & element );

    /// get the X component
    const int4 & GetX() const { return m_iX; }

    /// set the X component
    void SetX( const int4 iX ) { m_iX = iX; }

    /// get the Y component
    const int4 & GetY() const { return m_iY; }

    /// set the Y component
    void SetY( const int4 iY ) { m_iY = iY; }

    /// get the Z component
    const int4 & GetZ() const { return m_iZ; }

    /// set the Z component
    void SetZ( const int4 iZ ) { m_iZ = iZ; }

  // operators
  public:

    /// assignment operator
    vxCadVoxelPosition & operator =( const vxCadVoxelPosition & other );

  // member variables
  private:

    /// the x component of the position
    int4 m_iX;

    /// the y component of the position
    int4 m_iY;

    /// the z component of the position
    int4 m_iZ;

  }; // class vxCadVoxelPosition

} // namespace vxCAD

// $Log: vxCadVoxelPosition.h,v $
// Revision 1.4.2.1  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.4  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.3  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.2  2005/09/16 14:15:59  vxconfig
// added to/from file, added clear/remove voxel/finding
//
// Revision 1.1  2005/09/15 16:38:41  geconomos
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadVoxelPosition.h,v 1.4.2.1 2009/07/30 19:26:28 kchalupa Exp $
// $Id: vxCadVoxelPosition.h,v 1.4.2.1 2009/07/30 19:26:28 kchalupa Exp $
