// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmSequence.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmUtility.h"
#include "DcmElementCollection.h"


using namespace dcm;


/**
 * Default constructor
 */
DcmSequence::DcmSequence() :
DcmElement(),
m_indexer(-1)
{

} // DcmSequence()


/**
 * Construct a sequence that is an element 
 *
 * @param iGroup
 * @param iElement
 */
DcmSequence::DcmSequence(const DcmTag & tag) :
DcmElement(tag),
m_indexer(-1)
{
} // DcmSequence(unsigned short iGroup, unsigned short iElement)


#pragma region Create Methods

/**
 * Creates the DcmElement from the xml node
 *
 * @param pNide    Xml node
 *
 * @return Shared pointer to an DcmElement
 */
std::shared_ptr<dcm::DcmSequence> DcmSequence::Create(const xml::XmlNode & node)
{

  std::shared_ptr<DcmSequence> sequence(new DcmSequence());

  if ( sequence == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmSeqeunce object", "DcmSequence","Create"));

  sequence->Initialize(node);
 
  return sequence;

} // Create(xmlNodePtr pElementNode)

/**
 * Creates the DcmElement from the xml node
 *
 * @param pNide    Xml node
 *
 * @return Shared pointer to an DcmElement
 */
std::shared_ptr<dcm::DcmSequence> DcmSequence::Create(const DcmTag & tag)
{

  std::shared_ptr<DcmSequence> sequence(new DcmSequence(tag));

  if ( sequence == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmSeqeunce object", "DcmSequence","Create"));

  return sequence;
} // Create( const DcmTag & tag )

#pragma endregion


/**
 * CLones the sequnece
 */
std::shared_ptr<dcm::DcmElement> DcmSequence::Clone()
{
  // Creates a new empty sequence with the same tag
  std::shared_ptr<DcmSequence> pDcmSequence(Create(this->m_tag));

  // Copy the DcmElement properties
  DcmElement::CopyTo(std::dynamic_pointer_cast<dcm::DcmElement>(pDcmSequence));

  CopyTo(pDcmSequence, true);

  
  // Returns the cloned sequence
  return pDcmSequence;

} // Clone()


/**
 * Copies the sequence to the supplied sequence
 *
 * @param pDcmSequence        Sequence to be copied to.
 */
void DcmSequence::CopyTo(std::shared_ptr<dcm::DcmSequence>  & pDcmSequence, bool clone)
{

  // Copy the DcmElement properties
  DcmElement::CopyTo(std::dynamic_pointer_cast<dcm::DcmElement>(pDcmSequence));


  // ========================================================================================================================
  // Convertedt he loop from an iterator to a index. I have read that the indexer gives better performance by a bit over the
  // iterator method. 
  //
  // std::for_each( m_items.begin(), m_items.end(), [ &pDcmSequence ] ( std::shared_ptr<dcm::DcmSequenceItem> item )
  // {
  //   pDcmSequence->Insert(std::dynamic_pointer_cast<DcmSequenceItem>(item->Clone()));
  // } ) ;
  // 
  // Loop through all the sequence items and clone them.
  //========================================================================================================================
  if ( clone )
    for ( int i(0); i < m_items.size(); pDcmSequence->Add( m_items[i++]->Clone() ) );
  else
    for ( int i(0); i < m_items.size(); pDcmSequence->Add(m_items[i++]));



} // CopyTo(std::shared_ptr<dcm::DcmSequence>  & pDcmSequence)

/**
 * Generates the XML for the element
 */
std::string DcmSequence::ToXml()
{
  // <element tag="0010,21b0" vr="LT" vm="1" len="10" name="AdditionalPatientHistory">CHEST PAIN</element>

  std::string sXml(util::Strings::Format("<sequence tag=\"%04x,%04x\" vr=\"%s\" card=\"%d\" name=\"%s\">\n", 
                    GetGroup(), GetElement(), GetVR().c_str(), Count(), GetName().c_str()));

  Reset();

  while ( MoveNext() )
  {
    sXml.append(Current()->ToXml());
  }

  sXml.append("</sequence>\n");

  return sXml;
 
  
}// ToXml()

/**
 * Initilazies the object from the xml
 *
 * @param node     Xml node
 */
void DcmSequence::Initialize(const xml::XmlNode & node)
{

  DcmObject::Initialize(node);

  // Get the sequence list items
  std::vector<xml::XmlNode> items = const_cast<xml::XmlNode &>(node).SelectNodes("item");
 
  if ( items.size() == 0 )
    DcmLog::Warning(util::Strings::Format("No sequence list items specified in the xml (%s)", node.GetName().c_str()),"DcmSequence","Create");


  // ==============================
  // Loop through all the elements
  // =============================
  for ( std::vector<xml::XmlNode>::const_iterator it = items.cbegin(); it != items.cend(); it++ )
  {
    Add(DcmElementCollection::Create(*it));
  } // END ... For each sequence item list

} // DcmSequence(xmlNodePtr pNode)


/**
 * Inserts the element into the list
 */
void DcmSequence::Add(std::shared_ptr<dcm::DcmElementCollection> & collection)
{
    /// Set the level
    collection->SetLevel(m_iLevel + 1);

    /// Add the new item
    m_items.push_back(collection);

    // Reset enumerator
    Reset();

} // Insert(std::shared_ptr<DcmElement> pDcmElement)

/**
 * Removes the specified collection
 *
 * @param collection      Collection to be removed
 *
 * @return TRue if the collection was removed, flase if not found
 */
bool DcmSequence::Remove(const std::shared_ptr<DcmElementCollection> & collection)
{
  bool bRemoved(false);

  for ( DcmElementCollectionList::iterator it(m_items.begin()); it != m_items.end(); it++ )
  {
    if ( (*it)->Equals(collection) )
    {
      m_items.erase(it);
      bRemoved = true;
      break;
    }
  } // END ... For each collection


  return bRemoved;
} // Remove(const std::shared_ptr<DcmElementCollection> & collection)

/**
 * Determines if  the specified collection exists in the list
 *
 * @param collection      Collection to be removed
 *
 * @return True if the collection was found, flase if not found
 */
bool DcmSequence::Contains(const std::shared_ptr<DcmElementCollection> & collection)
{
  bool bContains(false);

  for ( DcmElementCollectionList::iterator it(m_items.begin()); it != m_items.end(); it++ )
  {
    if ( (*it)->Equals(collection) )
    {
      bContains = true;
      break;
    }
  } // END ... For each collection


  return bContains;
}


/**
 * Removes the sequence item
 *
 * @param item      sequence iten to remove
 *
 * @return Returns true if removed , false the item was not found
 */
inline bool DcmSequence::Remove(std::shared_ptr<dcm::DcmElement> & element)
{

  bool bRemoved(false);

  for( DcmElementCollectionListIterator it( m_items.begin() ); it !=  m_items.end() && ! bRemoved ; it++ )
  {
    bRemoved = (*it)->Remove(element);
  }

  return bRemoved;

} // Remove(std::shared_ptr<dcm::DcmSequenceItem> & item)

 
/**
 * Determines if the serquence card already exists
 *
 * @param item      DcmSequenceItem
 */
inline bool DcmSequence::Contains(std::shared_ptr<dcm::DcmElement> & element)
{

  bool bContains(false);

  for( DcmElementCollectionListIterator it( m_items.begin() ); it !=  m_items.end() && ! bContains ; it++ )
  {
    bContains = (*it)->Contains(element);
  }

  return bContains;
} // Conatins()

/**
 * Gets the next element from the list or null if we are at the end
 *
 * @return true if we have an item, false it we are beyond the list
 */
bool DcmSequence::MoveNext()
{
  bool bMoved(false);

  if ( ++m_indexer < m_items.size() )
    bMoved = true;
  
 
  return bMoved;
} // Next()


/**
 * Gets the current element from the list or null if we are at the end
 *
 * @return DcmElementCollection
 */
std::shared_ptr<dcm::DcmElementCollection> DcmSequence::Current()
{
  std::shared_ptr<dcm::DcmElementCollection> collection(__nullptr);

  if ( m_indexer > -1 && m_indexer < m_items.size() )
    collection = m_items[m_indexer];
  
  return collection;
} // Next()


/**
 * Tries to get the speicfied element from within the sequence. Only the first element found is returned.
 *
 * @param tag       Tag
 * @param element   Element to return
 *
 * @return True if the element is found
 */
bool DcmSequence::TryGetElement(const DcmTag & tag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements  )
{

  bool bFound(false);
  element = __nullptr;


  // ====================================================================================================================================
  // Converted to indexer for better performance
  //
  // for( ITEMLIST::iterator it(m_items.begin()); !bFound && it != m_items.end(); (bFound = (*it)->TryGetElement(tag,element)), it++ );
  // ====================================================================================================================================
  for ( int i(0); !bFound && i < m_items.size();  bFound = m_items[i++]->TryGetElement(tag,element, searchSubElements) );



  return bFound;

} // TryGetElement(const DcmTag & tag, std::shared_ptr<dcm::DcmElement> & element )


/**
 * Sets the level of the sequence
 *
 * @param iLevel      Level 
 */
void DcmSequence::SetLevel(unsigned short iLevel)
{

  dcm::DcmElement::SetLevel(iLevel);
  
  const unsigned short iItemLevel(iLevel + 1);

  for ( int i(0); i < m_items.size(); m_items[i++]->SetLevel(iItemLevel) );

  
} // SetLevel:get(unsigned short iLevel)



/**
 * Sets the level of the sequence
 *
 * @param iLevel      Level 
 */
void DcmSequence::SetExpanded(bool bExpanded)
{

  dcm::DcmElement::SetExpanded(bExpanded);
  

  if ( ! bExpanded )
  {
    for ( int i(0); i < m_items.size(); i++)
    {
      m_items[i]->Reset();
      while ( m_items[i]->MoveNext() )
      {
        m_items[i]->Current()->SetExpanded(bExpanded);
      }

    }
  }
  
} // SetLevel:get(unsigned short iLevel)



/**
 * Copies the sequence element  into the DcmItem
 *
 * @param pDcmItem
 */
void DcmSequence::CopyTo(DcmItem * pDcmItem)
{

  if ( pDcmItem != __nullptr )
  {
     DcmSequenceOfItems * pDcmSequenceOfItems = new DcmSequenceOfItems(GetDcmTag());

     DcmItem * pSeqItem = new DcmItem();

     pDcmSequenceOfItems->insert(pSeqItem);

    Reset();

    while ( MoveNext() )
    {
      Current()->CopyTo((DcmItem *)pSeqItem);

    }

    pDcmItem->insert(pDcmSequenceOfItems);
  }
} // CopyTo()