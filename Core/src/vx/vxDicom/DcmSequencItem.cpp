// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmSequenceItem.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmUtility.h"
#include "DcmSequence.h"
#include "DcmElement.h"

using namespace dcm;


#pragma region constructors

/**
 * Default constructor
 */
dcm::DcmSequenceItem::DcmSequenceItem() :
DcmElementCollection(),
m_iCard(0)
{

} // DcmElementCollection()

/**
 * Default constructor
 */
dcm::DcmSequenceItem::DcmSequenceItem(const unsigned int & iCard) :
DcmElementCollection(),
m_iCard(iCard)
{

} // DcmElementCollection()



#pragma endregion

#pragma region Create Methods


/**
 * Creates the element list from the header
 *
 * @param sHeader     Header
 *
 * @return the element list
 */
std::shared_ptr<dcm::DcmSequenceItem> dcm::DcmSequenceItem::Create(const xml::XmlNode & node)
{
  std::shared_ptr<DcmSequenceItem> item(new dcm::DcmSequenceItem());

  if ( item == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmSequenceItem object","DcmSequenceItem","Create"));

  item->Initialize(const_cast<xml::XmlNode &>(node));
  return item;

} // Create(xmlNodePtr pNode)


/**
 * Creates an empty list
 *
 * @return the element list
 */
std::shared_ptr<dcm::DcmSequenceItem> dcm::DcmSequenceItem::Create(const unsigned int iCard)
{
  std::shared_ptr<dcm::DcmSequenceItem> item(new dcm::DcmSequenceItem(iCard));

  if ( item == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmSequenceItem object","DcmSequenceItem","Create"));

 
  return item;

} // Create()

#pragma endregion

/**
 * Clones the sequnce item
 *
 * @return DcmElementCollection 
 */
std::shared_ptr<DcmElementCollection> DcmSequenceItem::Clone()
{
  std::shared_ptr<DcmSequenceItem> pDcmSequenceItem(Create(m_iCard));

 
  CopyTo(pDcmSequenceItem, true);

  return pDcmSequenceItem;
} // Clone();


/**
 * Copies all elements from this list to the other list
 *
 * @param pDcmSequenceItem       List to copy to 
 */
void DcmSequenceItem::CopyTo(std::shared_ptr<DcmSequenceItem> & pDcmSequenceItem, bool clone)
{
  pDcmSequenceItem->m_iCard = this->m_iCard;
  
  DcmElementCollection::CopyTo(std::dynamic_pointer_cast<DcmElementCollection>(pDcmSequenceItem), clone);
} // CopyTo(std::shared_ptr<DcmElementCollection> & pDcmElementCollection)


/**
 * Overridee the base element collections add method so hat the sub element flag can be set
 *
 * @param element     Element to add
 */
void DcmSequenceItem::Add(std::shared_ptr<dcm::DcmElement> & element)
{
  element->SetLevel(GetLevel());
  DcmElementCollection::Add(element);
  element->m_bIsSubElement = true;
} // Add(std::shared_ptr<dcm::DcmElement> & element)


/**
 * Generates the XML for the element
 */
std::string dcm::DcmSequenceItem::ToXml(const std::string & sRoot)
{
  std::string sXml(util::Strings::Format("<item card=\"%d\">\n", m_iCard));

  sXml.append(DcmElementCollection::ToXml());

  sXml.append("</item>\n");

  return sXml;
 
  
}// ToXml()

/**
 * Initializes the element list
 *
 * @param pNode       Node
 */
void dcm::DcmSequenceItem::Initialize(xml::XmlNode & node)
{
  
  node.TryGetAttributeValue<unsigned int>("card", m_iCard);

  dcm::DcmElementCollection::Initialize(node);

} // Initialize(xmlNodePtr pNode)

