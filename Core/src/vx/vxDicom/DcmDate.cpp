// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDate.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "DcmDate.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"


using namespace dcm;


/**
 * Defaiult constructor
 */
DcmDate::DcmDate() :
m_bEmpty(true),
m_iMonth(0),
m_iDay(0),
m_iYear(0)
{
} // DcmDate()


/**
 * Constructor
 *
 *  @param iMonth
 *  @param iDay
 *  @param iYear
 */
DcmDate::DcmDate(unsigned short iMonth, unsigned short iDay, unsigned short iYear) :
m_iMonth(iMonth),
m_iDay(iDay),
m_iYear(iYear),
m_bEmpty(false)
{
} // DcmDate(unsigned short iMonth, unsigned short iDay, unsigned short iYear) 


/**
 * Defaiult constructor
 *
 *  @param date
 */
DcmDate::DcmDate(const DcmDate & date)
{
  m_bEmpty  = date.m_bEmpty;
  m_iMonth  = date.m_iMonth;
  m_iDay    = date.m_iDay;
  m_iYear   = date.m_iYear;
} // DcmDate()


/**
 * Constructs the date from the dicom std::string
 *
 * @param sDate     Dicom date
 * 
 */
DcmDate::DcmDate(const std::string & sDate) :
m_bEmpty(sDate.empty()),
m_iMonth(0),
m_iDay(0),
m_iYear(0)
{
  Assign(sDate);
} // DcmDate(const std::string & sDate)


/**
 * Generates a dicom date std::string from the date fields
 *
 * @param sFormat     Format
 * @return string
 */
std::string DcmDate::ToString(const std::string & sFormat)
{ 

  if ( ! m_bEmpty )
  {
    // ===============================================================================
    // If the format is empty send the default format of yyyyMMdd otherwise use the 
    // format specified.
    // =================================================================================
    if ( sFormat.empty() )
    {
      return util::Strings::Format("%04d%02d%02d",m_iYear, m_iMonth, m_iDay); 
    }
    else
    {
      std::string sYear(util::Strings::Format("%04d",m_iYear));
      std::string sMonth(util::Strings::Format("%02d",m_iMonth));
      std::string sDay(util::Strings::Format("%02d",m_iDay));

      std::string sDate(sFormat);

      size_t pos = sDate.find("yyyy");

      if ( pos != std::string::npos )
        sDate.replace(pos, 4, sYear);
      else
      {
        pos = sDate.find("yy");

        if ( pos != std::string::npos )
          sDate.replace(pos,2,sYear.substr(sYear.size() - 2));
      }

       pos = sDate.find("MM");

       if ( pos != std::string::npos )
         sDate.replace( pos, 2, sMonth);

       pos = sDate.find("dd");

       if ( pos != std::string::npos )
         sDate.replace( pos, 2, sDay);

       return sDate;
    } // END .. 

  } // END ... If the object s not empty
  else
    return std::string("");

} // ToString(const std::string & sFormat)


/**
 * Validates the object
 */
void DcmDate::Validate()
{

  if ( ! m_bEmpty ) 
  {
    /// Validates the year
    if ( m_iYear < 1800 || m_iYear > 2500 )
      throw DcmInvalidDataException(LogRec("Invalid year must be greater then 1900 and less then 2500", "DcmDate", "Validate"));

    /// Validates the month
    if ( m_iMonth == 0 || m_iMonth > 12 )
      throw DcmInvalidDataException(LogRec("Invalid month value must be 1 to 12", "DcmDate", "Validate"));
  } // END ... If empty
  
} // Validate()


/**
 * Sets the date using the std::string
 *
 * @param sDate     Date std::string
 */
void DcmDate::Assign(const std::string & sDate)
{
    // =============================================================
  // If the std::string is empty then just leave the defaults of 0
  // =============================================================
  if ( ! sDate.empty() )
  {

    try
    {

      // ==========================================
      // Formatted std::string of YYYYMMDD is assumed
      // ==========================================
      if (  sDate.size() == 8 )
      {
        m_iYear   = (unsigned short)atoi(sDate.substr(0,4).c_str());
        m_iMonth  = (unsigned short)atoi(sDate.substr(4,2).c_str());
        m_iDay    = (unsigned short)atoi(sDate.substr(6,2).c_str());
      }
      // ===============================================================================================
      // Formatted std::string is assumed that there are delimters between the fields ( '/', '-', '.' )
      // ===============================================================================================
      else if ( sDate.size() == 10 )
      {
        bool bInvalidFormat(true);
        std::string delimiters("/-.");
        int iDelimiterIndex(0);

        for( ; iDelimiterIndex < delimiters.size() ;  )
        {
          size_t pos = sDate.find(delimiters[iDelimiterIndex]);
          if ( pos != std::string::npos )
          {
            bInvalidFormat = false;
            break;
          }
          iDelimiterIndex++;
        }
            
        if ( bInvalidFormat )
          throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom date std::string %s",sDate.c_str()), "DcmDate", "DcmDate"));

        std::vector<std::string> fields = util::Strings::Split(sDate,delimiters[iDelimiterIndex]);

        // Make sure there is a Year. Month and Day
        if ( fields.size() != 3 )
          throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom date std::string %s",sDate.c_str()), "DcmDate", "DcmDate"));
        
         if ( fields[0].size() == 4 && fields[1].size() <= 2 && fields[2].size() <= 2 )
         {
           m_iYear   = (unsigned short)atoi(fields[0].c_str());
           m_iMonth  = (unsigned short)atoi(fields[1].c_str());
           m_iDay    = (unsigned short)atoi(fields[2].c_str());
         }
         else if ( fields[2].size() == 4 && fields[0].size() <= 2 && fields[1].size() <= 2 )
         {
           m_iYear   = (unsigned short)atoi(fields[2].c_str());
           m_iMonth  = (unsigned short)atoi(fields[0].c_str());
           m_iDay    = (unsigned short)atoi(fields[1].c_str());
         }
         else 
           throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom date std::string %s",sDate.c_str()), "DcmDate", "DcmDate"));

      }
      else
        throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom date std::string %s",sDate.c_str()), "DcmDate", "DcmDate"));

      m_bEmpty = false;

    }
    catch ( DcmInvalidDataException & ex )
    {
      throw ex;
    }
    catch ( ... )
    {
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid dicom date std::string %s",sDate.c_str()), "DcmDate", "DcmDate"));
    }

  } // if

} // Set()


/**
 * Gets the current date
 */
DcmDate DcmDate::Now()
{

  SYSTEMTIME sys;
  ::GetSystemTime(&sys);

  return DcmDate(sys.wMonth, sys.wDay, sys.wYear);

} // Now()