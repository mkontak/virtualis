// $Id: vxEnvironmentList.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson (gdavidson@viatronix.com)

#pragma once

// Includes
#include "vxElement.h"
#include "Triple.h"
#include "Vector.h"


// Forward declarations
class vxEnvironment;


/**
 * Class to contain a list of lassos and process them in order
 * to create a new component.
 */
class VX_VIEWER_DLL vxEnvironmentList : public vxElement
{
// Typedefs
public:

  // iterator for the list of lassos.
  typedef std::list< vxEnvironment * > EnvironmentList;

// Member Functions
public:
  
  /// Constructor
  vxEnvironmentList();
  
  /// Destructor
  virtual ~vxEnvironmentList();

  /// Get the list of environments
  std::list< vxEnvironment * > & GetEnvironmentList() { return m_vEnvironmentList; }
    
// Data Members
private:

  /// List to store the collection of environments
  std::list< vxEnvironment * > m_vEnvironmentList;

}; // class vxEnvironmentList


// $Log: vxEnvironmentList.h,v $
// Revision 1.2  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.1  2006/03/27 18:57:49  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxEnvironmentList.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
// $Id: vxEnvironmentList.h,v 1.2 2007/03/07 18:06:10 geconomos Exp $
