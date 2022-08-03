// $Id: vxCadDimensions.h,v 1.3.2.1 2009/07/30 19:26:27 kchalupa Exp $
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
#include "vxCadDimension.h"


//fwd declaration
class vxDOMElement;

namespace vxCAD
{
  /**
   * A set of three orthogonal dimensions for use in the CAD interface
   */
  class vxCadDimensions
  {
  // construction
  public:

    /// default constructor
    vxCadDimensions();

    /// constructor
    vxCadDimensions( const vxCadDimension & largest, const vxCadDimension & perpendicular, const vxCadDimension & depth );

    /// copy constructor
    vxCadDimensions( const vxCadDimensions & other );

    /// assignment operator
    vxCadDimensions & operator =( const vxCadDimensions & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadDimensions FromXml( vxDOMElement & element );

    /// get the largest dimension
    const vxCadDimension & GetLargestDimension() const { return m_largest; }

    /// get the perpendicular dimension (perpendicular to largest)
    const vxCadDimension & GetPerpendicularDimension() const { return m_perpendicular; }

    /// get the depth dimension (perpendicular to all others)
    const vxCadDimension & GetDepthDimension() const { return m_depth; }


	// member variables
  private:

    /// the largest dimension
    vxCadDimension m_largest;

    /// the perpendicular dimension (perpendicular to largest)
    vxCadDimension m_perpendicular;

    /// the depth dimension (perpendicular to all others)
    vxCadDimension m_depth;

  }; // class vxCadDimensions

} // namespace vxCAD


// $Log: vxCadDimensions.h,v $
// Revision 1.3.2.1  2009/07/30 19:26:27  kchalupa
// Code Walkthrough
//
// Revision 1.3  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.2  2006/01/30 18:16:02  romy
// code review. Rearranged include files
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadDimensions.h,v 1.3.2.1 2009/07/30 19:26:27 kchalupa Exp $
// $Id: vxCadDimensions.h,v 1.3.2.1 2009/07/30 19:26:27 kchalupa Exp $
