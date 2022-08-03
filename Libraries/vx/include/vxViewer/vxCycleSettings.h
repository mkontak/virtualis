// $Id: vxCycleSettings.h,v 1.1 2007/05/03 21:55:04 gdavidson Exp $
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
#include "vxElement.h"


class VX_VIEWER_DLL vxCycleSettings : public vxElement
{
public:

  /// default constructor
  vxCycleSettings();

  /// gets the enabled flag
  inline bool IsEnabled() const { return m_bEnabled; }

  /// sets the enabled flag
  inline void SetEnabled( bool bEnabled ) { m_bEnabled = bEnabled; }

private:

  /// enabled flag
  bool m_bEnabled;

}; // class vxCycleSettings


// $Log: vxCycleSettings.h,v $
// Revision 1.1  2007/05/03 21:55:04  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCycleSettings.h,v 1.1 2007/05/03 21:55:04 gdavidson Exp $
// $Id: vxCycleSettings.h,v 1.1 2007/05/03 21:55:04 gdavidson Exp $

