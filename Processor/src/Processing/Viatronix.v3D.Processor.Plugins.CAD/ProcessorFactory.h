// $Id: ProcessorFactory.h,v 1.3 2007/03/07 15:59:39 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// include declarations
#include "vxComCAD.h"

class ProcessorFactory
{
// non-member functions
public:

  /// creates a new Processor instance and returns it's IProcessor interface 
  static HRESULT  Create( void ** ppiProcessor );

}; // class ProcessorFactory


// $Log: ProcessorFactory.h,v $
// Revision 1.3  2007/03/07 15:59:39  gdavidson
// Code review
//
// Revision 1.2  2007/03/01 20:40:36  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ProcessorFactory.h,v 1.3 2007/03/07 15:59:39 gdavidson Exp $
// $Id: ProcessorFactory.h,v 1.3 2007/03/07 15:59:39 gdavidson Exp $
