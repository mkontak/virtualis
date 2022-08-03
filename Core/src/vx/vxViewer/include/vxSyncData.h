// $Id: vxSyncData.h,v 1.1 2008/05/02 04:59:22 cqian Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Cheng Qian (mailto:cqian@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxElement.h"


class vxEnvironment;



// class definition
class VX_VIEWER_DLL vxSyncData : public vxElement
{
// functions
public:

  /// constructor
  vxSyncData();

  /// copy constructor
  ~vxSyncData();

  /// if the sync is enabled
  static inline bool IsEnabled() { return  m_bEnabled; }

  /// to sync the viewers 
  static inline void Enable() { m_bEnabled = true; }

  /// to remove the sync between viewers 
  static inline void Disable() { m_bEnabled = false; } 

  /// set the enabled field
  static inline void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }

  /// return the pointers to the viewers to be synchronized 
  inline std::vector< vxEnvironment * > & GetEnvironments() { return  m_environments; }

  // get the viewer at the specified index
  inline vxEnvironment * GetEnvironment( int4 index ) { return m_environments[ index ]; }

	/// add a viewer to the sychronization  
  void AddEnvironment( vxEnvironment * pEnvironment );

  // remove a viewer from the sychronization  
  void RemoveEnvironment( vxEnvironment * pEnvironment );

private:

  /// the vector of views to be synchronized 
  std::vector< vxEnvironment * > m_environments;

  /// whether the sync is enabled or not 
  static bool m_bEnabled;

}; // vxSyncViewers 


// Revision History:

// $Id: vxSyncData.h,v 1.1 2008/05/02 04:59:22 cqian Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSyncData.h,v 1.1 2008/05/02 04:59:22 cqian Exp $
