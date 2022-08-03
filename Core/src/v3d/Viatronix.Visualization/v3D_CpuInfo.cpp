// $Id: v3D_CpuInfo.cpp,v 1.1 2007/03/22 18:24:35 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "v3D_CpuInfo.h"
#include "CpuInfo.h"
#include "SystemInfo.h"


USING_VISUALIZATION_NS


/**
 * default constructor
 */
Viatronix::Visualization::CpuInfo::CpuInfo() :
  m_cpuInfo( SystemInfo::GetCpuInfo() )
{
} // CpuInfo()



// $Log: v3D_CpuInfo.cpp,v $
// Revision 1.1  2007/03/22 18:24:35  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CpuInfo.cpp,v 1.1 2007/03/22 18:24:35 gdavidson Exp $
// $Id: v3D_CpuInfo.cpp,v 1.1 2007/03/22 18:24:35 gdavidson Exp $

