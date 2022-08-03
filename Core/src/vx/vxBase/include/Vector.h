// $Id: Vector.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com


#ifndef Vector_h
#define Vector_h


// defines
// allow to still use old class name
#define Vector Vector3D 


// include declarations 
#include "Vector3D.h"


#endif  // Vector_h


// $Log: Vector.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:48:47  michael
// code review
//
// Revision 3.8  2003/01/22 21:05:31  ingmar
// moved all code to Vector3D, Vector is now a typedef for Vector3D
//
// Revision 3.7  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.6  2002/02/26 18:54:00  jaddonisio
// New distance functions.
//
// Revision 3.5  2002/02/15 14:47:32  jaddonisio
// Moved the [] operator to the Triple class.
//
// Revision 3.4  2002/02/11 18:35:57  michael
// added helper functions to the Triple which are used in the texture map based renderers
//
// Revision 3.3  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.2  2001/12/13 15:56:31  michael
// fixed bug in inl file, sorry. :-(
//
// Revision 3.1  2001/12/11 16:29:04  michael
// added opertor[] to Point and Vector3D class, simply because it is more
// convenient to use in loops ... but might be less efficient ...
//
// Revision 3.0  2001/10/14 23:02:18  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:22   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:47:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:22   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:41  soeren
// renamed math directory
//
// Revision 1.13  2001/08/23 17:50:11  sarang
// linux/gcc fix.
//
// Revision 1.12  2001/06/12 13:00:05  frank
// Added GetSquaredLength() for effciency.
//
// Revision 1.11  2001/05/31 21:32:08  ana
// changes regarding code review
//
// Revision 1.10  2001/05/08 18:22:06  ana
// Modifications after code review
//
// Revision 1.9  2001/05/01 23:04:20  ana
// code standards
//
// Revision 1.8  2001/04/30 13:59:39  ana
// changes to fit in code standards
//
// Revision 1.7  2001/04/17 23:34:10  ana
// added heading comments
//
// Revision 1.6  2001/04/16 13:51:59  ana
// added TestByType method
//
// Revision 1.5  2001/04/09 18:03:19  dmitry
// Removed Revision History from Top of File
//
// Revision 1.4  2001/04/05 18:00:04  dmitry
// updated datatypes
//
// Revision 1.3  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
// 
// 8     8/17/00 1:32p Antonio
// modified header
// 
// *****************  Version 7  *****************
// User: Antonio      Date: 8/10/00    Time: 4:30p
// Updated in $/vx/src/Math
// Modified header to newer version.
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/02/00    Time: 9:40p
// Updated in $/vx/src/Math
// added complete history to end of file
// 
// *****************  Version 5  *****************
// User: Antonio         Date:  8/02/00  Time: 12:15p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 4  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point, Normal, Vector3D
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
// User: Ingmar          Date:  7/18/00  Time: 10:28a
// Checked in $/vx/src/Math
// Comment:
//   made DLL link work in VC++
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Vector3D.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Vector.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Vector.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
