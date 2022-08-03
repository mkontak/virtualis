// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDouble.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmUtility.h"
#include "DcmList.h"


namespace dcm
{


/**
 * Implements a pair of data elements such as x and y. 
 */
template<class T>
class DCM_DLL DcmDouble 
{
public:

  /// Default constructor
  DcmDouble() 
  {
    m_list.Add((T)0);     // X
    m_list.Add((T)0);     // Y
  } // DcmDouble()

  /// Constructor
  DcmDouble(T x, T y) 
  {
    m_list.Add(x);        // X
    m_list.Add(y);        // Y

  } // DcmDouble(T x, T y, T z ) 

  /// Copy constructor
  DcmDouble(const DcmDouble<T> & dbl ) 
  {
    m_list.Add(const_cast<DcmDouble<T> &>(dbl).GetX());
    m_list.Add(const_cast<DcmDouble<T> &>(dbl).GetY());
  } // DcmDouble(const DcmTriple & triple )


  /// Gets the double using the values supplied
  DcmDouble(const std::string & sValue)
  {
    Assign(sValue);
  } // DcmDouble(const std::string & sDicomValue)


  /// Returns the dicom std::string for the double.
  std::string ToString() 
  { return m_list.ToString(); }

  /// Assign the double usin the string
  void Assign(const std::string & sValue)
  {
    /// Sets the DcmList with the delimited string
    m_list.AssignList(sValue);

    // Get the element count
    const int iSize(m_list.Size());

    // Make sure there are 2 elements
    if ( iSize != 2 ) 
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Dicom value is not in the proper format (only %d values detected, expected 2)", iSize), "DcmDouble", "DcmDouble"));
  }

    /// Overload to create the date from a stream
  friend std::istream & operator>>(std::istream & is, DcmDouble<T> & dbl) 
  {
    std::string sDouble;
    is >> sDouble;
    
    dbl.Assign(sDouble);

    return is;
  } // operator>>
 
  /// Overload to create the cosines from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmDouble<T> & dbl) 
  {
    os << dbl.ToString();
    
    return os;
  } // operator>>

 
public:

  /// Gets the X value
  virtual T GetX() 
  { return m_list[X]; }

  /// Sets the X value
  virtual void SetX(T x)
  { m_list[X] = x; }

  /// Gets the Y value
  virtual T GetY() 
  { return m_list[Y]; }

  /// Sets  the Y value
  virtual void SetY(T y)
  { m_list[Y] = y; }


protected:

  /// Implements the list of dicom objects
  DcmList<T> m_list;

 };  // class DcmDouble

};  // namespace dcm