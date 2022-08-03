// $Id: vxController.h,v 1.4 2007/02/05 15:25:20 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxController_h
#define vxController_h


// forward declarations
class vxEnvironment;


/**
 * Base class to define the operations of a controller.
 * A controller takes the current environment (ideally a modified queue)
 * and modifies it according to some logic.
 */
class VX_VIEWER_DLL vxController
{
// member functions
public:

  /// constructor
  vxController( vxEnvironment & environment );

  /// destructor
  virtual ~vxController() {}

  /// set the environment
  void SetEnvironment( vxEnvironment & environment ) { m_pEnvironment = & environment; }

  /// get the environment
  vxEnvironment & GetEnvironment() const { return * m_pEnvironment; }

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary() = 0;

  /// process the data
  virtual void Process() = 0;

// data members
private:

  /// the environment
  vxEnvironment * m_pEnvironment;

}; // class vxController


#endif // vxController_h


// $Log: vxController.h,v $
// Revision 1.4  2007/02/05 15:25:20  geconomos
// added virtual destructor
//
// Revision 1.3  2004/09/27 14:15:54  frank
// added pre-processing check to controller interface
//
// Revision 1.2  2004/05/20 12:45:12  frank
// added get and set environment methods
//
// Revision 1.1  2004/05/19 15:47:08  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxController.h,v 1.4 2007/02/05 15:25:20 geconomos Exp $
// $Id: vxController.h,v 1.4 2007/02/05 15:25:20 geconomos Exp $
