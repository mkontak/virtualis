// $Id: MovieSaveStateOptions.C,v 1.4.2.1 2009/01/21 22:02:59 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

#include "StdAfx.h"
#include "MovieSaveStateOptions.h"
//#include "ReaderGlobal.h"
//#include "SliceChildView.h"


using namespace Movies;
using namespace ReaderLib;
using namespace ColonViews::ChildViews;

/**
 * Constructor
 */
MovieSaveStateOptions::MovieSaveStateOptions() : 
  m_coordDisplayValue(SliceChildView::m_eCoordDisplay),
  m_renderSize(rdrGlobal.m_supine.m_fields.m_volumeRender.GetRenderSize(), rdrGlobal.m_prone.m_fields.m_volumeRender.GetRenderSize()),
  m_bBiopsyMode(rdrGlobal.m_bBiopsyMode), m_bSpaceLeaping(rdrGlobal.m_bSpaceLeaping), m_bShow2DCamera(SliceChildView::GetIsCameraShowing()),
  m_transWinLoc(rdrGlobal.m_supine.m_fields.m_volumeRender.m_magicWindowStartPt, rdrGlobal.m_prone.m_fields.m_volumeRender.m_magicWindowStartPt)
{
} // constructor


/**
 * Destructor
 */
MovieSaveStateOptions::~MovieSaveStateOptions()
{
  rdrGlobal.m_supine.m_fields.m_volumeRender.SetRenderSize(m_renderSize.first);
  rdrGlobal.m_prone.m_fields.m_volumeRender.SetRenderSize(m_renderSize.second);
  SliceChildView::ShowCamera(m_bShow2DCamera);
} // destructor


// $Log: MovieSaveStateOptions.C,v $
// Revision 1.4.2.1  2009/01/21 22:02:59  jmeade
// Issue 6223: making 2d camera location a global option; reset after completing movie creation.
//
// Revision 1.4  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.3  2007/03/08 22:22:05  romy
// code review - header files adjusted
//
// Revision 1.2  2006/09/21 19:52:45  geconomos
// Issue #4979: proper storing and restoration of primary and secondary volume render sizes
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieSaveStateOptions.C,v 1.4.2.1 2009/01/21 22:02:59 jmeade Exp $
// $Id: MovieSaveStateOptions.C,v 1.4.2.1 2009/01/21 22:02:59 jmeade Exp $
