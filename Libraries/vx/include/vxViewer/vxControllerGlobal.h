// $Id: vxControllerGlobal.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (mailto:george@viatronix.com)


// prgma declarations
#pragma once


// forward declarations
class vxViewer;


/**
 * Base class for all GlobalControllers
 */
class VX_VIEWER_DLL vxControllerGlobal
{
// member functions
public:

  /// constructor
  vxControllerGlobal();

  /// destructor
  virtual ~vxControllerGlobal();

  /// peforms initialization
  virtual void Initialize() {}

  /// determine if processing is necessary
  virtual bool IsProcessingNecessary() = 0;

  /// process the data
  virtual void Process() = 0;

  /// gets the viewers
  const std::vector< vxViewer * > & GetViewers() { return m_viewers; }

  /// sets the viewers
  void SetViewers( const std::vector< vxViewer * > & viewers ) { m_viewers = viewers; }

// member variables
private:

  /// viewers
  std::vector< vxViewer * > m_viewers;

}; // class vxControllerGlobal


// $Log: vxControllerGlobal.h,v $
// Revision 1.2  2007/03/02 17:19:14  geconomos
// code review preparation
//
// Revision 1.1  2006/09/14 20:05:12  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxControllerGlobal.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
// $Id: vxControllerGlobal.h,v 1.2 2007/03/02 17:19:14 geconomos Exp $
