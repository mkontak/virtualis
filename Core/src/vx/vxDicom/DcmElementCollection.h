
// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmElement.h"
#include "DcmDictionary.h"
#include "DcmTypedefs.h"
#include "ICollection.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{

/// Forward declarations
#pragma region forward declarations

// Sequence
class DCM_DLL DcmSequence;
class DCM_DLL DcmSequenceItem;
class DCM_DLL DcmGroup;
class DCM_DLL DcmIndex;


#pragma endregion



/**
 * List of eleements that is itself an element
 */
class DCM_DLL DcmElementCollection : public ICollection<dcm::DcmElement>
{

#pragma region constructor

protected:

  /// Constructor
  DcmElementCollection();

  /// Constructor (Copy)
  DcmElementCollection(const DcmElementCollection & dcmElementList);


#pragma endregion


#pragma region Create Methods

public:

  // Create the DcmElementCollection from the header string
  static std::shared_ptr<dcm::DcmElementCollection> Create(const std::string & sParameter, const DcmParameterType & type);

  // Create the DcmElementCollection from the xml node
  static std::shared_ptr<dcm::DcmElementCollection> Create(const xml::XmlNode & node);

  // Creates an empty list
  static std::shared_ptr<dcm::DcmElementCollection> Create();

#pragma endregion

#pragma region ICollection methods

  /// Returns the number of elements in the list
  virtual unsigned int Count()
  { return (unsigned int)m_elements.size(); }

  /// Clears the collection
  virtual inline void Clear()
  { m_elements.clear(); Reset(); }

  /// Resets the iterator
  virtual inline void Reset()
  { m_indexer = -1; }

  /// Current iterator item
  std::shared_ptr<dcm::DcmElement> Current();

  /// Move to the next item (used in conjunction with the current)
  virtual bool MoveNext();

  /// Remove the specified element
  virtual inline bool Remove(const std::shared_ptr<dcm::DcmElement> & element)
  {  return Remove(element->GetDcmTag()); }

  /// Removes all element in the specified group
  virtual void Remove(const unsigned short & iGroup);

  /// Removes the specified element
  virtual bool Remove(const DcmTag & tag);

  /// Returns true if the value is contained in the list, false otherwise
  virtual bool Contains(const DcmTag & tag);

  /// Determines if the element is contained in the list
  virtual bool inline Contains(const std::shared_ptr<dcm::DcmElement> & element)
  { return Contains(element->GetDcmTag()); }

  /// Returns true if the collection is empty, flase if not
  virtual bool IsEmpty()
  { return m_elements.size() == 0; }

  /// Insert/Updates the specified Element in the list
  virtual void Add(std::shared_ptr<dcm::DcmElement> & element);

#pragma endregion


public:

  /// Determines if the collections are equal
  bool Equals(const std::shared_ptr<DcmElementCollection> & collection);

    //// Write the XML
  virtual void WriteXml(const std::string & sFilePath);

  /// Generates the xml representation of the element list
  virtual std::string ToXml();


  /// Clones the object
  virtual std::shared_ptr<dcm::DcmElementCollection> Clone();

  /// Copy this objects list to the new element list object (may clone the objects)
  virtual void CopyTo(std::shared_ptr<dcm::DcmElementCollection> & pDcmELementList, bool clone = false);


  /// Inserts the collection supplied
  void Insert(std::shared_ptr<DcmElementCollection> & collection, bool overwrite = false);


  //// Returns a collection containing only the group specified
  std::shared_ptr<dcm::DcmElementCollection> GetElements(unsigned short iGroup);

  /// Gets the list of groups this list contains
  DcmGroupMap GetGroups();


  /// Get the elements collections for each frame (Per-Frame)
  DcmElementCollectionList GetFrames();


  /// Returns the oild style serialize header
  std::string GetSerializedHeader();

private:

  /// Genefrates the DcmItem for this collection 
  virtual void CopyTo(DcmItem * pDcmItem);


#pragma region TryGet methods

public:

  // Try and get the specified element - if found set the element and return true, if not found return false
  bool TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements = false);

  inline bool TryGetElement(const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements = false)
  { return TryGetElement(DcmTag(0,0), elementTag, element, searchSubElements); }

#pragma endregion

#pragma region properties

public:


  
  /// Gets the level
  virtual const unsigned short & GetLevel() const
  { return m_iLevel; }

  /// Sets the level
  virtual void SetLevel(unsigned short iLevel);

#pragma endregion

   
#pragma region Xml Loading Methods

protected:


  /// Attempts the load the xml node (Calls one or more of the methods below )
  virtual void Initialize(  const xml::XmlNode & node );

  /// Loads the patient node or the node that contains patient data
  void LoadPatientNode( const  xml::XmlNode & node );

  /// Loads the study node or the node that contains study data
  void LoadStudyNode( const  xml::XmlNode & node );

  /// Load the series node or node that contains series data
  void LoadSeriesNode( const  xml::XmlNode & node );

  /// Load data-set node
  void LoadHeaderNode( const  xml::XmlNode & node );


#pragma endregion

private:

  /// Loads the serialize header
  void LoadHeader(const std::string & sHeader);

#pragma region fields

private:

  /// Iterator
  DcmElementMapIterator m_iterator;

  /// Elements
  DcmElementMap m_elements;

  /// INdexer
  int m_indexer;

  /// Level
  unsigned short m_iLevel;

#pragma endregion

#pragma region fiends

/// Sequence (Needs access to CopyTo(DcmItem & item)
friend class DCM_DLL DcmSequence;
friend class DCM_DLL DcmImage;

#pragma endregion

};  // class DcmElementCollection

};  // namespace dcm

#pragma warning(pop)
