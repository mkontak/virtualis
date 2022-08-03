// $Id: vxRenderer.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dmitry Gritsayenko (dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxRenderer.h"
#include "vxEnvironment.h"


/**
 * Constructor
 * @param environment
 */
vxRenderer::vxRenderer(vxEnvironment & environment):
m_pEnvironment(&environment),
m_bEnabled(true)
//*******************************************************************
{
}


/**
 * Set the environment
 * @param environment
 */
void vxRenderer::SetEnvironment(vxEnvironment & environment)
//*******************************************************************
{
  m_pEnvironment = &environment;
}

/**
 * Get the environment
 * @return environment
 */
vxEnvironment & vxRenderer::GetEnvironment() const
//*******************************************************************
{
  return *m_pEnvironment;
}


// Revision History:
// $Log: vxRenderer.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2003/11/18 19:57:51  michael
// took out empty line
//
// Revision 1.15  2003/06/09 12:30:22  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.14  2003/05/17 15:44:49  michael
// reformatting
//
// Revision 1.13  2003/05/16 13:22:13  frank
// formatting
//
// Revision 1.12  2003/05/14 17:28:24  michael
// code review
//
// Revision 1.11  2003/05/14 16:10:19  michael
// code review
//
// Revision 1.10  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.9  2003/02/28 21:43:00  michael
// removed vxWinRect from vxRenderer. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.8  2003/01/28 15:04:17  frank
// Const access functions should be const!
//
// Revision 1.7  2002/07/29 14:57:49  michael
// Removed viewer type from vxViewer and moved it to vxRenderer
//
// Revision 1.6  2002/07/26 23:16:15  michael
// added a view type to the renderers
//
// Revision 1.5  2002/04/02 21:06:21  dmitry
// Added SetEnabled and IsEnabled methods.
//
// Revision 1.4  2002/03/19 17:28:56  dmitry
// added winRect to the Resize of the vxRenderer (and vxViewer)
//
// Revision 1.3  2002/03/12 15:38:36  jaddonisio
// Added vxRenderer to 79
//
// Revision 1.1  2002/03/11 20:40:44  michael
// added initial vxRenderer classes
//
// Revision 1.3  2002/03/07 22:40:55  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.1  2002/01/08 21:46:51  dmitry
// Initial Version, implementation retrieved from Renderer3D.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxRenderer.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxRenderer.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
