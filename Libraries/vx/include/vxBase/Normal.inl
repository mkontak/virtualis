// $Id: Normal.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000-2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Ingmar ingmar@viatronix.com

// moved all code to Normal3D

// $Log: Normal.inl,v $
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
// Revision 3.14  2003/01/21 20:31:43  frank
// Moved Normalize into inline file.
//
// Revision 3.13  2003/01/03 12:46:24  ingmar
// code beautify
//
// Revision 3.12  2001/01/02 18:50:49  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.11  2001/01/02 16:17:47  ingmar
// no longer derived from Triple
//
// Revision 3.10  2002/10/29 19:35:20  kevin
// Normal read/write is not consistent. SO rather than risk breakign
// something else, I put a LARGE warnign in Normal read and
// fixed the Camera class I/O outside of the Normal elements.
//
// Revision 3.9  2002/05/13 15:42:47  michael
// removed the Vector constructor since it is already in Triple
//
// Revision 3.8  2002/05/13 15:15:26  michael
// Added copy constructor for Normal using Vector3D
//
// Revision 3.7  2002/03/11 17:19:31  ingmar
// rewrote oprator[], now faster and without logger.
//
// Revision 3.6  2002/03/08 13:54:45  michael
// reworked 10_math and gained some compilation time
//
// Revision 3.5  2002/02/26 20:51:17  jaddonisio
// New Dot function.
//
// Revision 3.4  2002/02/26 17:19:55  michael
// declared operator[] to be const
//
// Revision 3.3  2002/02/25 21:19:49  michael
// made Get{Abs}MaxComponent{Coord}() const
//
// Revision 3.2  2002/02/22 17:01:54  michael
// Get...MaxComponentCoord() returns now uint1 (index) and not the template type
//
// Revision 3.1  2002/02/19 16:17:07  jaddonisio
// New methods to get max. component values.
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
//    Rev 2.0   Sep 16 2001 23:46:48   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:34:08   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:09:39  soeren
// renamed math directory
//
// Revision 1.12  2001/08/20 23:43:25  huamin
// Fixed the bug of Cross() function.  An unexpected bug costs me
// a lot of time
//
// Revision 1.11  2001/05/31 21:32:42  ana
// changes regarding code review
//
// Revision 1.10  2001/05/31 11:51:09  frank
// Floating point equality operators were foobar'd.  Needed fabs()!
//
// 
// *****************  Version 9  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Math
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
// 
// *****************  Version 8  *****************
// User: Antonio      Date: 8/17/00    Time: 1:05p
// Updated in $/vx/src/Math
// modified header.
// 
// *****************  Version 7  *****************
// User: Antonio      Date: 8/10/00    Time: 4:22p
// Updated in $/vx/src/Math
// modified header to newer version.
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/02/00    Time: 9:25p
// Updated in $/vx/src/Math
// added complete history to end of file
//
// *****************  Version 5  *****************
// User: Antonio         Date:  8/01/00  Time:  3:07p
// Checked in $/vx/src/Math
// Comment:
//   added header, history and copyright
// 
// *****************  Version 4  *****************
// User: Jay             Date:  7/28/00  Time: 12:01p
// Checked in $/vx/src/Math
// Comment:
//   Added AsString member function to Triple, Point, Normal, Vector
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
// User: Jay             Date:  7/18/00  Time: 12:59p
// Checked in $/vx/src/Math
// Comment:
//   The test functions work for float data types .. comparision of
// floats types for Triples etc are now < 0.0001 
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Normal.inl
// 
// $Id: Normal.inl,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
