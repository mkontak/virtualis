// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Timer.C,v 1.3.8.1 2008/12/05 17:38:21 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "Timer.h"


// defines
#define FILE_REVISION "$Revision: 1.3.8.1 $"


// namespaces
using namespace std;


// static member initializations
const float8 Timer::m_fSecondsPerTick(Timer::GetSecondsPerClockTick());

/**
 * Default constructor
 */
Timer::Timer ()
//*******************************************************************
{
  Reset();
} // default constructor


/**
 * Get processor spped from windows registry and set fSecondsPerTick accordingly.
 * This is for V3D-Colon viewer codes only
 * @return seconds per clock tick
 */
float8 Timer::GetSecondsPerClockTick()
//*******************************************************************
{
  static float8 fSecondsPerTick(0);

  if (fSecondsPerTick == 0)
  {
    // get CC from registry
    int4 iClockCyclesPerMicroSecond(0);
    HKEY hkSub;
    ULONG ulBuffSizeInBytes = sizeof(int4);
    RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", &hkSub);
    DWORD dwType = REG_DWORD;
    RegQueryValueEx(hkSub,"~MHz", NULL, &dwType, reinterpret_cast<LPBYTE>(&iClockCyclesPerMicroSecond), &ulBuffSizeInBytes);
    int8 i8HiResClockCyclesPerMicroSecond(iClockCyclesPerMicroSecond);

    fSecondsPerTick = 1.0L / (i8HiResClockCyclesPerMicroSecond * 1000000i64); // ??? MHz is really 1,000,000 not 1204^2 since it relates to analog signal processing long before power-of-two computers
  }

  return fSecondsPerTick;
} // GetSecondsPerClockTick()


/**
 * Reset the timer (elapsed time is reset to zero)
 * @return reference to the timer object.
 */
Timer & Timer::Reset()
//*******************************************************************
{
  m_iStartTime = 0;
  UpdateTimes();
  m_iStartTime = m_iElapsedTime; 
  m_bTimerIsRunning = true;

  return *this;
} // Reset()


/**
 * Stop the timer (elapsed time is frozen)
 * @return reference to the timer object.
 */
Timer & Timer::Stop()
//*******************************************************************
{
  if (m_bTimerIsRunning == true)
  {
    UpdateTimes();
    m_bTimerIsRunning = false;
  }
  return *this;
} // Stop()


/**
 * Return the currently elapsed time
 * @return elapsed time as float8
 */
float8 Timer::ElapsedTime()
//*******************************************************************
{
  if (m_bTimerIsRunning == true)
  {
    UpdateTimes();
  }
  float8 fElapsedSecs( m_iElapsedTime * GetFrequency() ); 
  if( fElapsedSecs < 0.0L )
  {
    LogWrn("Elapsed Time calculated as Negative!!!!!","Timer","ElapsedTime");
    fElapsedSecs = -fElapsedSecs;
  }
  return fElapsedSecs;
} // ElapsedTime()


/**
 * Continue the timer (elapsed time is continues).
 * @return reference to the timer object.
 */
Timer & Timer::Continue()
//*******************************************************************
{
  if (m_bTimerIsRunning == false)
  {
    unsigned __int64 tmp = m_iElapsedTime;
    UpdateTimes();
    m_iStartTime += m_iElapsedTime - tmp;
    m_bTimerIsRunning = true;
  }
  return(*this); 
} // Continue()


/**
 * Set the currently elapsed time.
 * @param time you want the timer to contiue from.
 */
void Timer::SetElapsedTime(const float8 & elapsedTime)
//*******************************************************************
{
  if (m_bTimerIsRunning == true)
  {
    UpdateTimes();
  }
  m_iStartTime += m_iElapsedTime - static_cast< unsigned __int64 >( elapsedTime / GetFrequency() );
} // SetElapsedTime()


/**
 * Returns the elapsed time as a string, formatted as Hh MMm SS.SSSs
 * @return the string with the elapsed time.
 */
std::string Timer::AsString() 
  //*******************************************************************
{
  if (m_bTimerIsRunning == true)
  {
    UpdateTimes();
  }

  uint4 uTotalSeconds(static_cast<uint4>(ElapsedTime()));
  uint4 uHours(uTotalSeconds/60/60);
  uint4 uMinutes(uTotalSeconds/60%60);
  float8 fSeconds(ElapsedTime() - uTotalSeconds + uTotalSeconds%60);
  char timeStr[50];
  sprintf(timeStr, "time: %uh %02um %06.3fs", uHours, uMinutes, fabs(fSeconds));

  return( timeStr );
} // AsString()

/**
 * Returns the elapsed time as a string, formatted as Hh MMm SS.SSSs
 * @return the string with the elapsed time.
 */
