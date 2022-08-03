// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmTime .cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmTime.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"

using namespace dcm;


/**
 * Defaiult constructor
 */
DcmTime::DcmTime() :
m_bEmpty(true),
m_iHour(0),
m_iMinutes(0),
m_fSeconds(0.0F)
{
} // DcmTime ()


/**
 * Constructor
 *
 *  @param iHour
 *  @param iMinutes
 *  @param fSeconds
 */
DcmTime::DcmTime(unsigned short iHour, unsigned short iMinutes, float fSeconds)
{
  m_bEmpty = false;
  SetHour(iHour);
  SetMinutes(iMinutes);
  SetSeconds(fSeconds);
} // DcmTime (unsigned short iMonth, unsigned short iDay, unsigned short iYear) 


/**
 * Copy constructor
 *
 *  @param  time
 */
DcmTime::DcmTime(const DcmTime  & time) 
{
  m_bEmpty    = time.m_bEmpty;
  m_iHour     = time.m_iHour;
  m_iMinutes  = time.m_iMinutes;
  m_fSeconds  = time.m_fSeconds;

} // DcmTime (const DcmTime  & time)


/**
 * Constructs the date from the dicom std::string
 *
 * @param sTime     Dicom date
 */
DcmTime::DcmTime(const std::string & sTime) :
m_bEmpty(sTime.empty()),
m_iHour(0),
m_iMinutes(0),
m_fSeconds(0.0F)
{
  Assign(sTime);
} // DcmTime (const std::string & sTime)


/**
 * Generates a dicom date std::string from the date fields
 *
 * @param sFormat     Format
 * @return  the string
 */
std::string DcmTime ::ToString(const std::string & sFormat)
{ 

  if ( ! m_bEmpty )
  {
    // ===============================================================================
    // If the format is empty send the default format of yyyyMMdd otherwise use the 
    // format specified.
    // =================================================================================
    if ( sFormat.empty() )
    {
      return util::Strings::Format("%02d%02d%07.4f",m_iHour, m_iMinutes, m_fSeconds);
    }
    else
    {
      std::string sHour(util::Strings::Format("%02d",m_iHour));
      std::string sMin(util::Strings::Format("%02d",m_iMinutes));
      std::string sSec(util::Strings::Format("%07.4f",m_fSeconds));

      std::string sTime(sFormat);

      size_t pos = sTime.find("HH");

      if ( pos != std::string::npos )
        sTime.replace(pos, 2, sHour);

       pos = sTime.find("mm");

       if ( pos != std::string::npos )
         sTime.replace( pos, 2, sMin);

       pos = sTime.find("ss");

       if ( pos != std::string::npos )
         sTime.replace( pos, 2, sSec.substr(0,2));

       pos = sTime.find("ss.ff");

       if ( pos != std::string::npos )
         sTime.replace( pos, 5, sSec);

       return sTime;
    } // END ..

    m_bEmpty = false;
  }
  else
    return std::string("");

} // ToString(const std::string & sFormat)


/**
 * Validates the time
 */
void DcmTime ::Validate()
{
  if ( ! m_bEmpty )
  {
    if ( m_iHour >= 24 )
      throw DcmInvalidDataException(LogRec("Hour must be less then 24", "DcmTime ", "Validate"));

    if ( m_iMinutes >= 60 )
      throw DcmInvalidDataException(LogRec("Minutes must be less then 60", "DcmTime ", "Validate"));

     if ( m_fSeconds >= 60.0F )
      throw DcmInvalidDataException(LogRec("Seconds must be less then 60", "DcmTime ", "Validate"));
  }
} // Validate()


/**
 * Sets the hour
 *
 *  @param iHour
 */
void DcmTime ::SetHour(unsigned short iHour)
{
  if ( iHour >= 24 )
    throw DcmInvalidDataException(LogRec("Hour must be less then 24", "DcmTime ", "Validate"));

  m_iHour = iHour;
  m_bEmpty = false;
} // SetHour(unsigned short iHour)


/**
 * Sets the minute
 *
 *  @param iMinutes
 */
void DcmTime ::SetMinutes(unsigned short iMinutes)
{
 if ( iMinutes >= 60 )
    throw DcmInvalidDataException(LogRec("Minutes must be less then 60", "DcmTime ", "Validate"));

  m_iMinutes = iMinutes;
  m_bEmpty = false;

} // SetMinutes(unsigned short iMinutes)


/**
 * Sets the seconds
 *
 *  @param  fSeconds
 */
void DcmTime ::SetSeconds(float fSeconds)
{
 if ( fSeconds >= 60.0F )
    throw DcmInvalidDataException(LogRec("Seconds must be less then 60", "DcmTime ", "Validate"));

 if ( fSeconds < 0.0F )
   throw DcmInvalidDataException(LogRec("Seconds must be greater then 0", "DcmTime ", "Validate"));

  m_fSeconds = fSeconds;
  m_bEmpty = false;

} // SetSeconds(unsigned short iSeconds)



/**
 * Sets the time using the std::string supplied
 *
 * @param sTime   Time std::string
 */
void DcmTime ::Assign(const std::string & sTime)
{
    // ======================================
  // Check if the time std::string has a value
  // ======================================
  if ( ! sTime.empty() )
  {
    try
    {      
      const size_t size = sTime.size();

      // =================================================================
      // Dicom Fomat HHMM[SS[FF]] where the bracketed entries are optional
      // =================================================================
      if ( size == 4 )
      {
        SetHour((unsigned short)atoi(sTime.substr(0,2).c_str()));
        SetMinutes((unsigned short)atoi(sTime.substr(2,2).c_str()));
        SetSeconds(0.0F);
      }
      else if ( size == 6 || ( size > 7 && sTime.find('.') != std::string::npos ) )
      {
        SetHour((unsigned short)atoi(sTime.substr(0,2).c_str()));
        SetMinutes((unsigned short)atoi(sTime.substr(2,2).c_str()));
        SetSeconds((float)atof(sTime.substr(4).c_str()));
      }
      else if ( sTime.size() >= 8 )
      {
        std::vector<std::string> fields = util::Strings::Split(sTime,':');

        if ( fields.size() != 3 )
          throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom time std::string %s",sTime.c_str()), "DcmTime ", "DcmTime "));

        SetHour((unsigned short)atoi(fields[0].c_str()));
        SetMinutes((unsigned short)atoi(fields[1].c_str()));
        SetSeconds((unsigned short)atoi(fields[2].c_str()));
     
      }
      else
        throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom time std::string %s",sTime.c_str()), "DcmTime ", "DcmTime "));
    }
    catch ( DcmInvalidDataException & ex )
    {
      throw ex;
    }
    catch ( ... )
    {
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom time std::string %s",sTime.c_str()), "DcmTime ", "DcmTime "));
    }

  } // END ... If the std::string is empty then just initialize to 0

} // Set()



/**
 * Gets the current time
 */
DcmTime  DcmTime ::Now()
{
  
  SYSTEMTIME sys;
  ::GetSystemTime(&sys);

  return DcmTime (sys.wHour, sys.wMinute, sys.wSecond);

} // Now()
