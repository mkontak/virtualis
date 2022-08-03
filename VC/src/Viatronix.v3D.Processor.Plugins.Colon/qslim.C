// $Id: qslim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: qslim.C  
// Description: auxiliary functions for class 'ViaQSlim'
// Created:  Oct. 12, 2000
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "stdmix.h"
#include "ViaQSlim.h"

ViaQSlim * m_pQSlim;
MxQSlim * slim = NULL;

bool qslim_smf_hook(int1 *op, int, int1 *argv[], MxStdModel& m)
{
	if (streq(op, "e")) {
		if (!m_pQSlim->target_edges)
			m_pQSlim->target_edges = new MxDynBlock<MxEdge>(m.vert_count() * 3);

		uint i = m_pQSlim->target_edges->add();

		(*(m_pQSlim->target_edges))[i].v1 = atoi(argv[0]) - 1;
		(*(m_pQSlim->target_edges))[i].v2 = atoi(argv[1]) - 1;

		return true;
	}
	return false;
}

void slim_history_callback(const MxPairContraction& conx, float4 cost)
{
	m_pQSlim->history->add(conx);
}

// $Log: qslim.C,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:50  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:34   ingmar
// Initial revision.
// Revision 1.7  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.6  2001/03/27 15:25:31  binli
// 'const' functions for safety.
//x
// Revision 1.5  2001/03/22 22:04:33  binli
// removed qslim.h
//
// Revision 1.4  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2.2.2  2000/10/17 21:49:26  ingmar
// made MxKit library vx style
//
// Revision 1.2.2.1  2000/10/16 18:10:00  binli
// added MixKit.lib
//
// Revision 1.2  2000/10/12 15:55:41  binli
// remain unchanged until the lib 'mixkit;' can be added to the CVS.
//
// Revision 1.1  2000/10/12 15:36:43  binli
// Integreted the class 'ViaQSlim' to the pipeline.
// This class is responsible for polygon simplification.
// (The code is a downloaded freeware, some modifications have been made by Bin Li
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/qslim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: qslim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