std::string Timer::AsString2() 
  //*******************************************************************
{
  if (m_bTimerIsRunning == true)
  {
    UpdateTimes();
  }

  uint4 uTotalSeconds(static_cast<uint4>(ElapsedTime()));
  uint4 uHours(uTotalSeconds/60/60);
  uint4 uMinutes(uTotalSeconds/60%60);
  float8 fSeconds(ElapsedTime() - uTotalSeconds + uTotalSeconds%60);
  char timeStr[50];
  sprintf(timeStr, "%uh %02um %06.3fs", uHours, uMinutes, fabs(fSeconds));

  return( timeStr );
} // AsString()


// $Log: Timer.C,v $
// Revision 1.3.8.1  2008/12/05 17:38:21  gdavidson
// Using QueryPerformanceCounter instead of __rdtsc()
//
// Revision 1.3  2006/04/11 13:31:04  romy
// fixed elapsed time issue
//
// Revision 1.2  2005/09/23 14:32:52  geconomos
// added AsString2
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.9  2003/05/16 13:08:20  frank
// code formatting
//
// Revision 3.8  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.7  2002/11/16 19:09:23  kevin
// fabs of returned elaspedTime, are you kidding me???????
// (note the fabs was added when the function was in Timer.inl)
//
// Revision 3.6  2002/11/15 19:47:42  kevin
// The resolution of the timer before was maxed out at 2.1Ghz. now the resolution is sufficent till the year 9999 :)
//
// Revision 3.5  2002/09/25 20:26:57  ingmar
// now using __int64  instead of float8 to keep the DRTSC register value
//
// Revision 3.4  2002/07/02 21:24:34  soeren
// moved operator << to .h file
//
// Revision 3.3  2002/07/02 20:44:12  geconomos
// Exporting classes .
//
// Revision 3.2  2002/04/10 23:25:51  jmeade
// Declared m_iElapsedTime a static member so that class instances will have
//  access to the assignment operator.
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
//    Rev 2.12   Oct 02 2001 19:03:14   ingmar
// beautify commentResolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.11   Oct 02 2001 18:51:30   ingmar
// comment beautify
// 
//    Rev 2.10   Oct 02 2001 18:30:48   ingmar
// code beautify
// 
//    Rev 2.9   Sep 29 2001 12:35:26   ingmar
// associated with issue
// Resolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.8   Sep 29 2001 12:29:20   ingmar
// code beautify
// Resolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.7   Sep 29 2001 12:27:46   ingmar
// code beautify
// 
//    Rev 2.6   Sep 29 2001 12:21:30   ingmar
// code beautify
// Resolution for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.5   Sep 29 2001 12:13:36   ingmar
// code beautify
// 
//    Rev 2.4   Sep 19 2001 14:30:02   liwei
// No change.
// 
//    Rev 2.3   Sep 17 2001 18:12:30   soeren
// fixed timer
// 
//    Rev 2.2   Sep 17 2001 13:00:34   ingmar
// no change
// 
//    Rev 2.1   Sep 17 2001 10:39:16   ingmar
// added header keyword ; for 1014: PVCS VM &Tracker setup test
// 
//    Rev 2.0   Sep 16 2001 23:46:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:42   ingmar
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
// Revision 1.6  2001/08/22 23:23:56  soeren
// made some changes to make it to compile with the intel compiler
//
// Revision 1.5  2001/08/10 19:58:39  soeren
// renamed time to Time
//
// Revision 1.4  2001/05/18 20:36:54  ingmar
// comments
//
// Revision 1.3  2001/04/09 18:02:56  dmitry
// Removed Revision History from Top of File
//
// Revision 1.2  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 8     8/29/00 12:25p Antonio
// updated header
// 
// 7     8/21/00 12:24a Sarang
// modified header
// 
// *****************  Version 6  *****************
// User: Antonio      Date: 8/08/00    Time: 5:35p
// Updated in $/vx/src/Utils
// made small syntax modifications
// 
// *****************  Version 5  *****************
// User: Antonio      Date: 8/02/00    Time: 10:07p
// Updated in $/vx/src/Utils
// added complete history to end of file
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/01/00  Time:  3:21p
// Checked in $/vx/src/Utils
// Comment:
//   added header, history and copyright
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  7/21/00  Time:  2:49p
// Checked in $/vx/src/Utils
// Comment:
//   added test to verify operation of timer functions
// 
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/18/00  Time: 10:40a
// Checked in $/vx/src/Utils
// Comment:
//   made Test bool
// 
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/18/00  Time:  9:02a
// Created Timer.C
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Timer.C,v 1.3.8.1 2008/12/05 17:38:21 gdavidson Exp $
// $Id: Timer.C,v 1.3.8.1 2008/12/05 17:38:21 gdavidson Exp $
