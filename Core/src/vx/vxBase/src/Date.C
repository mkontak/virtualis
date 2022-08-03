// $Id: Date.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: David (dave@viatronix.com)

#include "stdafx.h"
#include "Date.h"
#include "Exception.h"

using namespace std;
using namespace vx;


const uint4 Date::m_uMonthStartDay[] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
const uint4 Date::m_uNumberOfDaysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define FILE_REVISION "$REVISION 3.0$"


/**
 * Default Constructor.
 */
Date::Date()
{
  vx::Date dt    = GetCurrentDate();
  m_uYear    = dt.m_uYear;
  m_uMonth   = dt.m_uMonth;
  m_uDay     = dt.m_uDay;
  m_uHour    = dt.m_uHour;
  m_uMin     = dt.m_uMin;
  m_fSeconds = dt.m_fSeconds;
} // Constructor


/**
 * Constructor.
 */
Date::Date(uint4 uYear, uint1 uMonth, uint1 uDay, uint1 uHour, uint1 uMin, float8 fSeconds) : 
      m_uYear(uYear), m_uMonth(uMonth), m_uDay(uDay), m_uHour(uHour), m_uMin(uMin), m_fSeconds(fSeconds) 
{
}


/**
 * Creates a date class from a string formated like mm/dd/yyyy hh:nn:ss
 */
Date::Date(const std::string & sDate)
{
  std::istrstream date_time(sDate.c_str());
  std::string sSection;
  date_time >> sSection;
  SetDate(sSection);
  date_time >> sSection;
  SetTime(sSection); 
} // String constructor


/////////////////////////////////////////////////////////////////////////////////////////////////
// START ... WINDOWS SPECIFIC DATE CONSTUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

/**
 * Creates a date class from a SYSTEMTIME
 */
Date::Date(const SYSTEMTIME & sysTime)
{
  SetDateTime(sysTime);
}

/**
 * Creates a date class from a FILETIME
 */
Date::Date(const FILETIME & fileTime)
{
  SetDateTime(fileTime);
}

/**
 * Creates a date class from a DOS date/time
 */
Date::Date(const WORD wDosDate, const WORD wDosTime)
{
  SetDateTime(wDosDate, wDosTime);
}

#endif   
/////////////////////////////////////////////////////////////////////////////////////////////////
// END ... WINDOWS SPECIFIC DATE CONSTUCTORS
/////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * Converts mm/dd/yyyy to yyyy-mm-dd.
 */
