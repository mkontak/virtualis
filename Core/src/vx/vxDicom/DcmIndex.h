// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#pragma warning(push)
#pragma warning(disable:4251)



namespace dcm
{


class DCM_DLL DcmIndex
{
protected:

  /// constrcutor
  DcmIndex(const std::string & sUid, const  DcmTag & functionalGroup, const DcmTag & indexElement);

  /// Copy constructor
  DcmIndex(const DcmIndex & index) { }

public:

  /// Creates a basic index with the tag to be used
  static std::shared_ptr<DcmIndex> Create(const DcmTag & indexElement);

  /// Creates a functaional defined index
  static std::shared_ptr<DcmIndex> Create(const std::string & sUid, const  DcmTag & functionalGroup, const DcmTag & indexElement);

 
  bool Equals(const std::shared_ptr<DcmIndex> & index)
  {  return index.get() == this; }


#pragma region operator overloads

public:

   /// operator ==
  inline bool operator==(const dcm::DcmIndex & rhs)
  { return ( m_sUid == rhs.m_sUid && m_functionalGroup == rhs.m_functionalGroup && m_indexElement == rhs.m_indexElement ); }

#pragma endregion



public:


  // Returns true if the index is empty
  bool IsEmpty();

  /// Get the UID this index is associated with
  const std::string & GetUid() const
  { return m_sUid; }

  /// Gets the group tag
  const DcmTag & GetFunctionalGroup() 
  { return m_functionalGroup; }

  /// Gets the index
  const DcmTag & GetIndexElement()
  { return m_indexElement; }

  /// Gets the name
  const std::string & GetIndexElementName() const
  { return m_sIndexElementName; }

  /// Gets the name
  const std::string & GetFunctionalGroupName() const
  { return m_sFuntcionalGroupName; }

private:

  /// UID index is associated with
  std::string m_sUid;

  /// Name of the index
  std::string m_sIndexElementName;

  /// Functaion group name
  std::string m_sFuntcionalGroupName;

  /// functional group (sequence)
  DcmTag m_functionalGroup;

  /// Index tag
  DcmTag m_indexElement;

};  // DcmIndex

}