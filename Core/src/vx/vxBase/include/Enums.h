// $Id: Enums.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Enums.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: Enums.h
// Description: Declaration of enumerations. 
// Created: June 13, 2001
// Author: Wei Li (liwei@viatronix.com)

#ifndef __ENUMS_H
#define __ENUMS_H


namespace nsTEXTURE 
{
	enum INTERPOLATION {INTERP_NEAREST, INTERP_LINEAR};
};

namespace nsORIENTATION 
{
	enum ORIENTATION {X_ORIENTATION, Y_ORIENTATION, Z_ORIENTATION};
	enum DIRECTION { POSITIVE, NEGATIVE};
};

namespace nsRENDERER3D
{
	enum PROJECTION_MODE { PERSPECTIVE, ORTHOGONAL, OTHER };
};

namespace DIR 
{
  enum DIRECTION {UNKNOWN, EAST, WEST, SOUTH, NORTH, NORTH_EAST, NORTH_WEST, SOUTH_WEST, SOUTH_EAST};
};

namespace nsCOLOR 
{
	enum COLOR_CHANNEL { RED = 0, GREEN, BLUE, ALPHA};
}


#endif


// $Log: Enums.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/16 11:44:22  michael
// code review
//
// Revision 3.2  2002/03/08 22:38:39  michael
// include cleanup
//
// Revision 3.1  2001/12/24 15:36:55  ingmar
// added class Sec_20_image to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:19  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 12 2001 18:01:22   liwei
// nsColor moved here.
// 
//    Rev 2.0   Sep 16 2001 23:47:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:34   ingmar
// Initial revision.
// Revision 1.2  2001/09/06 00:39:37  liwei
// Neighbor4EdgeIterator uses transition lookup table.
//
// Revision 1.1  2001/09/01 00:11:44  soeren
// renamed image directory
//
// Revision 1.3  2001/07/02 17:12:34  liwei
// names of namespaces in enums.h changed
//
// Revision 1.2  2001/06/28 20:21:13  liwei
// Added enum OTHER as a projection mode.
//
// Revision 1.1  2001/06/18 16:55:06  liwei
// Classes related to Texture moved to 5_image
//
// Revision 1.1  2001/06/13 14:29:36  liwei
// Added to CVS
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Enums.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Enums.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $