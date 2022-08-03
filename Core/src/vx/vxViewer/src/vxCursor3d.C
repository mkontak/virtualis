// $Id: vxCursor3d.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxCursor3d.h"


/**
 * Assignment operator
 * @param reference point
 */
const vxCursor3d & vxCursor3d::operator=(const vxCursor3d & other)
//*******************************************************************
{
  vxCamera::operator=(other);
  return *this;
} // assignment operator


/**
 * Sets the reference point and makes sure it is not the same as the eye
 * @param reference point
 */
void vxCursor3d::SetReference(const Point<float4> & reference)
//*******************************************************************
{
  if (GetEye() != reference)
  {
    SetViewReference(reference);
  }
  else
  {
    SetViewReference(Point<float4>(reference.X(), reference.Y(), reference.Z()+1));
  }
} // SetReference()


// Revision History:
// $Log: vxCursor3d.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/01/05 18:57:23  michael
// added "mailto:"
//
// Revision 1.3  2003/10/01 21:23:52  michael
// cosmetics
//
// Revision 1.2  2003/06/25 18:36:41  frank
// formatting
//
// Revision 1.1  2003/05/06 15:39:07  michael
// added initial version
//
// $Id: vxCursor3d.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCursor3d.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
