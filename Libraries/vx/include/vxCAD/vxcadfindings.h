// $Id: vxCadFindings.h,v 1.19.2.2 2009/07/30 19:26:28 kchalupa Exp $
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
#include <string>
#include <vector>
#include "vxCadFinding.h"


//fwd declaration
class vxDOMElement;
template <class Type> class Triple;

namespace vxCAD
{

  /**
   * A set of findings for use in the CAD interface
   */
  class vxCadFindings
  {
  // typedefs
  public:

    /// a cad finding comparison function definition
    typedef bool ( CALLBACK * PFNFINDINGCOMPARE )( vxCadFinding & , vxCadFinding & );

  // construction
  public:

    /// constructor
    vxCadFindings();

    /// copy constructor
    vxCadFindings( const vxCadFindings & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadFindings FromXml( vxDOMElement & element );

    /// serialize object to disk
    void ToFile( const std::string & sFilePath ) const;

    /// deserialize object from disk
    static vxCadFindings FromFile( const std::string & sFilePath );

    /// add a finding
    void AddFinding( const vxCadFinding & finding );

    /// get the number of findings
    const int4 GetCount() const { return static_cast< int4 >( m_findings.size() ); }

    /// returns true/false whether the list is empty
    const bool IsEmpty() const { return (GetCount() <= 0); }

    /// sort the findings
    void SortFindings();

    /// sort the findings
    void SortFindings(PFNFINDINGCOMPARE pFnCompare);

    /// get an individual finding
    const vxCadFinding & GetFinding( const int4 index ) const { return m_findings[ index ]; }

    /// get an individual finding
    vxCadFinding & GetFinding( const int4 index ) { return m_findings[ index ]; }

    /// get an individual finding
    vxCadFinding & GetFinding( const std::string & sID );

    /// gets the selected Finding
    const vxCadFinding & GetSelectedFinding() const { return GetFinding( m_iSelectedIndex ); } 
		
		/// remove a finding
    void RemoveFinding( const int4 index );

    /// clear all findings
    void ClearFindings() { m_findings.clear(); }

    /// get the CAD vendor name
    const std::string & GetCadApiVersion() const { return m_sCadApiVersion; }

    /// set the CAD vendor name
    void SetCadApiVersion( const std::string & sCadApiVersion) { m_sCadApiVersion = sCadApiVersion; }

    /// get the CAD vendor name
    const std::string & GetCadVendor() const { return m_sCadVendor; }

    /// set the CAD vendor name
    void SetCadVendor( const std::string & sCadVendor ) { m_sCadVendor = sCadVendor; }

    /// get the vendor-specific variables map
    const std::map< std::string, std::string > & GetVendorSpecificVariablesMap() const { return m_vendorSpecificVariablesMap; }

    /// set the vendor-specific variables map
    void SetVendorSpecificVariablesMap( const std::map< std::string, std::string > & vendorSpecificVariablesMap ) { m_vendorSpecificVariablesMap = vendorSpecificVariablesMap; }

	  /// gets the selected index
    int4 GetSelectedIndex() const { return m_iSelectedIndex; }

	  /// Sets the selected index
    void SetSelectedIndex( const int4 index);

  // utilities
  public:

    /// go through the findings and convert voxel positions to world positions
    void ResolveUnits( const Triple<float4> & voxelUnits );

  // operators
  public:

    /// assignment operator
    vxCadFindings & operator =( const vxCadFindings & other );

  // member variables
  public:
    /// the value of the CAD threshold (vendor-specific)
    ///  -1   N/A (not applicable to vendor)
    ///  -2   default not found in dataset's CAD file
    /// >=0   valid value for vendor
    int4 m_iCadThresholdValue;

  private:
    
    /// the findings
    std::vector< vxCadFinding > m_findings;

    /// the name of the CAD vendor
    std::string m_sCadVendor;

    /// the CAD API version
    std::string m_sCadApiVersion;

    /// the vendor-specific variables map
    std::map< std::string, std::string > m_vendorSpecificVariablesMap;

	  /// selected index
    int4 m_iSelectedIndex;

  }; // class vxCadFindings

} // using namespace vxCAD


// $Log: vxCadFindings.h,v $
// Revision 1.19.2.2  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.19.2.1  2009/01/31 00:38:35  jmeade
// Issue 6237: volume measurements in the notations pane.
//
// Revision 1.19  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.18  2007/02/27 21:50:17  jmeade
// Issue 5424: ensure cad threshold default is read when necessary.
//
// Revision 1.17  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.16  2006/10/04 23:15:30  jmeade
// code standards.
//
// Revision 1.15  2006/06/01 14:08:23  frank
// fixed bad declaration
//
// Revision 1.14  2006/04/12 23:14:40  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.13  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.12  2006/01/27 20:36:15  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.11  2005/11/21 23:15:54  jmeade
// Merge from Merge_VC_2-0_051121.
//
// Revision 1.10  2005/11/08 20:15:46  frank
// cleaned up code
//
// Revision 1.9.2.3  2006/03/22 20:42:49  frank
// moved the skeleton index to another data structure in the session
//
// Revision 1.9.2.2  2006/01/09 19:43:42  frank
// added variable map to findings object
//
// Revision 1.9.2.1  2005/11/17 20:15:26  jmeade
// Issue 4545: const func declaration where appropriate.
//
// Revision 1.9  2005/11/01 09:50:39  frank
// fixed scaling problem for mm centroids
//
// Revision 1.8  2005/10/07 19:41:29  vxconfig
// added api version
//
// Revision 1.7  2005/09/30 22:22:47  jmeade
// sorting
//
// Revision 1.6  2005/09/28 14:36:42  jmeade
// get finding by id; sort function (shell).
//
// Revision 1.5  2005/09/23 12:02:41  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.4  2005/09/22 17:00:14  jmeade
// non-const get method.
//
// Revision 1.3  2005/09/20 19:30:58  vxconfig
// added voxel units on vector
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadFindings.h,v 1.19.2.2 2009/07/30 19:26:28 kchalupa Exp $
// $Id: vxCadFindings.h,v 1.19.2.2 2009/07/30 19:26:28 kchalupa Exp $
