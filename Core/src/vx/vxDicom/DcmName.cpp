// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmName.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "DcmName.h"
#include "DcmUtility.h"
#include "DcmExceptions.h"


using namespace dcm;


/**
 * Constructs the name using the individual fields
 *
 * @param sName   Dicom name
 */
DcmName::DcmName() 
{
  Initialize();
} // DcmName(const std::string & sName)


/**
 * Constructs the name using the individual fields
 *
 * @param sLast   Last name
 * @param sFirst  First name
 * @param sMiddle Middle name
 * @param sPrefix name prefix
 * @param sSuffix name suffix
 */
DcmName::DcmName(const std::string & sLast, const std::string & sFirst, const std::string & sMiddle, const std::string & sPrefix, const std::string & sSuffix) 
{
  m_names.push_back(sLast);     // Last
  m_names.push_back(sFirst);    // First
  m_names.push_back(sMiddle);   // Middle
  m_names.push_back(sPrefix);   // Prefix
  m_names.push_back(sSuffix);   // Suffix

} // DcmName(const std::string & sLast, const std::string & sFirst, const std::string & sMiddle, const std::string & sPrefix, const std::string & sSuffix)


/**
 * Constructs the name using the individual fields
 *
 * @param sName   Dicom name
 */
DcmName::DcmName(const DcmName & name) 
{
  DcmUtility::Copy<std::string>(name.m_names, m_names);

  for ( size_t i(m_names.size()); i < 6;  m_names.push_back(""), i++ );

} // DcmName(const DcmName & name)


/**
 * Creates a DICOM name from the DICOM field
 * 
 * @param sName     Dicom name
 *
 * @return DcmName 
 */
DcmName::DcmName(const std::string & sName)
{
  Initialize();

  Assign(sName);

} // DcmName(const std::string & sName)


/**
 * Sets the name object using the su0pplied std::string
 *
 * @param sName   Name to parse
 */
void DcmName::Assign(const std::string & sName)
{

  // Look for ^, ' ' or , characters
  STRINGLIST fields = util::Strings::Split(sName, "^ ,");

  int i(0);
  for ( STRINGLIST::iterator it = fields.begin(); it != fields.end(); it++ )
  {
    if ( i < 5 )
      m_names[i++] = (*it);
    else
      m_names.push_back((*it));
  }

  for ( size_t i(m_names.size()); i < 6;  m_names.push_back(""), i++ );

}  // Set()Set(const std::string & sName)


/**
 * Generates a DICOM name
 */
std::string DcmName::ToString()
{
  std::ostringstream os;

  if ( ! m_names[SUFFIX].empty () )
    os << m_names[LAST] << "^" << m_names[FIRST] << "^" << m_names[MIDDLE] << "^" << m_names[PREFIX] << "^" << m_names[SUFFIX];
  else if ( ! m_names[PREFIX].empty () )
    os << m_names[LAST] << "^" << m_names[FIRST] << "^" << m_names[MIDDLE] << "^" << m_names[PREFIX];
  else if ( ! m_names[MIDDLE].empty () )
    os << m_names[LAST] << "^" << m_names[FIRST] << "^" << m_names[MIDDLE];
  else if ( ! m_names[FIRST].empty() )
    os << m_names[LAST] << "^" << m_names[FIRST];
  else 
    os << m_names[LAST];

  return os.str();

} // ToString()



/**
 * Validates the name
 */
void DcmName::Validate()
{
  if ( m_names[LAST].empty() )
    throw DcmInvalidDataException(LogRec("Must have a last name","DcmName","Validate"));
} // Validate()


/**
 * Initializes the object
 */
void DcmName::Initialize()
{
  m_names.push_back("");    // Last
  m_names.push_back("");    // First
  m_names.push_back("");    // Middle
  m_names.push_back("");    // Prefix
  m_names.push_back("");    // Suffix
} // Initialize()
