// $Id: MovieFormat.C,v 1.1 2005/10/28 13:27:43 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

// includes
#include "StdAfx.h"
#include "MovieFormat.h"
#include "FileExtensions.h"

// namespaces
using namespace Movies;

/** 
 * Default constructor
 */
MovieFormat::MovieFormat() : 
  m_sFileTypeExt( vx::WindowsMediaVideoExt ), 
  m_bIncludeMarkings( true ),
  m_bIncludeOverview( false ) 
{
} // MovieFormat()


/**
 * Desttuctor
 */
MovieFormat::~MovieFormat()
{
} // ~MovieFormat()


// $Log: MovieFormat.C,v $
// Revision 1.1  2005/10/28 13:27:43  geconomos
// refactor and cleanup of movie related classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/MovieFormat.C,v 1.1 2005/10/28 13:27:43 geconomos Exp $
// $Id: MovieFormat.C,v 1.1 2005/10/28 13:27:43 geconomos Exp $
