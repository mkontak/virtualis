// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmTriple.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "DcmEnums.h"
#include "DcmUtility.h"
#include "DcmDouble.h"

namespace dcm
{

/**
 * Implements a triple object that holds three values such as an x, y and z. Inherits from the DcmDouble class so that only the z 
 * needs to be added. 
 */
template<class T>
class DCM_DLL DcmTriple : public DcmDouble<T>
{
public:
  
  /// Default constructor
  DcmTriple() :
  DcmDouble()
  {
    // Add 1 entry to the list which should be the Z. The DcmDouble adds the first two entries X and Y.
    m_list.Add((T)0);
  } // DcmTriple()

  /// Constructor
  DcmTriple(T x, T y, T z ) :
  DcmDouble(x,y)
  {
    // Adds Z
    m_list.Add(z);
  } // DcmTriple(T x, T y, T z ) 

  /// Copy constructor
  DcmTriple(const DcmTriple<T> & triple ) :
  DcmDouble(static_cast<const DcmDouble<T> &>(triple))
  {
    // Adds Z
    m_list.Add(const_cast<DcmTriple<T> &>(triple).GetZ());
  } // DcmTriple(const DcmTriple & triple )


  /// Gets the triple using the values supplied
  DcmTriple(const std::string & sDicomValue) :
  DcmDouble()
  {
    Assign(sDicomValue);

  } // DcmTriple(const std::string & sDicomValue)

 
  // =============================================
  // Assign a DICOM string value to the trible
  // =============================================
  void Assign(const std::string & sDicomValue)
  {
    // Sets the internal DcmList uing the delimited string. 
    m_list.AssignList(sDicomValue);

    // Get the number of elements
    const int iSize(m_list.Size());

    // Make sure the number of elements are 3
    if ( iSize != 3 ) 
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Dicom value is not in the proper format (only %d values detected, expected 3)", iSize), "DcmTriple", "DcmTriple"));

  }

   /// Overload to create the date from a stream
  friend std::istream & operator>>(std::istream & is, DcmTriple<T> & triple) 
  {
    std::string sTriple;
    is >> sTriple;
    
    triple.Assign(sTriple);

    return is;
  } // operator>>
 
  /// Overload to create the cosines from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmTriple<T> & triple) 
  {
    os << triple.ToString();
    
    return os;
  } // operator>>


public:

  /// Gets the Z value
  virtual T GetZ()
  { return m_list[Z]; }

  /// Sets the Z value
  virtual void SetZ(T z)
  { m_list[Z] = z; }


};  // class DcmTriple


};  // namespace dcm