void Date::StandardDate(std::string & sDate)
{
  switch (sDate.length())
  {
  case 8:  sDate = sDate.substr(4,4)+"-0"+sDate.substr(0,1)+"-0"+sDate.substr(2,1);
           break;
  case 9:  if (sDate[1] == '/')
           sDate = sDate.substr(5,4)+"-0"+sDate.substr(0,1)+"-"+sDate.substr(2,2);
           else 
           sDate = sDate.substr(5,4)+"-0"+sDate.substr(0,2)+"-"+sDate.substr(3,1);
           break;
  case 10: sDate = sDate.substr(6,4)+"-"+sDate.substr(0,2)+"-"+sDate.substr(3,2);
           break;
  default: break;
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
// START ... WINDOWS SPECIFIC DATE METHODS
/////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

/**
 * Initaizlizes a date class from a SYSTEMTIME
 * 
 * @param const SYSTEMTIME & sysTime      System Time to be used as the date and time source
 */
void Date::SetDateTime(const SYSTEMTIME & sysTime)
{
  SetDate(sysTime);
  SetTime(sysTime);
} // SetDateTime()

/**
 * Initaizlizes a date class date portion from a SYSTEMTIME
 * 
 * @param const SYSTEMTIME & sysTime      System Time to be used as the date and time source
 */
void Date::SetDate(const SYSTEMTIME & sysTime)
{
  m_uDay     = (uint1)sysTime.wDay;
  m_uMonth   = (uint1)sysTime.wMonth;
  m_uYear    = (uint4)sysTime.wYear;
} // SetDate()


/**
 * Set the date from a typical DICOM formatted date string.
 */
void Date::SetDateFromDicom( const std::string & sDate )
{
  if( sDate.size() == 8 )
  {
    string sTmp( sDate );
    sTmp.insert(6, ".");
    sTmp.insert(4, ".");
    Date myDate;
    myDate.SetDate( sTmp );
    m_uDay = myDate.m_uDay;
    m_uMonth = myDate.m_uMonth;
    m_uYear = myDate.m_uYear;
  }
  else
  {
    throw ex::IllegalArgumentException( LogRec( "The given DICOM string (\"" + sDate + "\") is not of the expected format.", "Date", "SetDateFromDicom" ) );
  }
} // SetDateFromDicom()


/**
 * Initaizlizes a date class time portion from a SYSTEMTIME
 * 
 * @param const SYSTEMTIME & sysTime     System Time to be used as the date and time source
 */
void Date::SetTime(const SYSTEMTIME & sysTime)
{
  m_uMin     = (uint1)sysTime.wMinute;
  m_uHour    = (uint1)sysTime.wHour;
  m_fSeconds = (float8)sysTime.wSecond;
} // SetTime()


/**
 * Initializes a date class from a FILETIME
 * 
 * @param const FILETIME &  fileTime      File Time to be used as the date and time source
 */
void Date::SetDateTime(const FILETIME & fileTime)
{

  SYSTEMTIME sysTime;

  ///////////////////////////////////////
  // Convert the FILETIME to SYSTEMTIME
  ///////////////////////////////////////
  if ( ! FileTimeToSystemTime(&fileTime, &sysTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert FILETIME : Invalid Data", "Date", "SetDateTime"));
  }

  SetDateTime(sysTime);
} // SetDateTime()

/**
 * Initializes a date classes date portion from a FILETIME
 * 
 * @param const FILETIME &  fileTime      File Time to be used as the date and time source
 */
void Date::SetDate(const FILETIME & fileTime)
{

  SYSTEMTIME sysTime;

  ///////////////////////////////////////
  // Convert the FILETIME to SYSTEMTIME
  ///////////////////////////////////////
  if ( ! FileTimeToSystemTime(&fileTime, &sysTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert FILETIME : Invalid Data", "Date", "SetDate"));
  }

  SetDate(sysTime);
} // SetDate()
  
/**
 * Initializes a date classes time portion from a FILETIME
 * 
 * @param const FILETIME &  fileTime      File Time to be used as the date and time source
 */
void Date::SetTime(const FILETIME & fileTime)
{

  SYSTEMTIME sysTime;

  ///////////////////////////////////////
  // Convert the FILETIME to SYSTEMTIME
  ///////////////////////////////////////
  if ( ! FileTimeToSystemTime(&fileTime, &sysTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert FILETIME : Invalid Data", "Date", "SetTime"));
  }

  SetTime(sysTime);
} // SetTime()

/**
 * Initailzies a date class from a DOS date/time
 *
 * @param const WORD  wDosDate      DOS date value
 * @param const WORD  wDosTime      DOS Time Value
 */
void Date::SetDateTime(const WORD wDosDate, const WORD wDosTime)
{

  FILETIME fileTime;

  /////////////////////////////////////////
  // Convert the DOS Date/Time to FILETIME
  /////////////////////////////////////////
  if ( ! DosDateTimeToFileTime((WORD)wDosDate, (WORD)wDosTime, &fileTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert DOS Date/Time : Invalid Data", "Date", "SetDateTime"));
  }

  try
  {
    SetDateTime(fileTime);
  }
  catch ( ex::VException & e )
  {
    LogErr("Could not convert DOS Date/Time : Invalid Data (" + e.GetLogRecord().GetTextMessage() + ")", "Date", "SetDateTime");
    throw;
  }
} // SetDateTime()

/**
 * Initailzies a date classes date portion from a DOS date/time
 *
 * @param const WORD  wDosDate      DOS date value
 * @param const WORD  wDosTime      DOS Time Value
 */
void Date::SetDate(const WORD wDosDate, const WORD wDosTime)
{

  FILETIME fileTime;
  
  /////////////////////////////////////////
  // Convert the DOS Date/Time to FILETIME
  /////////////////////////////////////////
  if ( ! DosDateTimeToFileTime((WORD)wDosDate, (WORD)wDosTime, &fileTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert DOS Date/Time : Invalid Data", "Date", "SetDate"));
  }

  try
  {
    SetDate(fileTime);
  }
  catch ( ex::VException & e )
  {
    LogErr("Could not convert DOS Date/Time : Invalid Data (" + e.GetLogRecord().GetTextMessage() + ")", "Date", "SetDate");
    throw;
  }
} // SetDate()

/**
 * Initailzies a date classes date portion from a DOS date/time
 *
 * @param const WORD  wDosDate      DOS date value
 * @param const WORD  wDosTime      DOS Time Value
 */
void Date::SetTime(const WORD wDosDate, const WORD wDosTime)
{

  FILETIME fileTime;


  /////////////////////////////////////////
  // Convert the DOS Date/Time to FILETIME
  /////////////////////////////////////////
  if ( ! DosDateTimeToFileTime((WORD)wDosDate, (WORD)wDosTime, &fileTime) )
  {
    throw ex::InvalidDataException(LogRec("Could not convert DOS Date/Time : Invalid Data", "Date", "SetTime"));
  }

  try
  {
     SetTime(fileTime);
  }
  catch ( ex::VException & e )
  {
    LogErr("Could not convert DOS Date/Time : Invalid Data (" + e.GetLogRecord().GetTextMessage() + ")", "Date", "SetTime");
    throw;
  }

} // SetTime()

/**
 * Returns the date back as a DOS DATE and TIME
 *
 * @param WORD * pDosDate     Will contain the dos's date value
 * @param WORD * pDosTime     Will contain the dos's time value
 */
void Date::GetDosDateTime(WORD * pDosDate, WORD * pDosTime ) const
{
  *pDosDate = GetDosDate();
  *pDosTime = GetDosTime();
} // GetDosDateTime()

/**
 * Returns the DOS date value
 */
const WORD Date::GetDosDate() const
{
  WORD wDosDate(1970);
  uint2 iYear = m_uYear;

  if ( iYear <= 1980 )
  {
    iYear = 0;
  }
  else
  {
    iYear -= 1980;
    wDosDate = (uint2)( (uint2)m_uDay + ( (uint2)m_uMonth << 5) + (iYear << 9));
  }
  
  return(wDosDate);
} // GetDosDate()

/**
 * Returns the DOS time value
 */
const WORD Date::GetDosTime() const
{
  return((WORD)(((uint2)m_fSeconds >> 1 ) + ((uint2)m_uMin << 5) + ((uint2)m_uHour << 11)));
} // GetDosTime()

#endif
/////////////////////////////////////////////////////////////////////////////////////////////////
// END ... WINDOWS SPECIFIC DATE METHODS
/////////////////////////////////////////////////////////////////////////////////////////////////


/**
 *
 */
void Date::SetDate(const std::string &sDate)
{
  string sNewDate = sDate;
  if (sNewDate.find('/') < sNewDate.max_size())
    StandardDate(sNewDate);
  std::replace(sNewDate.begin(), sNewDate.end(), '.', '-');

  std::istringstream date(sNewDate);
  const int4 bSize = 10;
  int1 buffer[bSize];
  date.getline(buffer, bSize, '-'); m_uYear  = atoi(buffer);
  date.getline(buffer, bSize, '-'); m_uMonth = atoi(buffer);
  date.getline(buffer, bSize, '-'); m_uDay   = atoi(buffer);
} // SetDBDate


/**
 *
 */
void Date::SetTime(const std::string &sTime)
{
  std::istringstream time(sTime);
  const int4 bSize = 10;
  int1 buffer[bSize];
  time.getline(buffer, bSize, ':'); m_uHour    = atoi(buffer);
  time.getline(buffer, bSize, ':'); m_uMin     = atoi(buffer);
  time.getline(buffer, bSize, ':'); m_fSeconds = atoi(buffer);
} // SetTime

/**
 * Returns the current date and time as a Date object.
 * If the date is not available all values are set to zero.
 * @return the date object.
 */
Date Date::GetCurrentDate()
{
  uint4 uYear = 0;
  uint1 uMonth = 0, uDay = 0, uHour = 0, uMin = 0;
  float8 fSeconds = 0;
  
  time_t time_tStruct;

  if(-1 == time(&time_tStruct)) 
  { // time is not available
    // already zero
  }
  else 
  {
    // convert time_t to tm 
    tm * tmPtr = localtime(&time_tStruct);

    uYear  = tmPtr->tm_year + 1900;
    uMonth = tmPtr->tm_mon + 1;
    uDay   = tmPtr->tm_mday;
    uHour  = tmPtr->tm_hour;
    uMin   = tmPtr->tm_min;
    fSeconds = tmPtr->tm_sec;
  }

  return Date(uYear, uMonth, uDay, uHour, uMin, fSeconds);
} // GetCurrentDate


/**
 * Converts a date object to a string.
 * @return string 
 */
const std::string Date::AsString() const
{
  int1 iBuf[100];
  sprintf(iBuf,"%04u.%02u.%02u %02u:%02u:%04.1f", 
    m_uYear, m_uMonth, m_uDay, m_uHour, m_uMin, m_fSeconds);
  return string(iBuf);
} // AsString()


/**
 * Converts a date object to a string.
 * @return string 
 */
const std::string Date::DateAsString() const
{
  int1 iBuf[100];
  sprintf(iBuf,"%04u_%02u_%02u", m_uYear, m_uMonth, m_uDay);
  return string(iBuf);
} // DateAsString()


/**
 * Converts a date object to a string.
 * @return someStuff
 */
const std::string Date::GetDate() const
{
  int1 iBuf[100];
  sprintf(iBuf,"%04u-%02u-%02u", m_uYear, m_uMonth, m_uDay);
  return string(iBuf);
} // GetDate

/**
 * Converts a date object to a string.
 * @return someStuff
 */
const std::string Date::GetPackedDate() const
{
  int1 iBuf[100];
  sprintf(iBuf,"%04u%02u%02u", m_uYear, m_uMonth, m_uDay);
  return string(iBuf);
} // GetPackedDate


/**
 *
 */
const string Date::GetTime() const
{
  ostringstream time;
  time << setw(2) << setfill('0') << (int)m_uHour<<":" << setw(2)<< setfill('0') << (int)m_uMin << ":" << setw(2) << setfill('0') << int4(m_fSeconds);
  return string(time.str());
} // GetTime

/**
 *
 */
const string Date::GetPackedTime() const
{
  ostringstream time;
  time << setw(2) << setfill('0') << (int)m_uHour << setw(2)<< setfill('0') << (int)m_uMin << setw(2) << setfill('0') << m_fSeconds;
  return string(time.str());
} // GetPackedTime

/**
 *
 */
const std::string Date::GetDateTime() const
{
  return GetDate() + " " + GetTime();  
} // GetDateTime


/**
 *
 */
const std::string Date::GetYearMonth() const
{
  int1 iBuf[100];
  sprintf(iBuf,"%04u-%02u", m_uYear, m_uMonth);
  return string(iBuf);
} // GetYearMonth


/**
 *
 */
const uint4 & Date::GetYear() const
{
  return m_uYear;
}


/**
 *
 */
void Date::SetYear(const uint4 & uYear)
{
  m_uYear = uYear;
}


/**
 *
 */
const uint1 & Date::GetMonth() const
{
  return m_uMonth;
}


/**
 *
 */
void Date::SetMonth(const uint1 & uMonth)
{
  m_uMonth = uMonth;
}


/**
 *
 */
const uint1 & Date::GetDay() const
{
  return m_uDay;
}


/**
 *
 */
void Date::SetDay(const uint1 & uDay)
{
  m_uDay = uDay;
}


/**
 *
 */
const uint1 & Date::GetHour() const
{
  return m_uHour;
}


/**
 *
 */
void Date::SetHour(const uint1 & uHour)
{
  m_uHour = uHour;
}


/**
 *
 */
const uint1 & Date::GetMin() const
{
  return m_uMin;
}


/**
 *
 */
void Date::SetMin(const uint1 & uMin)
{
  m_uMin = uMin;
}


/**
 *
 */
const float8 & Date::GetSec() const
{
  return m_fSeconds;
}


/**
 *
 */
void Date::SetSec(const float8 & fSeconds)
{
  m_fSeconds = fSeconds;
}

/**
 * returns the day of the year 0 - 365.
 */
const uint4 Date::GetDayOfTheYear() const 
{
  uint4 uAdditive = 0;
  
  if ( IsLeapYear() && m_uMonth > 1 )
    uAdditive = 1;
  
  uint4 uDayOfTheYear = m_uMonthStartDay[m_uMonth] + m_uDay;
  
  return(uDayOfTheYear);
}

/**
 * Returns the number of days in the currentmonth
 */
const uint4 Date::GetNumberOfDaysInMonth() const 
{
  
  uint4 uNumberOfDays(0);
  
  // Maske sure a valid month is specified
  if ( m_uMonth >= 1 && m_uMonth <= 12 )
  {
    uNumberOfDays = m_uNumberOfDaysInMonth[m_uMonth];
    
    // Acoount for leap year
    if ( m_uMonth == 2 && IsLeapYear() )
    {
      uNumberOfDays++;
    }
  }
  
  return (uNumberOfDays);
  
}



/**
 * Returns false if date has the invalid value "0000-00-00", true otherwise
 *
 */
bool Date::IsValid() const
{
  return (GetDate() != "0000-00-00");
}


#undef FILE_REVISION

// $Log: Date.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.13  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.12.2.1  2003/08/11 11:06:53  mkontak
// moved Date into vx namespace
//
// Revision 3.12  2003/05/16 11:42:40  michael
// code review
//
// Revision 3.11  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.10  2003/03/12 16:12:57  geconomos
// Removed GetOleDate
//
// Revision 3.9  2002/10/16 16:38:17  frank
// Recovered the date from DICOM style formatted string.
//
// Revision 3.8  2002/10/04 15:43:03  jmeade
// Coding standards: Lots of missed const's for parameters and method declarations.
//
// Revision 3.7  2002/05/28 21:07:10  mkontak
// Fixed GetTime() so that it returns the seconds as a whole number
//
// Revision 3.6  2002/05/09 21:01:09  mkontak
// Added GetPackedDate() and GetPackedTime().
//
// Revision 3.5  2002/04/11 20:04:59  mkontak
// Expanded functionality to include WINDOWS dates
//
// Revision 3.4  2002/04/03 15:18:51  ingmar
// removed unused nDaysInMonth variable
//
// Revision 3.3  2002/04/02 15:56:05  mkontak
// Added GetOleDate() to return a date as a double
//
// Revision 3.2  2001/12/28 19:44:50  dmitry
// Made some functions const.
//
// Revision 3.1  2001/11/09 01:14:48  jmeade
// Merge with 1.1 - 2001-11-08
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
//    Rev 2.1   Sep 19 2001 14:29:52   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:28   ingmar
// Initial revision.
// Revision 1.3  2001/09/15 00:34:10  soeren
// changed LogFyi to LogDbg
//
// Revision 1.2  2001/09/05 18:40:15  soeren
// fixed a very stupid bug
//
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.27  2001/08/02 23:10:49  soeren
// added DateAsString function
//
// Revision 1.26  2001/07/16 23:20:21  jmeade
// unnecessary includes in header files moved to implementation files
//
// Revision 1.25  2001/07/13 16:35:32  frank
// No sense returning a temporary...
//
// Revision 1.24  2001/07/10 21:21:55  jmeade
// one unnecessary const? for GetCurrentDate()
//
// Revision 1.23  2001/07/10 18:20:29  jmeade
// consts and &'s where appropriate and as per coding conventions
//
// Revision 1.22  2001/06/14 16:27:47  dmitry
// Fixed bug in StandardDate method.
//
// Revision 1.21  2001/06/11 16:04:29  dmitry
// Generic constructor w/ a string param added.
//
// Revision 1.20  2001/06/05 15:22:41  dmitry
// GetYearMonth method added, returns "yyyy-mm"
// and SetDBDate (ex: "yyyy-mm-dd")
//
// Revision 1.19  2001/05/28 18:18:49  dave
// added setfill
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
// Revision 1.11  2001/04/09 16:25:34  dave
// removed some uses of uint, uchar, etc
//
// Revision 1.10  2001/04/05 17:59:53  dmitry
// updated datatypes
//
// Revision 1.9  2001/03/27 19:10:51  dmitry
// Updated more types as def. in Typedef.h
//
// Revision 1.8  2001/03/20 18:32:18  dmitry
// Updated datatypes to defs. in Typedef.h
// Added constructor to handle strings, and both string and char *
// constructors use SetDate protected method
//
// Revision 1.7  2001/03/20 12:40:46  frank
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
// Revision 1.3  2000/09/26 02:58:13  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
// 
// 14    9/03/00 5:31p Antonio
// deleted char buffer[[128], it was an unused variable
// 
// 13    8/24/00 9:50p Sarang
// Changes for linux/gcc compilation
// 
// 12    8/24/00 3:01p Jay
// system date/time for linux included
// 
// 11    8/20/00 6:49p Sarang
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
// 
// 10    8/18/00 9:08a Dave
// date output beautify
// 
// 9     8/18/00 8:56a Dave
// date output beautify
// 
// 8     8/17/00 10:47p Jay
// 
// 7     8/17/00 10:12p Jay
// 
// 6     8/17/00 2:11p Antonio
// added header and footer
// 
// *****************  Version 5  *****************
// User: Jay             Date:  8/17/00  Time:  1:51p
// Checked in $/v2k/src/Utils
// 
// *****************  Version 4  *****************
// User: Jay             Date:  8/17/00  Time:  1:42p
// Checked in $/v2k/src/Utils
// Comment:
//   Date field tested to work
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
// User: Jay             Date:  8/16/00  Time: 11:41p
// Created Date.C
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/Date.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: Date.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
