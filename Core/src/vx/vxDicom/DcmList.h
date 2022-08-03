// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmList.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmExceptions.h"

#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm 
{

/**
 * Templatized list class used to hold a collection of objects.
 */
template<class T>
class DCM_DLL DcmList
{
public:

  /// Default constructor
  DcmList() { }

  /// Copy constructor
  DcmList(const DcmList<T> & list)
  {
    // ============================================================
    // Loop through all the objects copying them to the local list
    // ============================================================
    for ( std::vector<T>::const_iterator it = list.m_values.begin(); it != list.m_values.end(); it++ )
    {
      m_values.push_back(*it);
    } // END ... For each object
  } // DcmList

  /// Constructor using the delimited std::string
  DcmList(const std::string & list)
  {
    AssignList(list);
  } // DcmList

   

  /// Clears the value list
  virtual inline void Clear()
  { m_values.clear(); }


  /// Returns true if the list is empty 
  virtual inline bool IsEmpty()
  { return m_values.size() == 0; }
 

  /// Overloaded == operator
  inline bool operator==(const DcmList<T> & rhs)
  { return ( ToString() == const_cast<DcmList<T> &>(rhs).ToString() ); }


  /**
   * Converts the object to a DICOM representation of a multi-value std::string
	 *
	 *  @param sepperator
	 *  @return the string
   */
  std::string ToString(char seperator)
  {
    std::ostringstream os;

    for ( std::vector<T>::iterator it = m_values.begin(); it != m_values.end();  )
    {
      os << *it;
      if ( ++it != m_values.end() )
        os << seperator;
    }

    return os.str();
  } // ToString()


  /// Uses the more common '\' delimiter
  inline std::string ToString()
  { return ToString('\\'); }


#pragma region Add Methods

  /// Adds new value
  void Add(const T & value)
  {  
     m_values.push_back(value); 
  } // Add


  /**
	 *  Add string value which splits
	 *
	 *  @param sValues
	 */
  void AddList(const std::string & sValues)
  {
    STRINGLIST list = util::Strings::Split(sValues,"\\:;,|");

    for ( STRINGLIST::iterator it = list.begin(); it != list.end(); it++ )
    {
      T value;

      std::istringstream is((*it));

      is >> value;

      m_values.push_back(value);
    }
  } // Add

  
#pragma endregion

#pragma region Assign Methods

  /*
	 * Assigns the specified value
	 *
	 *  @param value
	 */
  void Assign(const T & value)
  {
    m_values.clear();

    m_values.push_back(value);

  } // Assign(const T & sValue)


  /**
	 *  Assigns the specified value
	 *
	 *  @param value
	 */
  void AssignList(const std::string & value)
  {
    m_values.clear();

    AddList(value);

  } // Assign(const T & sValue)const std::string & value)


#pragma endregion


public:


  /**
	 *  Overload >> operator
	 *
	 *  @param is 
	 *  @param list
	 *  @return is
	 */
  friend std::istream & operator>>(std::istream & is, DcmList & list) 
  {
    std::string sList;
    is >> sList;
    
    list.AssignList(sList);

    return is;
  } // operator>>
 

	/**
	 *   Overload << operator 
	 *
	 *   @param os
	 *   @param list
	 *   @return os
	 */
  friend std::ostream & operator<<( std::ostream & os, DcmList & list) 
  {
    os << list.ToString();
    
    return os;
  } // operator>>


  /// Overloads the get []
  T &operator [](int i)
  {
    if ( i < 0 || i > m_values.size() )
      throw DcmOutOfBoundsException(LogRec("Index to list is out of bounds","DcmList", "GetItem"));

    return m_values[i];
  } 


public:

  /// Gets the count
  inline int Size() 
  { return m_values.size(); }

protected:

  /// Max size
  int m_iMaxSize;

  /// List vector
  std::vector<T> m_values;

  /// Iterator
  std::vector<T> m_iterator;

};  // class DcmList


};  // namespace dcm

#pragma warning(pop)