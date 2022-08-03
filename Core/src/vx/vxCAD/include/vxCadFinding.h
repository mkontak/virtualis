// $Id: vxCadFinding.h,v 1.14.2.2 2009/07/30 19:26:27 kchalupa Exp $
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
#pragma comment( lib, "rpcrt4" )


// includes
#include <string>
#include <map>
#include "vxCadVector.h"
#include "vxCadDimensions.h"
#include "vxCadROI.h"

//fwd declaration
class vxDOMElement;

namespace vxCAD
{

  /**
   * A finding for use in the CAD interface
   */
  class vxCadFinding
  {
  // typedefs
  public:

    /// contains names and values of vendor-specific data
    typedef std::map< std::string, std::string > VendorSpecificVariablesMap;

  // construction
  public:

    /// default constructor
    vxCadFinding();

    /// copy constructor
    vxCadFinding( const vxCadFinding & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadFinding FromXml( vxDOMElement & element );

    /// get the id
    const std::string & GetId() const { return m_sId; }

    /// set the id
    void SetId( const std::string & sId ) { m_sId = sId; }

    /// get the number of the Viatronix segment this finding is within
    const unsigned char & GetSegment() const { return m_uSegment; }

    /// set the number of the Viatronix segment this finding is within
    void SetSegment( const unsigned char & uSegment ) { m_uSegment = uSegment; }

    /// get the centroid (in MM)
    const vxCadVector & GetCentroidMM() const { return m_centroidMM; }

    /// set the centroid (in MM)
    void SetCentroidMM( const vxCadVector & centroidMM ) { m_centroidMM = centroidMM; }

    /// get the normal (in MM)
    const vxCadVector & GetNormalMM() const { return m_normalMM; }

    /// set the normal (in MM)
    void SetNormalMM( const vxCadVector & normalMM ) { m_normalMM = normalMM; }

    /// get the surface ROI
    const vxCadROI & GetSurfaceROI() const { return m_surfaceROI; }

    /// set the surface ROI
    void SetSurfaceROI( const vxCadROI & surfaceROI ) { m_surfaceROI = surfaceROI; }

    /// get the volume ROI
    vxCadROI & GetVolumeROI() { return m_volumeROI; }
    const vxCadROI & GetVolumeROI() const { return m_volumeROI; }

    /// set the volume ROI
    void SetVolumeROI( const vxCadROI & volumeROI ) { m_volumeROI = volumeROI; }

    /// get the dimensions
    const vxCadDimensions & GetDimensions() const { return m_dimensions; }

    /// set the dimensions
    void SetDimensions( const vxCadDimensions & dimensions ) { m_dimensions = dimensions; }

    /// get the vendor-specific variables map
    const VendorSpecificVariablesMap & GetVendorSpecificVariablesMap() const { return m_vendorSpecificVariablesMap; }

    /// set the vendor-specific variables map
    void SetVendorSpecificVariablesMap( const VendorSpecificVariablesMap & vendorSpecificVariablesMap ) { m_vendorSpecificVariablesMap = vendorSpecificVariablesMap; }

  // operators
  public:

    /// assignment operator
    vxCadFinding & operator =( const vxCadFinding & other );

  // member variables
  private:

    /// the id
    std::string m_sId;

    /// the number of the Viatronix segment this finding is within
    unsigned char m_uSegment;

    /// the centroid of the finding in millimeters
    vxCadVector m_centroidMM;

    /// the normal of the finding in millimeters
    vxCadVector m_normalMM;

    /// the surface ROI
    vxCadROI m_surfaceROI;

    /// the volume ROI
    vxCadROI m_volumeROI;

    /// the dimensions
    vxCadDimensions m_dimensions;

    /// the vendor-specific variables map
    VendorSpecificVariablesMap m_vendorSpecificVariablesMap;

  }; // class vxCadFinding

} // namespace vxCAD


// $Log: vxCadFinding.h,v $
// Revision 1.14.2.2  2009/07/30 19:26:27  kchalupa
// Code Walkthrough
//
// Revision 1.14.2.1  2007/06/20 21:40:54  dongqing
// add overload for the GetFindings() since we need to reset the ROI
//
// Revision 1.14  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.13  2007/02/15 00:08:24  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.12  2007/02/02 21:57:50  jmeade
// variable type for vendor map.
//
// Revision 1.11  2006/04/12 23:14:40  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.10  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.9  2006/01/27 20:36:15  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.8  2005/11/08 20:15:46  frank
// cleaned up code
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
// Revision 1.7  2005/09/29 17:12:41  dongqing
// fix normal get bug
//
// Revision 1.6  2005/09/28 16:59:19  vxconfig
// name change: element -> index
//
// Revision 1.5  2005/09/23 12:02:41  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.4  2005/09/23 03:20:49  vxconfig
// working on display of cad findings
//
// Revision 1.3  2005/09/22 17:00:34  jmeade
// store cad finding locations along centerline.
//
// Revision 1.2  2005/09/16 14:15:59  vxconfig
// added to/from file, added clear/remove voxel/finding
//
// Revision 1.1  2005/09/15 16:38:41  geconomos
// moved from vxColon project
//
// Revision 1.5  2005/09/14 20:04:38  vxconfig
// added copy constructors and assignment operators to all CAD classes
//
// Revision 1.4  2005/09/14 19:42:52  vxconfig
// finishing up serialization
//
// Revision 1.3  2005/09/14 18:21:40  vxconfig
// working on serialization, added some more constructors
//
// Revision 1.2  2005/09/14 14:27:31  geconomos
// assigned a uuid to the id property in constructor
//
// Revision 1.1  2005/09/13 17:06:56  frank
// initial version of CAD finding data structures
//
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadFinding.h,v 1.14.2.2 2009/07/30 19:26:27 kchalupa Exp $
// $Id: vxCadFinding.h,v 1.14.2.2 2009/07/30 19:26:27 kchalupa Exp $
