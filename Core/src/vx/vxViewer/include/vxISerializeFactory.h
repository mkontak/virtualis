// $Id: vxISerializeFactory.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 * This class is a pure interface class for a factory
 * to create a serializable object based on a given 
 * name (string).
 */


#ifndef vxISerializeFactory_h
#define vxISerializeFactory_h



// forward declarations
class vxISerialize;


// class definition
class VX_VIEWER_DLL vxISerializeFactory
{
// functions
public:
  
  /// default constructor
  vxISerializeFactory() {};

  /// destructor
  virtual ~vxISerializeFactory() {};

  /// create object
  virtual vxISerialize * Create(const std::string & sName) const = 0;
}; // vxISerializeFactory


#endif // vxISerializeFactory_h


// Revision History:
// $Log: vxISerializeFactory.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.3  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2003/02/21 19:09:52  michael
// added initial version of the vxISerializeFactory classes
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxISerializeFactory.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxISerializeFactory.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
