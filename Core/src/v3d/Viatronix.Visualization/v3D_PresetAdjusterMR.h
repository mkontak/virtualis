// $Id: v3D_PresetAdjusterMR.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright (c) 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// pragmas
#pragma once


// includes
#include "v3D_PresetAdjuster.h"


// namespaces
OPEN_VISUALIZATION_NS


/**
 * 
 */
public ref class PresetAdjusterMR : public PresetAdjuster
{

// member functions
public:

  PresetAdjusterMR();

  virtual void Adjust( Viatronix::Visualization::Preset ^ mpPreset, Viatronix::Visualization::Volume ^ mpVolume ) override;
  
}; // class ClientConfigSectionHandle


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_PresetAdjusterMR.h,v $
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2005/01/07 16:58:51  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PresetAdjusterMR.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_PresetAdjusterMR.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $