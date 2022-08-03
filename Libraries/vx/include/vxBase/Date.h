// $Id: Date.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: David (dave@viatronix.com)

/* 
  Simple date-time class. 
  Suitable for storing a date but not comparisons, or computation with date.
  Darn complexity of the Gregorian calendar makes robust/accurate comparison difficult.
  Note QT offers corresponding classes w/greater functionality.
*/

#ifndef Date_h
#define Date_h


namespace vx 
{

// class definition
class VX_BASE_DLL Date 
{
// functions
public:

  /// default constructor, sets the date by the system clock
  Date();

  /// constructor
  Date(const uint4 uYear, const uint1 uMonth, const uint1 uDay,
       const uint1 uHour = 0, const uint1 uMin = 0, const float8 fSeconds = 0);

  /// constructor, initiates class with date and time string ex: mm/dd/yyyy hh:nn:ss
  Date(const std::string & sDate);

  ///////////////////////////////////////////////////
  /// WIN32 Specified Date Methods/Constructors
  ///////////////////////////////////////////////////
#ifdef _WIN32

  /// constructor creating a Date object using the SYSTEMTIME
  Date(const SYSTEMTIME & sysTime);

  /// constructor creating a DATE object using a DOS DATE/TIME 
  Date(const WORD wDosDate, const WORD wDosTime);

  /// constructor creating a DATE object using the FILETIME
  Date(const _FILETIME & fileTime);

  /// initializes the date class using  a SYSTEMTIME 
  void SetDateTime(const SYSTEMTIME & sysTime);

  /// initializes the date classes date portion using  a SYSTEMTIME 
  void SetDate(const SYSTEMTIME & sysTime);

  /// initializes the date classes time portion using  a SYSTEMTIME 
  void SetTime(const SYSTEMTIME & sysTime);

  /// initializes the date class using  a FILETIME 
  void SetDateTime(const _FILETIME & fileTime);

  /// initializes the date classes date portion using   FILETIME 
  void SetDate(const _FILETIME & fileTime);

  /// initializes the date classes time portion using a FILETIME 
  void SetTime(const _FILETIME & fileTime);

  /// initializes the date calss using  a DOS date/time
  void SetDateTime(const WORD wDosDate, const WORD wDosTime);

  /// initializes the date clases date portion using  using  a DOS date/time 
  void SetDate(const WORD wDosDate, const WORD wDosTime);

  /// initializes the date clases time portion using  using  a DOS date/time 
  void SetTime(const WORD wDosDate, const WORD wDosTime);

  /// returns the DOS DATE and DOS Time in the parameter fields
  void GetDosDateTime(WORD * pDosDate, WORD * pDosTime) const;

  /// returns the DOS Date 
  const WORD GetDosDate() const;

  /// returns the DOS time
  const WORD GetDosTime() const;

#endif

  /// set the date via string, ex: yyyy-mm-dd, yyyy.mm.dd, or mm/dd/yyyy
  void SetDate(const std::string &sDate);

  /// set the date from a typical DICOM formatted date string.
  void SetDateFromDicom(const std::string & sDate);

  /// set the time via string
  void SetTime(const std::string & sTime);
  
  /// returns a date object
  static Date GetCurrentDate();

  /// converts to std::string. YYYY.MM.DD hh.mm.ss.sss
  const std::string AsString() const;

  /// converts to std::string. YYYY_MM_DD
  const std::string DateAsString() const;
  
  /// returns the date string ex: 2000-11-08
  const std::string GetDate() const;

  /// returns the date string ex: YYYYMMDD
  const std::string GetPackedDate() const;

  /// returns the time string ex: 23:56:59
  const std::string GetTime() const;

  /// returns the time string ex: HHMMSS
  const std::string GetPackedTime() const;

  /// returns the date and time string ex: 2000-11-08 23:56:59
  const std::string GetDateTime() const;

  /// returns the year and month ex: yyyy-mm
  const std::string GetYearMonth() const;
  
  /// returns year
  const uint4 & GetYear() const;

  /// set year
  void SetYear(const uint4 & uYear);

  /// returns month
  const uint1 & GetMonth() const;
  
  /// sets month
  void SetMonth(const uint1 & uMonth);

  /// returns day
  const uint1 & GetDay() const;
  
  /// sets day
  void SetDay(const uint1 & uDay);

  /// returns hour
  const uint1 & GetHour() const;

  /// sets hour
  void SetHour(const uint1 & uHour);

  /// returns minute
  const uint1 & GetMin() const;

  /// sets minute
  void SetMin(const uint1 & uMin);

  /// returns second
  const float8 & GetSec() const;

  /// sets second
  void SetSec(const float8 & fSeconds);

  /// returns false if date has the invalid value "0000-00-00", true otherwise
  bool IsValid() const;

  /// determines if the year is a leap year
  bool IsLeapYear() const
  {
    return((((m_uYear % 4) == 0 && ((m_uYear % 100) != 0 || (m_uYear % 400) == 0)) ? true : false));
  };

  /// returns the day of the 0 - 365 (366 for leap year).
  const uint4 GetDayOfTheYear() const;
  
  /// returns the number of days in the year
  const uint4 NumderOfDaysInYear() const { return((IsLeapYear() ? 366 : 365)); }

  /// returns the Last Day of Month
  const uint4 GetNumberOfDaysInMonth() const;

  /// returns the number of day till the end of the month
  const uint4 DaysTillEndOfMonth() const
  {
    uint4 uAdditive(0);
    if (m_uMonth == 2 && IsLeapYear())
    { 
      uAdditive = 1;
    }

    uint4 uDaysLeft = (m_uNumberOfDaysInMonth[m_uMonth] + uAdditive) - m_uDay;
    
    return(uDaysLeft);
  } // DaysTillEndOfMonth()
  
  /// returns the difference in the specified dates in days.
  static int4 DateDiff(const vx::Date & date1, const vx::Date & date2)
  {
    int4 iDateDiff(0);
    
    if (date1.GetYear() == date2.GetYear())
    {
      iDateDiff = date1.GetDayOfTheYear() - date2.GetDayOfTheYear();
    }
    else
    {
      if (date1.GetYear() > date2.GetYear())
      {
        iDateDiff = (date2.NumderOfDaysInYear() - date2.GetDayOfTheYear()) + date1.GetDayOfTheYear();
        
        for (Date midYears21(date2.GetYear()+1, 1, 1); midYears21.GetYear() < date1.GetYear(); midYears21.SetYear(midYears21.GetYear()+1))
        {
          iDateDiff += midYears21.NumderOfDaysInYear();
        }
      }
      else
      {
        iDateDiff =  -1 * ((date1.NumderOfDaysInYear() - date1.GetDayOfTheYear()) + date2.GetDayOfTheYear());
        
        for (Date midYears12(date1.GetYear()+1, 1, 1); midYears12.GetYear() < date2.GetYear(); midYears12.SetYear(midYears12.GetYear()+1))
        {
          iDateDiff += midYears12.NumderOfDaysInYear();
        }
      }
    }

    return(iDateDiff);
  } // DateDiff()

  /// operator+
  int4 operator+(const int4 & iDays) const
  {
     Date baseDate(1970, 1, 1);
     return (DateDiff(*this, baseDate) + iDays);
  }

  /// operator-
  int4 operator-(const int4 & iDays) const
  {
    Date baseDate(1970, 1, 1);
    return (DateDiff(*this, baseDate) - iDays);
  }

  /// operator*
  int4 operator*(const int4 & iDays) const
  {
    Date baseDate(1970, 1, 1);
    return (DateDiff(*this, baseDate) * iDays);
  }

  /// operator<
  bool operator<(const int4 & iDate) const
  {
    Date baseDate(1970, 1, 1);
    return (DateDiff(*this, baseDate) < iDate);
  }

  /// operator-
  int4 operator-(const vx::Date & other) const
  {
    return DateDiff(*this, other);
  }
  
    /// operator<
  bool operator<(const Date & other) const
  {
    return (DateDiff(*this, other) < 0);
  }

  /// operator>
  bool operator>(const vx::Date & other) const
  {
    return (DateDiff(*this, other) > 0);
  }

  /// operator<<
  friend std::ostream & operator <<(std::ostream & os, const vx::Date & date)
  {
    os << date.GetDate().c_str();
    return os;
  }

private:

  /// converts mm/dd/yyyy into yyyy-mm-dd
  void StandardDate(std::string & sDate);

// data
protected:

  /// year
  uint4 m_uYear;

  /// month
  uint1 m_uMonth;

  /// day
  uint1 m_uDay;

  /// hour
  uint1 m_uHour;

  /// minutes
  uint1 m_uMin;

  /// seconds
  float8 m_fSeconds;

private:

  /// month start day
  static const uint4 m_uMonthStartDay[];

  /// number of days in month
  static const uint4 m_uNumberOfDaysInMonth[];

  /// last day og month
  static const uint4 m_uLastDayOfMonth[];
}; // Date

};  // namespace vx

#endif // Date_h


// $Log: Date.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.15  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.14.2.1  2003/08/11 11:06:53  mkontak
// moved Date into vx namespace
//
// Revision 3.14  2003/05/13 19:09:07  michael
// code review
//
// Revision 3.13  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.12  2003/03/14 17:02:59  mkontak
// Changed FILETIME to _FILETIME
//
// Revision 3.11  2003/03/12 16:12:57  geconomos
// Removed GetOleDate
//
// Revision 3.10  2002/10/16 16:38:17  frank
// Recovered the date from DICOM style formatted string.
//
// Revision 3.9  2002/10/04 15:43:03  jmeade
// Coding standards: Lots of missed const's for parameters and method declarations.
//
// Revision 3.8  2002/07/02 20:44:12  geconomos
// Exporting classes .
//
// Revision 3.7  2002/05/09 21:01:09  mkontak
// Added GetPackedDate() and GetPackedTime().
//
// Revision 3.6  2002/04/11 20:05:00  mkontak
// Expanded functionality to include WINDOWS dates
//
// Revision 3.5  2002/04/02 15:56:05  mkontak
// Added GetOleDate() to return a date as a double
//
// Revision 3.4  2001/12/29 00:10:23  jmeade
// Merge from 1.1 branch.
//
// Revision 3.3  2001/12/28 19:46:08  dmitry
// Made some functions const. Added some operators for the RendererGraph class. Modified the DateDiff to give the difference in days over years.
//
// Revision 3.2  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.1  2001/11/09 01:14:48  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.0.2.4  2001/11/09 14:53:42  mkontak
// Fixed IsLeapYear() method
//
// Revision 3.0.2.3  2001/11/05 17:26:23  mkontak
// Issue 1325: Fised DateDiff() method.
//
// Revision 3.0.2.2  2001/11/05 16:07:41  mkontak
// Issue 1321: Added GetNumberOfDaysInMonth() method.
//
// Revision 3.0.2.1  2001/10/25 20:24:05  mkontak
// Issue 1063: Added additional date utilities for calcultaing date differences.
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Sep 28 2001 16:29:12   jmeade
// Encapsulate "is valid" check in a function, rather than == "0000-00-00" all over our code
// 
//    Rev 2.1   Sep 19 2001 14:29:54   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:30   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.26  2001/08/02 23:10:49  soeren
// added DateAsString function
//
// Revision 1.25  2001/07/16 23:20:21  jmeade
// unnecessary includes in header files moved to implementation files
//
// Revision 1.24  2001/07/13 16:35:32  frank
// No sense returning a temporary...
//
// Revision 1.23  2001/07/10 21:21:55  jmeade
// one unnecessary const? for GetCurrentDate()
//
// Revision 1.22  2001/07/10 18:20:29  jmeade
// consts and &'s where appropriate and as per coding conventions
//
// Revision 1.21  2001/06/11 16:04:29  dmitry
// Generic constructor w/ a string param added.
//
// Revision 1.20  2001/06/05 15:22:41  dmitry
// GetYearMonth method added, returns "yyyy-mm"
// and SetDBDate (ex: "yyyy-mm-dd")
//
// Revision 1.19  2001/05/30 14:27:24  jmeade
// Default parameters for time in constructor
//
// Revision 1.18  2001/05/28 00:51:22  dmitry
// Changed constructor for database return values.
//
// Revision 1.17  2001/05/27 22:04:35  dmitry
// Modified the Date(std::string) constructor and added the SetDate, SetTime methods.
//
// Revision 1.16  2001/05/24 21:16:04  geconomos
// changed asDatabaseDate() to GetDateTime()
//
// Revision 1.15  2001/05/23 18:47:36  dave
// added GetDate(), GetTime()
//
// Revision 1.14  2001/05/21 19:13:06  geconomos
// Added asDatabaseString()
//
// Revision 1.13  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.12  2001/05/03 20:44:35  dave
// added const to Get functions
//
// Revision 1.11  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.10  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.9  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.8  2001/03/20 18:32:17  dmitry
// Updated datatypes to defs. in Typedef.h
// Added constructor to handle strings, and both string and char *
// constructors use SetDate protected method
//
// Revision 1.7  2001/03/20 12:40:42  frank
// The use of reinterpret_cast is contraindicated.
//
// Revision 1.6  2001/03/19 18:54:03  dongqing
// Updated datatypes as defined in Typedef.h
// Added another constructor to avoid error from v2kGui
//
// Revision 1.5  2000/12/14 05:26:37  dave
// varMap string corrected
//
// Revision 1.4  2000/11/15 05:11:23  dave
// major revision of API and internals
//
// Revision 1.3  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2.2.1  2000/09/25 18:35:32  kevin
// Made Static Libraries
//
// Revision 1.2  2000/09/19 20:05:17  ingmar
// generic makefile with full optimizations
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// 
// 9     8/29/00 12:23p Antonio
// updated header and footer
// 
// 8     8/24/00 9:50p Sarang
// Changes for linux/gcc compilation
// 
// 7     8/24/00 3:01p Jay
// system date/time for linux included
// 
// 6     8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
// 
// 5     8/17/00 10:47p Jay
// 
// 4     8/17/00 2:13p Antonio
// added header and footer.
// 
// *****************  Version 3  *****************
// User: Jay             Date:  8/17/00  Time:  1:03p
// Checked in $/v2k/src/Utils
// Comment:
//   Date string reformatted
// 
// *****************  Version 2  *****************
// User: Jay             Date:  8/17/00  Time:  5:17a
// Checked in $/v2k/src/Utils
// Comment:
//   Class Date added
// 
// *****************  Version 1  *****************
// User: Jay             Date:  8/16/00  Time: 10:29p
// Created Date.h
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Date.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Date.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
