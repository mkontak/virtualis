// $Id: ViaQSlim.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaQSlim.h  
// Description: Do polygon simplification based on 'MixKit.lib' (QSlim)
// Created:  Oct. 12, 2000
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#ifndef ViaQSlim_h
#define ViaQSlim_h

#include "stdmix.h"
#include "MxStdSlim.h"
#include "MxSMF.h"
#include "MxQSlim.h"

enum OutputFormat { SMF, PM, MMF, LOG, IV, VRML };
#define CLEANUP(x)  if(x) { delete x; x=NULL; }
typedef MxDynBlock<MxPairContraction> QSlimLog;
extern MxQSlim *slim;
extern void slim_history_callback(const MxPairContraction&,float4);
extern bool qslim_smf_hook(int1 *op, int4, int1 *argv[], MxStdModel& m);

class ViaQSlim 
{
public:
	/// Constructor/Destructor.
	ViaQSlim();
	virtual ~ViaQSlim();

// Member functions.
//-----------------
	bool (*unparsed_hook)(int1 *, int4, int1*[], MxStdModel&);
	void SlimInit();
	void SlimCleanup();
	void InputFromBuffer();
	void ReplacedStartupAndInput(int4 polygonNum);
	int1 * OutputCurrentModelSimplified();
	int1 * OutputFinalModelSimplified();
	void CleanupForOutput();

// Data members.
//--------------
	uint4 m_iFaceTarget;
	int4  m_iPlacementPolicy;
	int4  m_iWeightingPolicy;
	int4	 m_iNumFaces, m_iNumNormals, m_iNumVertex;
	float8 m_fBoundaryWeight;
	float8 m_fCompactnessRatio;
	float8 m_fMeshingPenalty;
	bool m_bWillUseFSlim;
	bool m_bWillRecordHistory;
	bool m_bWillJoinOnly;
	int1 * m_sOutputFilename;
	int1 * m_sInputPtr;
	OutputFormat output_format;

	MxSMFReader * smf;
	MxStdModel * m_StdModel;
	MxEdgeQSlim *eslim;
	MxFaceQSlim *fslim;
	QSlimLog *history;
	MxDynBlock<MxEdge> *target_edges;
}; // class 'ViaQSlim'.

extern ViaQSlim *m_pQSlim;

#endif

// $Log: ViaQSlim.h,v $
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
//    Rev 2.0   Sep 16 2001 23:39:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:40   ingmar
// Initial revision.
// Revision 1.14  2001/04/12 14:38:47  dmitry
// Sec_j_tri base class added
//
// Revision 1.13  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.12  2001/03/27 15:25:31  binli
// 'const' functions for safety.
//
// Revision 1.11  2001/03/22 22:04:34  binli
// removed qslim.h
//
// Revision 1.10  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.9  2001/03/21 15:57:25  binli
// code cleaned according to new code standard.
//
// Revision 1.8  2001/02/28 15:46:42  binli
// removed <getopt.h>
//
// Revision 1.7  2001/02/26 23:11:13  binli
// code compressed
//
// Revision 1.6  2001/02/09 18:24:42  binli
// // defect 000186: turn the temporal file 'triangle.smf' into in-memory buffer
//
// Revision 1.5  2001/02/08 18:43:44  binli
// defect 000186: turn the temporal file triangle.tmp into in-memory buffer
//
// Revision 1.4  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2.2.2  2000/10/19 20:37:02  binli
// code optimization
//
// Revision 1.2.2.1  2000/10/16 18:10:00  binli
// added MixKit.lib
//
// Revision 1.2  2000/10/12 15:55:41  binli
// remain unchanged until the lib 'mixkit;' can be added to the CVS.
//
// Revision 1.1  2000/10/12 15:33:24  binli
// Integreted the class 'ViaQSlim' to the pipeline.
// This class is responsible for polygon simplification.
// (The code is a downloaded freeware, some modifications have been made by Bin Li
//
// Revision 1.1  2000/10/12 15:29:42  binli

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaQSlim.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaQSlim.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
