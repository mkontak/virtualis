#include "Stdafx.h"
#include "DcmExceptions.h"

#include "DcmSequence.h"
#include "DcmElementFactory.h"


using namespace dcm;


/**
 * Creates a DcmElement based on xml node
 */
std::shared_ptr<dcm::DcmElement> DcmElementFactory::Create(const xml::XmlNode & node )
{


  const std::string sName(node.GetName());

  if ( sName == "element" )
    return DcmElement::Create(node);
  else if ( sName == "sequence" )
    return DcmSequence::Create(node);
  else 
    return __nullptr;

} // DcmElementFactory::Create(xml::XmlNode node)

