// $Id: Point.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com

                        
#ifndef Point_h
#define Point_h


// defines
// allow to still use old class name
#define Point Point3D 


// include declarations 
#include "Point3D.h"


#endif // Point3D_h



// $Log: Point.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:43:46  michael
// code review
//
// Revision 3.8  2003/01/22 21:00:02  ingmar
// moved all code to Point3D, Point is now a typedef for Point3D
//
// Revision 3.7  2001/01/02 18:55:00  ingmar
// made copy constructor explicit
//
// Revision 3.6  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.5  2002/02/26 18:48:08  jaddonisio
// New distance functions.
//
// Revision 3.4  2002/02/15 14:47:32  jaddonisio
// Moved the [] operator to the Triple class.
//
// Revision 3.3  2001/12/20 22:32:35  ingmar
// added class Sec_10_math to structure the doxygen documentation
//
// Revision 3.2  2001/12/13 15:56:31  michael
// fixed bug in inl file, sorry. :-(
//
// Revision 3.1  2001/12/11 16:29:04  michael
// added opertor[] to Point3D and Vector class, simply because it is more
// convenient to use in loops ... but might be less efficient ...
//
// Revision 3.0  2001/10/14 23:02:17  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:33:16   liwei
// No change. Checked out by error.
// 
//    Rev 2.0   Sep 16 2001 23:46:52   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:12   ingmar
// Initial revision.
// Revision 1.2  2001/09/07 14:17:22  liwei
// The third argument defaults to 0 for the constructor of Point3D.
//
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.16  2001/08/23 17:50:11  sarang
// linux/gcc fix.
//
// Revision 1.15  2001/05/31 21:32:29  ana
// changes regarding code review
//
// Revision 1.14  2001/05/08 18:21:30  ana
// Modifications after code review
//
// Revision 1.13  2001/05/01 23:03:42  ana
// code standards
//
// Revision 1.12  2001/04/30 13:59:24  ana
// changes to fit in code standards
//
// Revision 1.11  2001/04/17 23:34:21  ana
// added heading comments
//
// Revision 1.10  2001/04/16 13:51:38  ana
// added TestByType method
//
// Revision 1.9  2001/04/09 18:03:20  dmitry
// Removed Revision History from Top of File
//
// Revision 1.8  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.7  2001/02/01 23:55:36  lihong
// fixed a bug for operator >>
//
// Revision 1.6  2000/10/30 17:04:17  frank
// added consts
//
// Revision 1.5  2000/10/28 02:03:34  ingmar
// put EucledianDistance back
//
// Revision 1.3  2000/10/12 02:44:32  kevin
// Added Euclidean Distance
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 7     8/17/00 1:11p Antonio
// modified header
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/10/00    Time: 4:23p
// Updated in $/vx/src/Math
// modified header to newer version.
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/02/00    Time: 9:26p
// Updated in $/vx/src/Math
// added complete history to end of file
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/01/00  Time:  2:39p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 3  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point3D, Normal, Vector
// Since no virtual functions are desired, the same fn. appears in all despite
// inheritance
// 
// *****************  Version 2  *****************
// User: Frank           Date:  7/18/00  Time:  3:55p
// Checked in $/Viatron1000/src/Math
// Comment:
//   Removed "using namespace *" from header files to prevent ambiguities
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Point3D.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Point.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Point.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
