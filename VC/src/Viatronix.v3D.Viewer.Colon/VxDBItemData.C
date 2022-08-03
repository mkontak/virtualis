// $Id: VxDBItemData.C,v 1.3 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: VxDBItemData.C
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#include "stdafx.h"
#include "VxDBItemData.h"

using namespace ConsoleLib;

/**
 * Constructor
 */
VxDBItemData::VxDBItemData()
 : m_idStudyRecord(int8(0)), m_idDataset(int8(0)), m_iSiteID(0), m_eStudyType(StudyType_Unknown),
  m_eFileType(vx::eFileClassUnknown), m_eItemState(VxDBItemData::DEFAULT), m_hChangeNotify(HANDLE(NULL))
{
} // constructor

// $Log: VxDBItemData.C,v $
// Revision 1.3  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.2  2005/08/24 20:19:34  jmeade
// namespaces for ConsoleLib.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.2.4.2  2003/07/07 20:31:53  jmeade
// Code standards.
//
// Revision 1.2.4.1  2003/06/06 23:03:24  jmeade
// code review prep.
//
// Revision 1.2  2002/07/18 20:12:27  jmeade
// Moved report functionality to ConsoleLib.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/VxDBItemData.C,v 1.3 2007/03/07 22:34:05 jmeade Exp $
// $Id: VxDBItemData.C,v 1.3 2007/03/07 22:34:05 jmeade Exp $
