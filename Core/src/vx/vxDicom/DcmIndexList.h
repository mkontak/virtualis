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

class DCM_DLL DcmIndex;
class DCM_DLL DcmElement;
class DCM_DLL DcmElementCollection;

class DCM_DLL DcmIndexList
{
protected:

  DcmIndexList();


#pragma region create methods

public:

  /// Creates and empty list
  static std::shared_ptr<DcmIndexList> Create();

  /// Creates the liust based on the element tag supplied
  static std::shared_ptr<DcmIndexList> Create(const DcmTag & tag);

  /// Creates a named list
  static std::shared_ptr<DcmIndexList> Create(const std::string & sName);

#pragma endregion


#pragma region methods

public:

  /// Adds the DcmIndex 
  void Add(std::shared_ptr<DcmIndex> & index);

  /// Used to add element collection from sequences (Such as the Frame Content Sequence )
  void Add(std::shared_ptr<DcmElementCollection> & sequenceCollection);

  /// Adds the element as an index
  void Add(std::shared_ptr<dcm::DcmElement> & element);

  /// Determines if the index exists in the list
  bool Contains(const std::shared_ptr<DcmIndex> & index);

  /// Clears the indexes
  void Clear() { m_indexes.clear(); }

  /// Has index values tag
  bool HasMultiValuedIndex();

  //// Returns the index at the index specified
  std::shared_ptr<DcmIndex> & GetIndex(unsigned int index);

  /// Get the index count
  const int Count() const
  { return (int)m_indexes.size(); }

  /// Get the index values (Multi-Valued field containing the indexes ) 
  const std::shared_ptr<DcmIndex> & GetMultiValuedIndex() 
  { return m_multiValuedIndex; }


  /// Gets the deafult frame indexes (StackId/InStackPosition/TemporalIndex)
  static std::shared_ptr<DcmIndexList> GetDefaultSpatialIndexes();

  /// Returns true if the two indexs match
  bool Equals(const std::shared_ptr<DcmIndexList> & rhs)
  { return this == rhs.get(); }


#pragma endregion



#pragma region properties

public:

    /// Indidates that the index is empty
  bool IsEmpty();

#pragma endregion

  
#pragma region operators overloads

public:

    inline bool operator==(const dcm::DcmIndexList & rhs)
    {

      bool bEquals( m_multiValuedIndex == rhs.m_multiValuedIndex );

      for ( unsigned int i(0); i < m_indexes.size() && bEquals ; bEquals &= ( m_indexes[i] == rhs.m_indexes[i] ), i++ );
  
      return bEquals;
    } 

#pragma endregion

private:

  /// Mapped tag
  std::shared_ptr<DcmIndex> m_multiValuedIndex;

  /// Tag associated with the index list
  DcmTag m_tag;

  /// Name associated with the index list
  std::string m_sName;

  /// Indexes
  std::vector<std::shared_ptr<DcmIndex>> m_indexes;



}; // class DcmIndexList

};