// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmRange.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmExceptions.h"

namespace dcm {


/**
 * Implements a range that works as follows.
 */
template<class T>
class DCM_DLL DcmRange
{
public:

  /// Default constructor
  DcmRange() {}

  /// Copy constructor
  DcmRange( const DcmRange<T> & range ) :
  m_from(range.m_from),
  m_to(range.m_to)
  {
  } // 


  /**
	 *  Baseline constructor
	 *
	 *  @param from
	 *  @param to
	 */
  DcmRange(const T & from, const T & to) 
  {
    if ( from > to ) 
      throw DcmInvalidDataException(LogRec("Invalid range, from > to ", "DcmRange", "DcmRange"));

    m_from = from;
    m_to = to;
  } // DcmRange(


  /**
   * Sets the RANGE values using the std::string. The format is FROM-TO, FROM-, -TO, FROM.
   * 
   * Assumes that the class being used has overridden the << operation so that 
   * a istd::stringstream can be used to convert the value.
   *
   * @param sRange      Range std::string
   */
  void Set(const std::string & sRange)
  {

    if ( ! sRange.empty() )
    {
   
      // Extract the fields
      STRINGLIST fields = util::Strings::Split(sRange,'-');

      // Start with the from
      std::istringstream isFrom(fields[0]);

      // Create the from from the istream
      isFrom >> m_from;

      // ===========================================
      // If there are more fields then set the to
      // ============================================
      if ( fields.size() > 1 )
      {
        std::istringstream isTo(fields[1]);

        isTo >> m_to;
      }

    } // END ... If the range is not empty

  } // Set(const std::string & sRange)


  /**
   * Set the from and to all at once
   *
   * @param from      From value
   * @param to        To value
   */
  void Set(const T & from, const T & to)
  {
    if ( from > to ) 
      throw DcmInvalidDataException(LogRec("Invalid range, from > to ", "DcmRange", "DcmRange"));

    m_from = from;
    m_to = to;

  } // Set(T from, T to)


  /*
   * Determines if the object is empty
	 *
	 *  @return true if it is empty
   */
  bool IsEmpty()
  {
    std::ostringstream fos;
    std::ostringstream tos;
    
    fos << m_from;
    tos << m_to;

    return ( fos.str().size() == 0 && tos.str().size() == 0 );
  } // IsEmpty()
  

  /**
   * Generate a range std::string
	 *
	 *  @return the string
   */
  std::string ToString()
  {
    std::ostringstream os;
    std::ostringstream fos;
    std::ostringstream tos;
    
    fos << m_from;
    tos << m_to;

    std::string sFrom(fos.str());
    std::string sTo(tos.str());

    if ( sFrom.size() > 0 && sTo.size() > 0)
    {
      os << sFrom << "-" << sTo;
    }
    else if ( sFrom.size() > 0 && sTo.size() == 0 )
    {
      os << sFrom << "-";
    }
    else if ( sFrom.size() == 0 && sTo.size() > 0 )
    {
       os << "-" << sTo;
    }
    else if ( sFrom.size() > 0 && sTo.size() > 0 && sFrom == sTo )
    {
      os << sFrom;
    }

    return os.str();
  } // ToString()

public:

  /// Gets the from value
  T GetFrom() { return m_from; }

  /// Sets the from value
  void SetFrom(const T & from) 
  { 
    if ( from > m_to ) 
      throw DcmInvalidDataException(LogRec("Invalid range, from > to ", "DcmRange", "DcmRange"));

    m_from = from;
  } // SetFrom(T from)

  /// Gets the to value
  T GetToValue() { return m_to; }

  /// Sets the from value
  void SetTo(const T & to) 
  { 
    if ( m_from > to ) 
      throw DcmInvalidDataException(LogRec("Invalid range, from > to ", "DcmRange", "DcmRange"));

    m_to = to;

  } // SetTo(T to)


private:

  /// From value
  T m_from;

  /// To Value
  T m_to;

};  // class DCM_DLL DcmRange

}; // namespace dcm