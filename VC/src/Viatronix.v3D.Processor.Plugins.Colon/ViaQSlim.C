// $Id: ViaQSlim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaQSlim.C  
// Description: Do polygon simplification based on 'MixKit.lib' (QSlim)
// Created:  Oct. 12, 2000
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaQSlim.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static ostream *output_stream = NULL;

ViaQSlim::ViaQSlim()
{
  m_iFaceTarget = 0;
  m_iPlacementPolicy = MX_PLACE_OPTIMAL;
  m_iWeightingPolicy = MX_WEIGHT_AREA;
  m_fBoundaryWeight = 1000.0;
  m_fCompactnessRatio = 0.0;
  m_fMeshingPenalty = 1.0;
  m_bWillUseFSlim = false;
  m_bWillRecordHistory = false;
  m_bWillJoinOnly = false;
  m_sOutputFilename = NULL;
  OutputFormat output_format = SMF;

  history = NULL;
  MxSMFReader *smf = NULL;
  MxStdModel *m = NULL;
  MxEdgeQSlim *eslim = NULL;
  MxFaceQSlim *fslim = NULL;

  target_edges = NULL;
}


ViaQSlim::~ViaQSlim()
{
}


void ViaQSlim::SlimInit()
{
  if (!slim) 
  {
    if (m_bWillUseFSlim)
      slim = fslim = new MxFaceQSlim(*(m_StdModel));
    else
      slim = eslim = new MxEdgeQSlim(*(m_StdModel));
  } 
  else 
  {
    if (m_bWillUseFSlim)
      fslim = (MxFaceQSlim *)slim;
    else
      eslim = (MxEdgeQSlim *)slim;
  }

  slim->m_iPlacementPolicy = m_iPlacementPolicy;
  slim->m_fBoundaryWeight  = m_fBoundaryWeight;
  slim->m_iWeightingPolicy = m_iWeightingPolicy;
  slim->m_fCompactnessRatio= m_fCompactnessRatio;
  slim->m_fMeshingPenalty  = m_fMeshingPenalty;
  slim->m_bWillJoinOnly    = m_bWillJoinOnly;

  if (eslim && target_edges) 
  {
    eslim->initialize(*(target_edges), target_edges->length());
  }
  else
    slim->initialize();

	if (m_bWillRecordHistory) 
  {
    if (!eslim)
      MxMsg::signal(MXMSG_WARN, "History only available for edge contractions.");
    else 
    {
      history = new QSlimLog(100);
      eslim->contraction_callback = slim_history_callback;
    }
  }
}

// this routine is used to replace 'startup_and_input' routine.
void ViaQSlim::ReplacedStartupAndInput(int4 polygonNum)
{
  smf = new MxSMFReader;

  output_format = SMF;          // output file format.
  m_iFaceTarget = polygonNum;   // target number of polygons.

  unparsed_hook = qslim_smf_hook;
  smf->unparsed_hook = unparsed_hook;
  m_StdModel = new MxStdModel(100, 100);

	// for (; optind < argc; optind++) input_file(argv[optind]);
	// InputFromBuffer();	// read the file.
  smf->read(m_StdModel, m_sInputPtr);
  if (m_StdModel->face_count() == 0) 
  {
    smf->read(cin, m_StdModel);
  }
  // output_preamble();
}

void ViaQSlim::InputFromBuffer()
{
  smf->read(m_StdModel, m_sInputPtr);
}

static MxDynBlock<int1*> files_to_include(2);


int1 * ViaQSlim::OutputCurrentModelSimplified()
{
  MxSMFWriter writer;
  return writer.write(*(m_StdModel), &m_iNumVertex, &m_iNumNormals, &m_iNumFaces);
}

void ViaQSlim::CleanupForOutput()
{
  // First, mark stray vertices for removal
  for	(uint4 i=0; i<m_StdModel->vert_count(); i++)
    if (m_StdModel->vertex_is_valid(i) && m_StdModel->neighbors(i).length() == 0)
       m_StdModel->vertex_mark_invalid(i);

  // Compact vertex array so only valid vertices remain
  m_StdModel->compact_vertices();
}


int1 * ViaQSlim::OutputFinalModelSimplified()
{
  CleanupForOutput();
  return OutputCurrentModelSimplified();
}

void ViaQSlim::SlimCleanup()
{
  CLEANUP(smf);
  CLEANUP(m_StdModel);
  CLEANUP(slim);
  eslim = NULL;
  fslim = NULL;
  CLEANUP(history);
  CLEANUP(target_edges);
  if (output_stream != &cout)
    CLEANUP(output_stream);
}


// $Log: ViaQSlim.C,v $
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
// Revision 1.16  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.15  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.14  2001/03/30 16:07:20  binli
// fixed bug reported from CA. ---> quadrilateral polygon
//
// Revision 1.13  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.12  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.11  2001/03/21 15:57:25  binli
// code cleaned according to new code standard.
//
// Revision 1.10  2001/02/26 23:11:13  binli
// code compressed
//
// Revision 1.9  2001/02/09 18:24:42  binli
// // defect 000186: turn the temporal file 'triangle.smf' into in-memory buffer
//
// Revision 1.8  2001/02/08 18:43:44  binli
// defect 000186: turn the temporal file triangle.tmp into in-memory buffer
//
// Revision 1.7  2001/02/04 20:55:22  dave
// added debug print statements
//
// Revision 1.6  2000/11/09 22:07:55  binli
// tried to fix the bug in 'strips creation'. 4 data sets tested. No bugs found!
//
// Revision 1.5  2000/11/06 21:38:54  dave
// added more print stmts, put delete[] back
//
// Revision 1.4  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.2.2.3  2000/10/19 20:37:02  binli
// code optimization
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
// Revision 1.1  2000/10/12 15:29:42  binli
// Integreted the class 'ViaQSlim' to the pipeline.
// This class is responsible for polygon simplification.
// (The code is a downloaded freeware, some modifications have been made by Bin Li
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaQSlim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaQSlim.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
