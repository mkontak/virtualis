// $Id: vxISerializeFactoryManager.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)

/**
 *  This class manages a list of vxISerializeFactory objects
 *  to allow for extensability of e.g. plugins and it is ought
 *  to be used as singleton object.
 */

#ifndef vxISerializeFactoryManager_h
#define vxISerializeFactoryManager_h


// includes
#include "vxISerializeFactory.h"


// forward declarations
class vxISerialize;


// class definition
class VX_VIEWER_DLL vxISerializeFactoryManager : public vxISerializeFactory
{
// functions
public:
  
  /// default constructor
  vxISerializeFactoryManager() {};

  /// destructor
  virtual ~vxISerializeFactoryManager();

  /// create object
  virtual vxISerialize * Create(const std::string & sName) const;

  /// add a factory
  static void AddFactory(vxISerializeFactory * pFactory);

  /// remove a factory
  static bool RemoveFactory(vxISerializeFactory * pFactory);

  /// return singleton object
  static vxISerializeFactoryManager & GetManager();

// data
private:

  /// list of factories
  static std::list<vxISerializeFactory *> m_lpFactories;

  /// singleton object
  static vxISerializeFactoryManager * m_pManager;
}; // vxISerializeFactoryManager


#endif // vxISerializeFactoryManager_h


// Revision History:
// $Log: vxISerializeFactoryManager.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.3  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.2  2003/02/21 20:01:24  michael
// static member initialization and other changes
//
// Revision 1.1  2003/02/21 19:09:52  michael
// added initial version of the vxISerializeFactory classes
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxISerializeFactoryManager.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxISerializeFactoryManager.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
