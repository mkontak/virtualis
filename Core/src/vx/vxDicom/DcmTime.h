// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmTime.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


namespace dcm
{


/**
 * Dicom time structire
 *
 * allows manipulation of a dicom time data field
 */
class DCM_DLL DcmTime
{
public:

  DcmTime();

  // Constructor
  DcmTime(unsigned short iHour, unsigned short iMinutes, float fSeconds);

  /// Copy constructor
  DcmTime(const DcmTime & time);

  /// Creates the DcmTime from the dicom date std::string
  DcmTime(const std::string & sTime);

  /// Returns emtry time
  static DcmTime Empty()
  { return DcmTime(); }

  /// Dicom date
  inline std::string ToString()
  { return ToString(""); }

  /// Converts the time to a std::string of the specified format
  std::string ToString(const std::string & sFormat);

  /// Overloaded > operator
  inline bool operator>(const DcmTime & rhs) const
  { return (  ( m_iHour > rhs.GetHour() ) || 
              ( m_iHour == rhs.GetHour() && m_iMinutes > rhs.GetMinutes() ) || 
              ( m_iHour == rhs.GetHour() && m_iMinutes == rhs.GetMinutes() && m_fSeconds > rhs.GetSeconds())  ); }

  ///// Overload to create the date from a stream
  friend std::istream & operator>>(std::istream & is, DcmTime & time) 
  {
    std::string sTime;
    is >> sTime;
    
    time.Assign(sTime);

    return is;
  } // operator>>
 
  /// Overload to create the date from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmTime & time) 
  {
    os << time.ToString();
    
    return os;
  } // operator>>


  /// Validate the time
  void Validate();

  /// Return true if the object is not set
  bool IsEmpty()
  { return m_bEmpty; }

  /// Sets the time using the std::string
  void Assign(const std::string & sTime);

  /// Get the current time
  static DcmTime Now();

public:

  /// Sets the hour
  void SetHour(unsigned short iHour);

  /// Gets the hours
  const unsigned short GetHour() const
  { return m_iHour; }

  /// Sets the minutes
  void SetMinutes(unsigned short iMinutes);
 
  /// Gets the minutes
  const unsigned short GetMinutes() const 
  { return m_iMinutes; }

  /// Sets the minutes
  void SetSeconds(float fSeconds);

  /// Gets the seconds
  const float GetSeconds() const
  { return m_fSeconds; }

private:

  /// Month
  unsigned short m_iHour;

  /// Day
  unsigned short m_iMinutes;

  /// Year
  float m_fSeconds;

  /// Empty flag
  bool m_bEmpty;

};  // class DcmTime

}; // namespace dcm