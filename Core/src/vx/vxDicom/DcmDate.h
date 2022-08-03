// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDate.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once
 

namespace dcm
{  

/**
 * Dicom date 
 */
class DCM_DLL DcmDate
{
public:

  /// Default constructor
  DcmDate();

  // Constructor
  DcmDate(unsigned short iMonth, unsigned short iDay, unsigned short iYear);

  /// Copy constructor
  DcmDate( const DcmDate & date);

  /// Creates the DcmDate from the dicom date std::string
  DcmDate( const std::string & sDate );

  /// Returns empty date
  static DcmDate Empty()
  { return DcmDate(); }

  /// Dicom date
  inline std::string ToString()
  { return ToString(""); }

  /// Returns date using the format specified yyyy - year, MM - month, dd - day
  std::string ToString(const std::string & sFormat);

  /// Overloaded == operator
  inline bool operator==(const DcmDate & rhs) const
  { return ( m_iYear == rhs.GetYear() && m_iMonth == rhs.GetMonth() && m_iDay == rhs.GetDay() ); }

  /// Overloaded > operator
  inline bool operator>(const DcmDate & rhs) const
  { return (  ( m_iYear > rhs.GetYear() ) ||
              ( m_iYear == rhs.GetYear() && m_iMonth > rhs.GetMonth() ) ||
              ( m_iYear == rhs.GetYear() && m_iMonth == rhs.GetMonth() && m_iDay > rhs.GetDay() ) ); }


  /// Overload to create the date from a stream
  friend std::istream & operator>>(std::istream & is, DcmDate & date) 
  {
    std::string sDate;
    is >> sDate;
    
    date.Assign(sDate);

    return is;
  } // operator>>
 
  /// Overload to create the date from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmDate & date) 
  {
    os << date.ToString();
    
    return os;
  } // operator>>

  /// Validates the date
  void Validate();

  /// Return true if the object is not set
  bool IsEmpty()
  { return m_bEmpty; }

  /// Sets the date
  void Assign(const std::string & sDate);

  /// Gets the current date
  static DcmDate Now();

public:

  /// Gets the month
  const unsigned short GetMonth() const
  { return m_iMonth; }

  /// Gets the day
  const unsigned short GetDay() const 
  { return m_iDay; }

  /// Gets the year
  const unsigned short GetYear() const
  { return m_iYear; }

private:

  /// Month
  unsigned short m_iMonth;

  /// Day
  unsigned short m_iDay;

  /// Year
  unsigned short m_iYear;

  /// Empty indicator
  bool m_bEmpty;

};  // class DcmDate





}; // namespace dcm