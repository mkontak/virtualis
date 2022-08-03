// $Id: v3D_VolumeHeader.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (meissner@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_VolumeHeader.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializer from datatype
 * @param eDatatpe volume datatype 
 */
 VolumeHeader::VolumeHeader( System::IntPtr data )
{
  m_pVolumeHeader = reinterpret_cast< vxVolumeHeader * >(data.ToPointer());
} // constructor


// Revision History:
// $Log: v3D_VolumeHeader.cpp,v $
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/06/04 17:18:11  michael
// added initial version for wrapping vxVolumeHeader
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VolumeHeader.cpp,v 1.1.1.1 2004/03/02 03:58:39 geconomos Exp $
// $Id:
