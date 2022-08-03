// $Id: vxCadROI.h,v 1.6.2.3 2009/07/30 19:26:28 kchalupa Exp $
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


// includes
#include "vxCadVoxelPosition.h"

//fwd declaration
class vxDOMElement;

namespace vxCAD
{

  /**
   * A region of interest for use in the CAD interface
   */
  class vxCadROI
  {
  // construction
  public:

    /// constructor
    vxCadROI();

    /// copy constructor
    vxCadROI( const vxCadROI & other );

    /// assignment operator
    vxCadROI & operator =( const vxCadROI & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadROI FromXml( vxDOMElement & element );

    /// add a voxel
    void AddVoxel( const vxCadVoxelPosition & voxel );

    /// get the number of voxels
    const int4 GetCount() const { return static_cast< int4 > ( m_voxels.size() ); }

    /// get an individual finding
    const vxCadVoxelPosition & GetVoxel( const int4 iIndex ) const { return m_voxels[ iIndex ]; }

    /// remove a voxel
    void RemoveVoxel( const int4 index );

    /// clear all voxels
    void ClearVoxels() { m_voxels.clear(); }

    /// get the average HU value
    const float4 & GetAverageHU() const { return m_fAverageHU; }

    /// set the average HU value
    void SetAverageHU( const float4 averageHU ) { m_fAverageHU = averageHU; }

    /// get the standard deviation (in HU)
    const float4 & GetStandardDeviationHU() const { return m_fStandardDeviationHU; }

    /// set the standard deviation (in HU)
    void SetStandardDeviationHU( const float4 standardDeviationHU ) { m_fStandardDeviationHU = standardDeviationHU; }

    /// set the ROI volume value
    void SetVolumeValue( const float4 fVolumeValue ) { m_fVolumeValue = fVolumeValue; }

    /// get the ROI volume value
    const float4 & GetVolumeValue() const { return( m_fVolumeValue ); }

    /// set label index 
    void SetLabelIndex( const uint2 uLabelIndex ) { m_uLabelIndex = uLabelIndex; }

    /// get label index
    const uint2 GetLabelIndex() { return(m_uLabelIndex); }

  // member variables
  private:

    /// the average HU value
    float4 m_fAverageHU;

    /// the standard deviation of HU values
    float4 m_fStandardDeviationHU;

    /// the voxels in this region of interest
    std::vector< vxCadVoxelPosition > m_voxels;

    /// ROI volume value
    float4 m_fVolumeValue;

    /// label index in the label volume
    uint2 m_uLabelIndex;

  }; // class vxCadROI

} // namespace vxCAD


// $Log: vxCadROI.h,v $
// Revision 1.6.2.3  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.6.2.2  2008/01/17 23:05:09  dongqing
// make the GetCount as const
//
// Revision 1.6.2.1  2008/01/16 22:04:46  dongqing
// add m_uLabelIndex for the ROI
//
// Revision 1.6  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.5  2006/11/08 16:23:15  jmeade
// const declaration on get function.
//
// Revision 1.4  2006/09/14 17:46:37  dongqing
// add volume value Set/Get
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
// Revision 1.4  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.3  2005/09/14 18:21:40  vxconfig
// working on serialization, added some more constructors
//
// Revision 1.2  2005/09/14 14:28:31  geconomos
// proper casting of vector.size() to int
//
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadROI.h,v 1.6.2.3 2009/07/30 19:26:28 kchalupa Exp $
// $Id: vxCadROI.h,v 1.6.2.3 2009/07/30 19:26:28 kchalupa Exp $
