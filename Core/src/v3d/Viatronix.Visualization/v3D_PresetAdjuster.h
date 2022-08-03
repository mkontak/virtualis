// $Id: v3D_PresetAdjuster.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
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


// namespaces
OPEN_VISUALIZATION_NS


// forward declaration
ref class Volume;
ref class Preset;


/**
 * 
 */
public ref class PresetAdjuster
{

// member functions
public:

  PresetAdjuster();

  virtual void Adjust( Viatronix::Visualization::Preset ^ mpPreset, Viatronix::Visualization::Volume ^ mpVolume );
  

}; // class ClientConfigSectionHandle


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_PresetAdjuster.h,v $
// Revision 1.2  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2005/01/07 16:58:51  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PresetAdjuster.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_PresetAdjuster.h,v 1.2 2006/10/04 18:29:05 gdavidson Exp $