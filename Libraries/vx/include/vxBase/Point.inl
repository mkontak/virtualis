// $Id: Point.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: [Name] [e-mail]
//
// Complete history at bottom of file.


// moved all code to Point3D


// $Log: Point.inl,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 11:43:47  michael
// code review
//
// Revision 3.8  2003/01/22 21:00:54  ingmar
// moved all code to Point3D, Point is now a typedef for Point3D
//
// Revision 3.7  2001/01/02 18:55:00  ingmar
// made copy constructor explicit
//
// Revision 3.6  2002/03/15 03:31:46  jmeade
// Throw exception on read fail.
//
// Revision 3.5  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.4  2002/02/26 18:48:13  jaddonisio
// New distance functions.
//
// Revision 3.3  2002/02/15 14:47:32  jaddonisio
// Moved the [] operator to the Triple class.
//
// Revision 3.2  2001/12/13 15:56:31  michael
// fixed bug in inl file, sorry. :-(
//
// Revision 3.1  2001/12/11 16:29:04  michael
// added opertor[] to Point and Vector class, simply because it is more
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
// Revision 1.1  2001/09/01 00:09:40  soeren
// renamed math directory
//
// Revision 1.15  2001/06/01 21:50:02  liwei
// vector renamed to vec to resolve name confilicts of STL vector
//
// Revision 1.14  2001/05/31 21:32:28  ana
// changes regarding code review
//
// Revision 1.13  2001/05/08 18:21:35  ana
// Modifications after code review
//
// Revision 1.12  2001/05/01 23:03:47  ana
// code standards
//
// Revision 1.11  2001/04/30 13:59:24  ana
// changes to fit in code standards
//
// Revision 1.10  2001/04/24 23:46:33  ana
// modifications to fit in code standards
//
// Revision 1.9  2001/04/16 13:51:38  ana
// added TestByType method
//
// Revision 1.8  2001/04/09 18:03:20  dmitry
// Removed Revision History from Top of File
//
// Revision 1.7  2001/03/28 19:01:00  dmitry
// Updated datatypes, implemented proper casting.
//
// Revision 1.6  2001/02/01 23:55:36  lihong
// fixed a bug for operator >>
//
// Revision 1.5  2000/10/30 17:04:17  frank
// added consts
//
// Revision 1.4  2000/10/28 02:03:34  ingmar
// put EucledianDistance back
//
// Revision 1.2  2000/10/12 02:44:32  kevin
// Added Euclidean Distance
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 7     8/17/00 1:19p Antonio
// modified header
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/10/00    Time: 4:25p
// Updated in $/vx/src/Math
// Modified header to newer version.
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/02/00    Time: 9:29p
// Updated in $/vx/src/Math
// added complete history to end of file
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/01/00  Time:  3:09p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 3  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point, Normal, Vector
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
// Created Point.inl
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Point.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Point.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
