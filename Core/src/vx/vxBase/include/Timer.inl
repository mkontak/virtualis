// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Timer.inl,v 1.7.8.1 2008/12/05 17:38:21 gdavidson Exp $
//
// Copyright 2000,2001 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)



// defines
#define FILE_REVISION "$Revision: 1.7.8.1 $"


/**
 * Updates the times
 */
inline void Timer::UpdateTimes()
//*******************************************************************
{
  unsigned __int64 time;
  QueryPerformanceCounter( reinterpret_cast< LARGE_INTEGER * >( &time ));
  m_iElapsedTime = time - m_iStartTime;
} // UpdateTimes()


/**
 * Operator<<
 * @param ostream
 * @param timer
 * @return ostream
 */
inline std::ostream & operator<<(std::ostream & os, Timer & timer)
//*******************************************************************
{
  return os<<timer.AsString();
} // operator <<


/**
 * Get processor spped from windows registry and set fSecondsPerTick accordingly.
 * @return seconds per clock tick
 */
double Timer::GetFrequency()
//*******************************************************************
{
  static double fFrequency( -1.0 );
  if( fFrequency <= 0 )
  {
    unsigned __int64 iFrequency(0);
    if( QueryPerformanceFrequency( reinterpret_cast< LARGE_INTEGER * >( &iFrequency )))
      fFrequency = 1.0 / static_cast< double >( iFrequency );
  }

  return fFrequency;
} // GetFrequency()


#undef FILE_REVISION


// $Log: Timer.inl,v $
// Revision 1.7.8.1  2008/12/05 17:38:21  gdavidson
// Using QueryPerformanceCounter instead of __rdtsc()
//
// Revision 1.7  2006/07/07 18:00:12  geconomos
// using compiler intrinsic __rdtsc() instead of using inline assembly
//
// Revision 1.6  2006/04/11 17:00:13  romy
// fixed elapsed time issue
//
// Revision 1.5  2006/04/11 16:58:15  romy
// fixed elapsed time issue
//
// Revision 1.4  2006/04/11 13:30:35  romy
// fixed elapsed time issue
//
// Revision 1.3  2006/04/10 21:08:52  romy
// temp fix to solve the Negative Elapsed time
//
// Revision 1.2  2006/02/02 20:02:26  geconomos
// added considerations for 64-bit builds
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.8  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.7  2002/09/25 20:26:57  ingmar
// now using __int64  instead of float8 to keep the DRTSC register value
//
// Revision 3.6  2002/05/07 22:16:38  soeren
// added fabs fix for P4 Zeon
//
// Revision 3.5  2002/05/04 17:01:16  soeren
// removed totally wrong function
//
// Revision 3.4  2002/04/27 23:16:27  jaddonisio
// Made a simple time logger.
//
// Revision 3.3  2002/04/10 23:25:51  jmeade
// Declared m_iElapsedTime a static member so that class instances will have
//  access to the assignment operator.
//
// Revision 3.2  2002/04/04 19:42:17  ingmar
// added explicit cast upon compiler request :-)
//
// Revision 3.1  2002/04/04 19:28:23  rekha
// changed retieval of clockticks to use of RDTSC, calibrated through Registry setting of processor clock speed
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.12   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.11   Sep 17 2001 18:29:00   soeren
// changed Time to time
// 
//    Rev 2.10   Sep 17 2001 18:19:06   soeren
// Fixed timer
// 
//    Rev 2.9   Sep 17 2001 13:53:58   ingmar
// code beautify
// 
//    Rev 2.8   Sep 17 2001 13:19:14   ingmar
// Comment beautify ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.5   Sep 17 2001 11:46:56   ingmar
// added function comment ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.4   Sep 17 2001 11:33:32   ingmar
// Id -> Header ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.3   Sep 17 2001 10:47:28   ingmar
// added 201 to copyright ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.2   Sep 17 2001 01:08:24   ingmar
// code beautify ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.1   Sep 17 2001 00:57:20   ingmar
// added comments to some member functions. ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.0   Sep 16 2001 23:46:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:44   ingmar
// Initial revision.
// Revision 1.3  2001/09/16 20:33:21  ingmar
// code beautify
//
// Revision 1.2  2001/09/13 20:50:24  soeren
// clean up RunAllStages a little bit
//
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.10  2001/08/23 00:19:57  soeren
// added inline to AsTReam
//
// Revision 1.9  2001/08/22 23:23:56  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.8  2001/08/14 18:13:14  lihong
// updated AsString()
//
// Revision 1.7  2001/08/11 14:46:02  frank
// Don't forget to inline inline functions.
//
// Revision 1.6  2001/05/18 20:36:54  ingmar
// comments
//
// Revision 1.5  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.4  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.3  2001/02/22 01:49:33  soeren
// repaired timer class, now it spit out the correct timmings under Windows 2000
//
// Revision 1.2  2001/01/18 16:16:37  dave
// thought about changing to std::clock(), didn't since already works
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 6     8/29/00 12:18p Antonio
// updated header
// 
// 5     8/21/00 12:53a Sarang
// modified header
// 
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 12:05p
// Updated in $/vx/src/Utils
// updated header of file
// 
// *****************  Version 3  *****************
// User: Kevin        Date: 8/04/00    Time: 4:47p
// Updated in $/Viatron1000/src/Utils
// Added SetElapsedTime function
// 
// *****************  Version 2  *****************
// User: Antonio      Date: 8/02/00    Time: 10:31p
// Updated in $/vx/src/Utils
// added header and complete history to end of file  
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Timer.inl
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Timer.inl,v 1.7.8.1 2008/12/05 17:38:21 gdavidson Exp $
