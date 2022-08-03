// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Timer.h,v 1.5.2.1 2008/12/05 17:38:21 gdavidson Exp $
//
// Copyright 2000,2001 Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Ingmar Bitter (ingmar@viatronix.com)



#pragma once


// class definition
class VX_BASE_DLL Timer
{
// functions
public:

  /// constructor
  Timer ();

  /// reset the timer to zero and activate at the same time
  Timer & Reset();

  /// stop the timer (and freeze the elapsed time)
  Timer & Stop();

  /// continue the timer from where Stop() had frozen it
  Timer & Continue();  

  /// return the currently elapsed time
  float8 ElapsedTime();

  /// set the currently elapsed time
  void SetElapsedTime(const float8 & elapsedTime);

  /// returns the elapsed time as a string.
  std::string AsString();

  /// returns the elapsed time as a string.
  std::string AsString2();

  /// output to stream
  friend std::ostream & operator<<(std::ostream & os, Timer & timer);

  /// get seconds per clock tick
  inline static double GetFrequency();
  
	/// get seconds per clock tick
  static float8 GetSecondsPerClockTick();

protected:

  /// read the system time and then update m_elapsedTime accordingly
  inline void UpdateTimes();

// data
protected:

  /// true if the timer is running
  bool m_bTimerIsRunning;

  /// absolute time the timer was created or restarted
  unsigned __int64 m_iStartTime;

  /// relative time since the timer was created or restarted
  unsigned __int64 m_iElapsedTime;

  /// precision of the system clock
  static const float8 m_fSecondsPerTick;

}; // Timer


#include "Timer.inl"


// $Log: Timer.h,v $
// Revision 1.5.2.1  2008/12/05 17:38:21  gdavidson
// Using QueryPerformanceCounter instead of __rdtsc()
//
// Revision 1.5  2007/11/03 00:02:37  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.4.2.1  2007/10/22 23:21:58  jmeade
// typos.
//
// Revision 1.4  2006/07/07 18:00:12  geconomos
// using compiler intrinsic __rdtsc() instead of using inline assembly
//
// Revision 1.3  2006/04/11 13:30:46  romy
// fixed elapsed time issue
//
// Revision 1.2  2005/09/23 14:33:26  geconomos
// added AsString2
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.10  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.9  2002/12/20 19:01:15  ingmar
// removed single scalar constructor from PixelRGBA
//
// Revision 3.8  2002/09/25 20:26:57  ingmar
// now using __int64  instead of float8 to keep the DRTSC register value
//
// Revision 3.7  2002/07/02 21:24:34  soeren
// moved operator << to .h file
//
// Revision 3.6  2002/07/02 20:44:12  geconomos
// Exporting classes .
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
// Revision 3.2  2002/04/04 19:28:23  rekha
// changed retieval of clockticks to use of RDTSC, calibrated through Registry setting of processor clock speed
//
// Revision 3.1  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Sep 29 2001 12:04:52   ingmar
// code beautify
// 
//    Rev 2.2   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.1   Sep 17 2001 00:50:12   ingmar
// added comments to member variables
// 
//    Rev 2.0   Sep 16 2001 23:46:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:42   ingmar
// Initial revision.
// Revision 1.2  2001/09/16 20:33:21  ingmar
// code beautify
//
// Revision 1.1  2001/09/01 00:08:31  soeren
// renamed utils directory
//
// Revision 1.12  2001/08/22 23:23:56  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.11  2001/08/14 18:13:14  lihong
// updated AsString()
//
// Revision 1.10  2001/07/03 14:56:49  soeren
// removed stdafx.c and .h
//
// Revision 1.9  2001/05/18 20:36:54  ingmar
// comments
//
// Revision 1.8  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.7  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.6  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.5  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.4  2001/01/18 16:16:37  dave
// thought about changing to std::clock(), didn't since already works
//
// Revision 1.3  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/19 20:05:17  ingmar
// generic makefile with full optimizations
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 7     8/21/00 12:40a Sarang
// modified header
// 
// *****************  Version 6  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Utils
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/11/00    Time: 12:00p
// Updated in $/vx/src/Utils
// updated header of file
// 
// *****************  Version 4  *****************
// User: Kevin        Date: 8/04/00    Time: 4:47p
// Updated in $/Viatron1000/src/Utils
// Added SetElapsedTime function
// 
// *****************  Version 3  *****************
// User: Antonio      Date: 8/02/00    Time: 10:29p
// Updated in $/vx/src/Utils
// added header and complete history to end of file  
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/18/00  Time: 10:40a
// Checked in $/vx/src/Utils
// Comment:
//   made Test bool
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Timer.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Timer.h,v 1.5.2.1 2008/12/05 17:38:21 gdavidson Exp $
