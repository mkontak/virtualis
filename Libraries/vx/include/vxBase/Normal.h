// $Id: Normal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Normal_h
#define Normal_h


// defines
// allow to still use old class name
#define Normal Normal3D 


// include declarations 
#include "Normal3D.h"


#endif  // Normal_h


// $Log: Normal.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.16  2003/05/16 11:43:46  michael
// code review
//
// Revision 3.15  2003/01/22 20:41:59  ingmar
// moved all code to Normal3D, Normal is now a typedef for Normal3D
//
// Revision 3.14  2001/01/03 12:46:24  ingmar
// code beautify
//
// Revision 3.13  2001/01/02 19:38:03  ingmar
// better documentation structure
//
// Revision 3.12  2001/01/02 18:50:49  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.11  2001/01/02 16:17:46  ingmar
// no longer derived from Triple
//
// Revision 3.10  2002/05/13 15:42:47  michael
// removed the Vector constructor since it is already in Triple
//
// Revision 3.9  2002/05/13 15:15:26  michael
// Added copy constructor for Normal3D using Vector3D
//
// Revision 3.8  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.7  2002/02/26 20:51:17  jaddonisio
// New Dot function.
//
// Revision 3.6  2002/02/26 17:19:55  michael
// declared operator[] to be const
//
// Revision 3.5  2002/02/25 21:19:49  michael
// made Get{Abs}MaxComponent{Coord}() const
//
// Revision 3.4  2002/02/22 17:01:54  michael
// Get...MaxComponentCoord() returns now uint1 (index) and not the template type
//
// Revision 3.3  2002/02/19 16:17:07  jaddonisio
// New methods to get max. component values.
//
// Revision 3.2  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.1  2001/12/12 20:02:31  jmeade
// Comments.
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:12   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:46   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:08   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.11  2001/08/23 17:48:09  sarang
// linux/gcc fix.
//
// Revision 1.10  2001/05/31 21:32:42  ana
// changes regarding code review
//
// Revision 1.9  2001/05/25 20:28:05  kevin
// Moved Project Normal3D onto plane
//
// Revision 1.8  2001/05/08 18:21:17  ana
// Modifications after code review
//
// Revision 1.7  2001/05/01 23:03:33  ana
// code standards
//
// Revision 1.6  2001/04/30 13:59:19  ana
// changes to fit in code standards
//
// Revision 1.5  2001/04/17 23:34:26  ana
// added heading comments
//
// Revision 1.4  2001/04/17 21:43:14  ana
// added TestByType function
//
// Revision 1.3  2001/04/09 18:03:20  dmitry
// Removed Revision History from Top of File
//
// Revision 1.2  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 8     8/17/00 1:04p Antonio
// modified header
// 
// *****************  Version 7  *****************
// User: Antonio      Date: 8/10/00    Time: 4:21p
// Updated in $/vx/src/Math
// modified header to newer version.
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/02/00    Time: 9:21p
// Updated in $/vx/src/Math
// added complete history to end of file
//
// *****************  Version 5  *****************
// User: Antonio         Date:  8/01/00  Time:  3:06p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 4  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point, Normal3D, Vector
// Since no virtual functions are desired, the same fn. appears in all despite
// inheritance
// 
// *****************  Version 3  *****************
// User: Frank           Date:  7/18/00  Time:  3:55p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 2  *****************
// User: Jay             Date:  7/18/00  Time: 11:07a
// Checked in $/vx/src/Math
// Comment:
//   Made dll linking work for the static functions
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Normal3D.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Normal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Normal.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
