// $Id: MovieFrame.C,v 1.3.2.1 2009/01/21 06:02:33 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

#include "StdAfx.h"
#include "MovieFrame.h"
//#include "ReaderGlobal.h"

using namespace Movies;

/**
 * Constructor
 */
MovieFrame::MovieFrame() :  
m_eFrameType( MovieFrame::FLIGHT ), 
  m_bSpaceLeaping( true ), 
  m_eDatasetOrientation( ReaderLib::DatasetOrientations::eSUPINE ),
  m_uSampleTime( 0 ), m_bShow2DCamera( true )
{
} // MovieFrame()


/**
 * Destructor
 */
MovieFrame::~MovieFrame()
{
} // ~MovieFrame()


// $Log: MovieFrame.C,v $
// Revision 1.3.2.1  2009/01/21 06:02:33  jmeade
// show/hide position camera in 2d movie.
//
// Revision 1.3  2007/03/12 19:43:46  jmeade
// code standards.
//
// Revision 1.2  2007/03/08 22:21:56  romy
// code review - header files adjusted
//
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieFrame.C,v 1.3.2.1 2009/01/21 06:02:33 jmeade Exp $
// $Id: MovieFrame.C,v 1.3.2.1 2009/01/21 06:02:33 jmeade Exp $
