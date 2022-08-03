// $Id: WmTimer.C,v 1.2 2005/08/12 21:48:54 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: WmTimer.C
// Author(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "WmTimer.h"

uint4 WinUtils::WmTimer::m_uGlobalNextTimerId = 1;


// Revision History:
// $Log: WmTimer.C,v $
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:52   ingmar
// Initial revision.
// Revision 1.1  2001/08/02 21:47:53  kevin
// New File Per Jeff Meade
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WmTimer.C,v 1.2 2005/08/12 21:48:54 jmeade Exp $
// $Id: WmTimer.C,v 1.2 2005/08/12 21:48:54 jmeade Exp $
