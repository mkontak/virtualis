// $Id: vxViewObliqueData.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille (frank@viatronix.com)


// Includes
#include "stdafx.h"
#include "vxViewObliqueData.h"
#include "vxCamera.h"
#include "Quadrilateral.h"


// Defines
#define FILE_REVISION "$Revision: 1.1.1.1 $"


/**
 * Default constructor
 */
vxViewObliqueData::vxViewObliqueData()
: m_fFieldOfViewMM( 100.0F )
{
} // vxViewObliqueData()


/**
 * Destructor
 */
vxViewObliqueData::~vxViewObliqueData()
{
} // ~vxViewObliqueData()


/**
 * Get the four corners of the oblique plane in world coordinates.
 *
 * @return the quadrilateral described by the oblique view data
 */
Quadrilateral vxViewObliqueData::GetQuadrilateralMM() const
{
  return Quadrilateral( m_camera.GetEye(), m_camera.GetViewUp(), m_camera.GetViewRight(), m_fFieldOfViewMM );
} // GetQuadrilateralMM()


// $Log: vxViewObliqueData.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/21 18:43:26  frank
// code review
//
// Revision 1.6  2003/04/25 12:09:00  michael
// more code review
//
// Revision 1.5  2003/02/13 17:01:58  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.4  2003/02/06 20:11:50  frank
// Changed default FOV
//
// Revision 1.3.2.1  2003/02/12 22:48:16  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2002/11/21 01:40:49  frank
// Increased field of view.
//
// Revision 1.2  2002/11/14 16:33:04  michael
// changed vxCamera::GetRightDirection() to vxCamera::GetViewRight()
//
// Revision 1.1  2002/11/13 22:08:31  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.1  2002/11/13 20:11:05  frank
// enabled oblique view renderer and manipulator
//
// $Id: vxViewObliqueData.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewObliqueData.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
