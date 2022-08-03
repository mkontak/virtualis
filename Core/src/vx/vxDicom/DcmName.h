// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmName.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "DcmUtility.h"

#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

/**
 * Implements a Dicom NM field
 */
class DCM_DLL DcmName
{
public:

  DcmName();

  /// Construct a name using the individual fields
  DcmName(const std::string & sLast, const std::string & sFirst, const std::string & sMiddle = "", const std::string & sPrefix = "", const std::string & sSuffix = "" );

  /// Constructs a name using the dicom formatted name
  DcmName(const std::string & sName);

  /// Copy constructor
  DcmName(const DcmName & name);

  /// Destructor
  virtual ~DcmName() {}


  /// Gets the Last, First 
  std::string GetLastFirst() const;

  /// Gets the Last, First Middle
  std::string GetLastFirstMiddle() const;
  
  /// Gets the dicom formatted std::string
  std::string ToString();

  /// Validates the name
  void Validate();

  /// Sets the name using the supplied std::string
  void Assign(const std::string & sName);

#pragma region operator overloads

public:

  /// Overload to create the name from a stream
  friend std::istream & operator>>(std::istream & is, DcmName & name) 
  {
    std::string sName;

    is >> sName;
    
    /// Because of spaces assume that they have been replace with the US 
    name.Assign(util::Strings::Replace(sName,0X1F,' '));

    return is;
  } // operator>>
 
  /// Overload to create the date from a stream
  friend std::ostream & operator<<( std::ostream & os, DcmName & name) 
  {
    os << name.ToString();
    
    return os;
  } // operator>>


  /// Overloaded == operator
  inline bool operator==(const DcmName & rhs)
  { return (  m_names[LAST]   == rhs.m_names[LAST]   &&
              m_names[FIRST]  == rhs.m_names[FIRST]  &&
              m_names[MIDDLE] == rhs.m_names[MIDDLE] && 
              m_names[PREFIX] == rhs.m_names[PREFIX] &&
              m_names[SUFFIX] == rhs.m_names[SUFFIX] ); }

  /// Overloaded = operator
  inline void operator=(const DcmName & rhs)
  { 
    m_names[LAST]   == rhs.m_names[LAST];
    m_names[FIRST]  == rhs.m_names[FIRST];
    m_names[MIDDLE] == rhs.m_names[MIDDLE];
    m_names[PREFIX] == rhs.m_names[PREFIX];
    m_names[SUFFIX] == rhs.m_names[SUFFIX]; 
  }

#pragma endregion


private:

  /// Initializes objects state
  void Initialize();

  /// Splits the value up using the delimiter specified
  STRINGLIST Split(const std::string & sValue, std::string::value_type del);

  /// Name Fields
  typedef enum NameFields
  {
    LAST = 0,
    FIRST,
    MIDDLE,
    PREFIX,
    SUFFIX
  } NameFields;

public:

  /// Gets the first name
  const std::string & GetFirst() const
  { return m_names[FIRST]; }

  /// Sets the first name
  void SetFirst(const std::string & sName)
  { m_names[FIRST] = sName; }


  /// Gets the last name
  const std::string & GetLast() const
  { return m_names[LAST]; }

  /// Sets the last name
  void SetLast(const std::string & sName)
  { m_names[LAST] = sName; }

  /// Gets the middle name
  const std::string & GetMiddle() const
  { return m_names[MIDDLE]; }

  /// Sets the middle name
  void SetMiddle(const std::string & sName)
  { m_names[MIDDLE] = sName; }

  /// Gets the name prefix
  const std::string & GetPrefix() const
  { return m_names[PREFIX]; }

 /// Sets the prefix name
  void SetPrefix(const std::string & sPrefix)
  { m_names[PREFIX] = sPrefix; }

  /// Gets teh name suffix
  const std::string & GetSuffix() const
  { return m_names[SUFFIX]; }


 /// Sets the suffix name
  void SetSuffix(const std::string & sSuffix)
  { m_names[SUFFIX] = sSuffix; }

private:

  /// Name list [0] Last, [1] First, [2] Middle, [3] Prefix, [4] Suffix
  STRINGLIST m_names;


}; // class DcmName

};// namespace dcm

#pragma warning(pop)