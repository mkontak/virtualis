// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmGroup.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once
 
#include "DcmDictionary.h"

namespace dcm
{ 

/**
 * Dicom Group 
 */
class DCM_DLL DcmGroup
{

public:


  // Constructor
  DcmGroup(const unsigned short & id, const std::string & description) :
  m_iId(id),
  m_sDescription(description)
  { }

  // Constructor
  DcmGroup(const unsigned short & id) :
  m_iId(id)
  {
    m_sDescription = DcmDictionary::GetGroupDescription(m_iId);
  }

  /// Dicom date
  inline std::string ToString()
  { return util::Strings::Format("%04d", m_iId); }

  /// Overloaded == operator
  inline bool operator==(const DcmGroup & rhs) const
  { return ( m_iId == rhs.GetId()  ); }



public:

  /// Gets the formatted Id
  const std::string GetFormattedId()
  { return  ( m_iId == 0XFFFF ? "-" : util::Strings::Format("%04x",m_iId) ); }

  /// Gets group identifier
  const unsigned short & GetId() const
  { return m_iId; }


  /// Gets the year
  const std::string & GetDescription() const
  { return m_sDescription ; }

private:

  /// Griup identifier
  unsigned short m_iId;

  /// Description
  std::string m_sDescription;


};  // class DcmGroup





}; // namespace dcm

