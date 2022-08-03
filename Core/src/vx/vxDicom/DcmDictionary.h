// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDictionary.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

 
namespace dcm
{

/// Forward declarations
class DCM_DLL DcmElement;

class DCM_DLL DcmGroup;


typedef std::map<unsigned short, dcm::DcmGroup> DcmGroupMap;
typedef std::vector<dcm::DcmGroup> DcmGroupList;

/**
 * Encapsulates the dictionary operations
 */
class DCM_DLL DcmDictionary
{


public:

  /// Try and get the entry
  static bool TryGetEntry(const DcmTag & tag, DcmDictEntry ** pEntry);

  /// Exists
  static bool Exists(const DcmTag & tag);

  /// Exists
  static bool Exists(std::shared_ptr<dcm::DcmElement> & pElement);

  /// Adds the element in the node
  static void AddElement(std::shared_ptr<dcm::DcmElement> pElement);

  /// Gets the group description
  static std::string GetGroupDescription(unsigned short iGroup);

  /// Gets a vector of groups
  static const DcmGroupMap & GetGroups();

  /// Loads external distionaries
  static bool LoadExternalDictionary(const std::string & sFilePath);


private:

  static void Insert(const DcmGroup & group);

private:


  /// Groups
  static DcmGroupMap m_groups;

}; // class DCM_DLL DcmDictionary

};  // namespace DcmDictaionary