// $Id: vxCadDimension.h,v 1.4.2.1 2009/07/30 19:26:27 kchalupa Exp $
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
#include "vxCadVector.h"


//fwd declaration
class vxDOMElement;

namespace vxCAD
{
  /**
   * A dimension for use in the CAD interface
   */
  class vxCadDimension
  {
  // construction
  public:

    /// default constructor
    vxCadDimension();

    /// constructor
    vxCadDimension( const vxCadVector & startPoint, const vxCadVector & endPoint );

    /// copy constructor
    vxCadDimension( const vxCadDimension & other );

  // member functions
  public:

    /// serialize object to XML
    void ToXml( vxDOMElement & element ) const;

    /// deserialize object from XML
    static vxCadDimension FromXml( vxDOMElement & element );

    /// get the start point
    const vxCadVector & GetStartPoint() const { return m_startPoint; }

    /// get the end point
    const vxCadVector & GetEndPoint() const { return m_endPoint; }

  // operators
  public:

    /// assignment operator
    vxCadDimension & operator =( const vxCadDimension & other );

  // member variables
  private:

    /// the start point
    vxCadVector m_startPoint;

    /// the end point
    vxCadVector m_endPoint;

  }; // class vxCadVector

} // namespace vxCAD

// $Log: vxCadDimension.h,v $
// Revision 1.4.2.1  2009/07/30 19:26:27  kchalupa
// Code Walkthrough
//
// Revision 1.4  2007/03/12 19:43:27  jmeade
// code standards.
//
// Revision 1.3  2006/01/30 18:16:02  romy
// code review. Rearranged include files
//
// Revision 1.2  2005/11/03 21:25:21  frank
// fixed some constant function signatures
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
// $Header: /CVS/cvsRepository/V3D/src/vxCAD/include/vxCadDimension.h,v 1.4.2.1 2009/07/30 19:26:27 kchalupa Exp $
// $Id: vxCadDimension.h,v 1.4.2.1 2009/07/30 19:26:27 kchalupa Exp $
