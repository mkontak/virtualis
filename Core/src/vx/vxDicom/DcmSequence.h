// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "ICollection.h"
#include "DcmElement.h"
#include "DcmElementCollection.h"


#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{


 // forward declarations
class DCM_DLL DcmElement;


#pragma region typedefs

typedef std::vector<std::shared_ptr<dcm::DcmElementCollection>> DcmElementCollectionList;
typedef DcmElementCollectionList::iterator DcmElementCollectionListIterator;


#pragma endregion



/**
 * Implements a sequence which is a collection of element list
 */
class DCM_DLL DcmSequence : public DcmElement, public ICollection<DcmElementCollection>
{
protected:

  /// Constructor (Default)
  DcmSequence();

  /// Constructor with group/element
  DcmSequence(const DcmTag & tag);


  /// Clone the sequence
  virtual std::shared_ptr<dcm::DcmElement> Clone();


#pragma region Create Methods

public:

  /// Create the DcmSeqeunce from the xml node
  static std::shared_ptr<dcm::DcmSequence> Create(const xml::XmlNode & node);

  /// Create the DcmSeqeunce from the xml node
  static std::shared_ptr<dcm::DcmSequence> Create(const DcmTag & tag);

#pragma endregion

  
  /// Copy sequence to new sequence
  virtual void CopyTo(std::shared_ptr<dcm::DcmSequence>  & DcmSequence, bool clone = false );

  /// Generates the xml for this sequence
  virtual std::string ToXml();

  /// Will retieve the first element found in the sequence
  bool TryGetElement(const DcmTag & tag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements = false);


protected:

  /// Initializes the object from the xml node
  virtual void Initialize(const xml::XmlNode & node);

    /// Copies the sequence to the item
  virtual void CopyTo(DcmItem * pDcmItem);

public:

   /// Removes the specified element from the sequence
  bool Remove( std::shared_ptr<dcm::DcmElement> & element );
  
  /// Determines if the sequence item is already in the list
  bool Contains( std::shared_ptr<dcm::DcmElement> & element );

#pragma region ICollection

public:

  /// Returns true if the sequnce is empty
  virtual bool IsEmpty()
  { return m_items.size() == 0; }

  // Adds new collection to sequence
  void Add(std::shared_ptr<dcm::DcmElementCollection> & collection);


  /// Removes the sepcified collection from the sequence
  bool Remove( const std::shared_ptr<dcm::DcmElementCollection> & collection );


  /// Determnes if the sepcified collection exists in the collection
  bool Contains( const std::shared_ptr<dcm::DcmElementCollection> & collection );

  /// Resets the iterator
  inline void Reset()
  { m_indexer = -1; }

  /// Increments the iterator and returns the next item or null if we are at the end
  bool MoveNext();

  /// Clears the sequence item list
  inline void Clear()
  { m_items.clear(); Reset(); }

  /// Gets the current object
  std::shared_ptr<DcmElementCollection> Current();

  /// Returns the number of items in the list
  virtual unsigned int Count()
  { return (unsigned int)m_items.size(); }

#pragma endregion

#pragma region properties

public:


  /// Returns true if there are sub-elements
  virtual bool HasSubElements()
  { return (m_items.size() > 0); }

  /// Sets the value (No value for sequence)
  virtual void SetValue(const std::string & value)
  { }

  /// Sets the level
  virtual void SetLevel(unsigned short iLevel);
 
  
  /// Sets the expanded flag
  virtual void SetExpanded(bool bExpanded);

#pragma endregion

#pragma region operators

public:

  /// Overloaded == operator
  inline bool operator==(const dcm::DcmSequence & rhs)
  { return ( m_tag.hash() == rhs.m_tag.hash() ); }

 
#pragma endregion


private:

  /// Indexer
  int m_indexer;

  /// Iterator
  DcmElementCollectionListIterator m_iterator;

  /// Elements
  DcmElementCollectionList m_items;

  /// Read Only
  bool m_bReadOnly;


  friend class DCM_DLL DcmImage;

};  // class DcmSequence

};  // namespace dcm

#pragma warning(pop)