#include "Stdafx.h"
#include "DcmExceptions.h"
#include "DcmSequenceItem.h"
#include "DcmElementCollection.h"
#include "DcmElementCollectionFactory.h"

#include <regex>


using namespace dcm;

 /**
  * Creates the DcmElementCollection from the xml node
  *
  * @param node     XmlNode
  *
  * @return DcmElementCOllection
  */
std::shared_ptr<DcmElementCollection> DcmElementCollectionFactory::Create(const xml::XmlNode & node)
{
  unsigned int card;

  // =================================================================================
  // Only the sequence item has the card attribute so we check for it and if 
  // it is found then we create a DcmSequenceItem otherwise it is a regular 
  // DcmElementCollection
  // =================================================================================
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<unsigned int>("card", card) )
    return DcmSequenceItem::Create(node);
  else
    return DcmElementCollection::Create(node);

} // Create(const xml::XmlNode & node)
 