// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmObject.h"
#include "DcmUtility.h"
#include "DcmLog.h"
#include "DcmDictionary.h"

using namespace dcm;

#pragma region constructors

/**
 * Constructor
 */
dcm::DcmObject::DcmObject() :
m_sName("")
{
} // DcmElement()

/**
 * Constructor
 */
dcm::DcmObject::DcmObject(const DcmTag & tag) :
m_tag(tag),
m_sName("")
{
} // DcmElement()

/**
 * Constructor (Copy)
 */
dcm::DcmObject::DcmObject(const DcmObject & dcmObject) :
m_sName("")
{
} // DcmElement()



#pragma endregion


/**
 * Copis the elements of this object to the sorresponding element of the supplied 
 * object.
 *
 * @param pDcmObject      Destination object
 */
void dcm::DcmObject::CopyTo(std::shared_ptr<dcm::DcmObject> & pDmcObject)
{

  pDmcObject->m_sName    = this->m_sName;
  pDmcObject->m_tag      = this->m_tag;

} // CopyTo()

/**
 * Inializes the element using the xml node
 *
 * @param pNode     Xml node
 */
void dcm::DcmObject::Initialize( const xml::XmlNode & node)
{

    /// Get the Tag
  std::string sTag;
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "tag", sTag) )
    throw DcmNotFoundException(LogRec("Missing tag attribute","DcmImage","LoadDatasetNode"));

  std::vector<std::string> fields = util::Strings::Split(sTag, ',');

  if ( fields.size() != 2 )
    throw DcmInvalidDataException(LogRec("Invalid element node, invalid tag attribute","DcmElement", "Create"));

  SetGroup((unsigned short)DcmUtility::HexToInt(fields[0]));
  SetElement((unsigned short)DcmUtility::HexToInt(fields[1]));

 
  std::string sVR;
  if ( ! const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("vr", sVR) )
    throw DcmNotFoundException(LogRec("Missing vr attribute","DcmElement","Create"));

  std::transform(sVR.begin(), sVR.end(), sVR.begin(), ::towupper);

  DcmVR vr;
  vr.setVR(sVR.c_str());

  m_tag.setVR(vr);

  const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "name", m_sName);

 
} //Initialize(const xml::XmlNode & node)


/**
 * Generates the string representation of the DcmObject
 */
std::string dcm::DcmObject::ToString()
{

  return util::Strings::Format("(%s) %s",GetTag().c_str(), m_sName.c_str());

}   // ToString()