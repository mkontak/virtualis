// $Id: vxNodeGlobalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


#pragma once

// includes
#include "vxVisualizationGraphNode.h"


// forward declarations
class vxControllerGlobal;


class VX_VIEWER_DLL vxNodeGlobalController : public vxVisualizationGraphNode
{
public:

  /// function pointer for the index changed method
  typedef void ( __stdcall* ClearFuncPtr )();

// functions
public:

  /// default constructor
  vxNodeGlobalController();

  /// constructor
  vxNodeGlobalController( ClearFuncPtr pFuncPtr );

  /// adds a global controller to the node
  void AddController( std::string sId, vxControllerGlobal * pGlobalController ); 

  /// removes a global controller from the node
  void RemoveController( const std::string & sId ); 

  /// gets a global controller based on index
  inline const vxControllerGlobal * GetController( const int iIndex ) const { return m_globalControllers[ iIndex ]; }

  /// gets a global controller based on index
  inline const vxControllerGlobal * GetController( const std::string & sId ) { return m_globalControllerMap[ sId ]; }

  /// checks if the node contains the global controllers
  inline const bool HasController( const std::string & sId ) const { return m_globalControllerMap.find( sId ) != m_globalControllerMap.end(); }

  /// clears all global controllers
  void ClearControllers();

// vxVisualizationGraphNode functions
public:

  /// does the node requires a flush
  inline bool IsFlushNecessary() const { return true; }

  /// initializes the node
  void Initialize();

  /// executes the node
  void Run();

  /// resets the node
  void Reset();

// data
private:

  /// collection of global controllers
  std::vector< vxControllerGlobal * > m_globalControllers;

  /// mapping of type strings to global controllers
  std::map< std::string, vxControllerGlobal * > m_globalControllerMap;

  /// clear function pointer
  ClearFuncPtr m_pClearFuncPtr;

}; // class vxNodeGlobalController


// $Log: vxNodeGlobalController.h,v $
// Revision 1.1  2007/05/03 21:42:28  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeGlobalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
// $Id: vxNodeGlobalController.h,v 1.1 2007/05/03 21:42:28 gdavidson Exp $
