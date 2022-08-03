// $Id: vxCadVector.h,v 1.6.2.1 2009/07/30 19:26:28 kchalupa Exp $
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
#include "Vector3D.h"

//fwd declaration
class vxDOMElement;


namespace vxCAD
{
  /**
   * A simple vector for use in the CAD interface
   */
  class vxCadVector
  {
  // enums
  public:

    /// the units
    enum Units { MILLIMETER, VOXELS };

  // construction
  public:

    /// default constructor
    vxCadVector();

    /// constructor
    vxCadVector( const float4 fX, const float4 fY, const float4 fZ );

    /// copy constructor
    vxCadVector( const vxCadVector & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadVector FromXml( vxDOMElement & element );

    /// get the vector
    const Vector3D<float4> & GetVector() const { return m_vVector; }

    /// set the vector
    void SetVector( const Vector3D<float4> vector ) { m_vVector = vector; }

    /// get the units
    const enum Units & GetUnits() const { return m_eUnits; }

    /// set the units
    void SetUnits( const enum Units & eUnits ) { m_eUnits = eUnits; }

  // operators
  public:

    /// assignment operator
    vxCadVector & operator =( const vxCadVector & other );

  // member variables
  private:

    /// the vector data
    Vector3D<float4> m_vVector;

    /// the units of the quantity
    enum Units m_eUnits;
    
  }; // class vxCadVector

} // namespace vxCAD

// $Log: vxCadVector.h,v $
// Revision 1.6.2.1  2009/07/30 19:26:28  kchalupa
// Code Walkthrough
//
// Revision 1.6  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.5  2006/01/30 18:16:02  romy
// code review. Rearranged include files
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
// Revision 1.1  2005/09/15 16:38:41  geconomos
// moved from vxColon project
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadVector.h,v 1.6.2.1 2009/07/30 19:26:28 kchalupa Exp $
// $Id: vxCadVector.h,v 1.6.2.1 2009/07/30 19:26:28 kchalupa Exp $
