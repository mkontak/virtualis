// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmElement.h"
#include "DcmUtility.h"
#include "DcmLog.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmDictionary.h"
#include "dcmdata\dcbytstr.h"
#include "dcmdata\dcchrstr.h"

using namespace dcm;

#pragma region constructors

/**
 * Constructor
 */
dcm::DcmElement::DcmElement() :
DcmObject(),
m_sValue(""),
m_bBinary(false),
m_bHidden(false),
m_iVM(1),
m_iLength(0),
m_iLevel(0),
m_bIsSubElement(false),
m_bExpanded(false)
{
} // DcmElement()

/**
 * Constructor
 */
dcm::DcmElement::DcmElement(const DcmTag & tag, const std::string & sValue) :
DcmObject(tag),
m_sValue(sValue),
m_bBinary(false),
m_bHidden(false),
m_iVM(1),
m_iLength(0),
m_iLevel(0),
m_bIsSubElement(false),
m_bExpanded(false)
{
} // DcmElement()

/**
 * Constructor (Copy)
 */
dcm::DcmElement::DcmElement(const DcmElement & dcmElement) :
DcmObject(dcmElement),
m_sValue(dcmElement.m_sValue),
m_bBinary(dcmElement.m_bHidden),
m_bHidden(dcmElement.m_bHidden),
m_iVM(dcmElement.m_iVM),
m_iLength(dcmElement.m_iLength),
m_iLevel(dcmElement.m_iLevel),
m_bIsSubElement(dcmElement.m_bIsSubElement),
m_bExpanded(dcmElement.m_bExpanded)
{
} // DcmElement()



/**
 * Constructor
 *
 * @param sTag        Tag (GGGG_EEEE)
 * @param sValue      Serialize Value
 */
dcm::DcmElement::DcmElement(const std::string & sTag, const std::string & sValue)
{
  // Split the tag into the group and element id ( format is GROUP_ELEMENT )
  std::vector<std::string> tagIds = util::Strings::Split(sTag,'_');

  // ================================================================================================================
  // Make sure the tag is proper GGGG_EEEE format. The size of the split value from above needs to be 2 otherwise 
  // we are most likely out of sync
  // ================================================================================================================
  if ( tagIds.size() != 2 )
    throw DcmInvalidDataException(LogRec("Invalid tag found in string", "DcmElement", "Create"));

  m_tag.setGroup((unsigned short)DcmUtility::HexToInt(tagIds[0]));
  m_tag.setElement((unsigned short)DcmUtility::HexToInt(tagIds[1]));

  // Get the value which is the next value
  m_sValue = DeSerializeValue(sValue);

  // Dictionary entry holder
  DcmDictEntry * pEntry(__nullptr);

  // ==================================
  // Try and get the dictionary entry 
  // ==================================
  if ( DcmDictionary::TryGetEntry(m_tag, & pEntry) )
  {
    m_tag.setVR(pEntry->getVR());
  }
  else
    DcmLog::Warning("Element not found in dictionary","DcmElement","Create");

} // DcmElement(const std::string & sTag, const std::string & sValue)

#pragma endregion



#pragma region Create Methods


/**
 * Creates the DcmElement from the xml node
 *
 * @return Shared pointer to an DcmElement
 */
std::shared_ptr<dcm::DcmElement> dcm::DcmElement::Create()
{

  std::shared_ptr<DcmElement> element(std::shared_ptr<dcm::DcmElement>(new DcmElement()));

  if ( element == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElement object", "DcmElement","Create"));

  return element;

} // Create()

/**
 * Creates the DcmElement from the xml node
 *
 * @param pElementNode    Xml node
 *
 * @return Shared pointer to an DcmElement
 */
std::shared_ptr<dcm::DcmElement> dcm::DcmElement::Create(const xml::XmlNode & node)
{

  std::shared_ptr<DcmElement> element(std::shared_ptr<DcmElement>(new dcm::DcmElement()));


  if ( element == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElement object", "DcmElement","Create"));

    element->Initialize(const_cast<xml::XmlNode &>(node));

  return element;

} // Create(const xml::XmlNode & node)


/**
 * Creates the DcmElement from the serialize data
 *
 * @param sTag      Tag 
 * @param sValue
 *
 * @return Shared pointer to an DcmElement
 */
std::shared_ptr<dcm::DcmElement> dcm::DcmElement::Create(const std::string & sTag, const std::string & sValue)
{

  std::shared_ptr<dcm::DcmElement> element(std::shared_ptr<dcm::DcmElement>(new dcm::DcmElement(sTag, sValue)));

  if ( element == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElement object", "DcmElement","Create"));

  return element;

} // Create(const std::string & sTag, const std::string & sValue)

#pragma endregion




/**
 * Clones the DcmElement
 */
std::shared_ptr<dcm::DcmElement> dcm::DcmElement::Clone()
{

  // Create a new empty element
  std::shared_ptr<dcm::DcmElement> pDcmElement(Create());

  // Copy this elements properties to the new element
  CopyTo(pDcmElement);

  // Returns the newly cloned element
  return pDcmElement;
} // Clone()




/**
 * Clones the DcmElement
 */
void dcm::DcmElement::CopyTo(std::shared_ptr<dcm::DcmElement>  & pDcmElement)
{
  
  /// Copy the objects base
  DcmObject::CopyTo(std::dynamic_pointer_cast<dcm::DcmObject>(pDcmElement));

  // Copy the element properties
  pDcmElement->m_bBinary  = this->m_bBinary;
  pDcmElement->m_bHidden  = this->m_bHidden;
  pDcmElement->m_iLength  = this->m_iLength;
  pDcmElement->m_iVM      = this->m_iVM;
  pDcmElement->m_sValue   = this->m_sValue;

} // CopyTo(std::shared_ptr<dcm::DcmElement>  & pDcmElement)




/**
 * Generates the XML for the element
 */
std::string dcm::DcmElement::ToXml()
{
  // <element tag="0010,21b0" vr="LT" vm="1" len="10" name="AdditionalPatientHistory">CHEST PAIN</element>


  if ( IsBinary() && IsHidden() )
    return util::Strings::Format("<element tag=\"%04x,%04x\" vr=\"%s\" vm=\"%d\" len=\"%d\" name=\"%s\" binary=\"hidden\">%s</element>\n", 
                                GetGroup(), GetElement(), GetVR().c_str(), GetVM(), m_iLength, GetName().c_str(), GetValue().c_str());
  else
    return util::Strings::Format("<element tag=\"%04x,%04x\" vr=\"%s\" vm=\"%d\" len=\"%d\" name=\"%s\">%s</element>\n", 
                                GetGroup(), GetElement(), GetVR().c_str(), GetVM(), m_iLength, GetName().c_str(), GetValue().c_str());
 
  
}// ToXml()



/**
 * Inializes the element using the xml node
 *
 * @param pNode     Xml node
 */
void dcm::DcmElement::Initialize( xml::XmlNode & node)
{

  /// Base object
  DcmObject::Initialize(node);

  /// skip binary for now, but it it was stored in the cxml it would be base64
  std::string sBinary;
  if ( node.TryGetAttributeValue<std::string>( "binary", sBinary) ) // && sBinary == "hidden" )
  {
    m_bBinary = true;
    m_bHidden = ( sBinary == "hidden" );
  }
  
  // Value is of the content is under the first child, which has a name of text and it's content has the value.
  m_sValue =  ParseValue( node.HasChildren()  ?  node.GetFirstChild().GetContent() : "" );

  node.TryGetAttributeValue<unsigned short>( "vm", m_iVM);

  node.TryGetAttributeValue<unsigned int>( "len", m_iLength);

} //Initialize(const xml::XmlNode & node)

/**
 * Serializes the element value.
 *
 * The following character strings are replaced so that the original value can be realized.
 *
 * Original Character     Mapped To
 *  ' '                   ~~
 *  '\r'                  !!
 *  '\n'                  @@
 *  '#'                   **
 *
 *  empty                 NONE
 *
 * @param Serialized value
 */
std::string dcm::DcmElement::SerializeValue(const std::string & sValue)
{
  std::string sSerializedValue(sValue);
   
  if ( sSerializedValue.length() != 0 )
  {
    util::Strings::Replace(sSerializedValue, " ","~~");
    util::Strings::Replace(sSerializedValue, "\r","!!");
    util::Strings::Replace(sSerializedValue, "\n","@@");
    util::Strings::Replace(sSerializedValue, "#","**");

  }
  else
    sSerializedValue.assign("NONE");

  return sSerializedValue;
} // SerializeValue(std::string & sValue)


/**
 * De-serializes the element value back to its original format.
 *
 * The following character strings are replaced so that the original value can be realized.
 *
 * Mapped String      Original Character
 *  ~~                 ' '
 *  !!                 '\r'
 *  @@                 '\n'
 *  **                 '#'
 *  
 *  NONE               empty
 *  NOVALUE            empty      (Backward compatible to volumes created  prior to the change to NONE in CON-3.0)
 *
 * @param De-serialized value
 */
std::string dcm::DcmElement::DeSerializeValue(const std::string & sValue)
{

  std::string sDeserializedString(sValue);

  if ( sDeserializedString != "NONE" && sDeserializedString != "NOVALUE" )
  {
    util::Strings::Replace(sDeserializedString, "~~", " ");
    util::Strings::Replace(sDeserializedString, "!!", "\n");
    util::Strings::Replace(sDeserializedString, "@@", "\r");
    util::Strings::Replace(sDeserializedString, "**", "#");

  }
  else
    sDeserializedString.assign("");


  return sDeserializedString;


} // DeSerializeValue(std::string & sValue)

/**
 * Copies the element into the DcmItem
 *
 * @param pDcmItem
 */
void dcm::DcmElement::CopyTo(DcmItem * pDcmItem)
{

  if ( pDcmItem != __nullptr )
  {

    pDcmItem->remove(m_tag);

    ::DcmByteString  * pElement = new ::DcmByteString(m_tag);
    pElement->putString(m_sValue.c_str());
    OFCondition cond = pDcmItem->insert(pElement);



  }
} // CopyTo()


/**
 * Gets the value for the macro specified. Will call the appropriate methods to obtain the value.
 *
 * @param   macro     Macro to get the value for.
 *
 * @return Macro value
 */
std::string dcm::DcmElement::GetMacroValue(const std::string & macro)
{

  std::string macroValue(macro);

  if (macro == "@UID@")
    macroValue = dcm::DcmUtility::GenerateUid();
  else if (macro == "@DATE@")
    macroValue = dcm::DcmDate::Now().ToString();
  else if (macro == "@TIME@")
    macroValue = dcm::DcmTime::Now().ToString();
  else if (macro == "@DATETIME@")
    macroValue = util::Strings::Format("%s%s", dcm::DcmDate::Now().ToString().c_str(), dcm::DcmTime::Now().ToString().c_str());
  
  return macroValue;
}

/**
 * Parses the value for special macros
 */
std::string dcm::DcmElement::ParseValue(const std::string & sValue)
{

  std::string newValue(sValue);

  const std::vector<std::string> macros = { "@UID@", "@DATE@", "@TIME@", "@DATETIME@" };



    std::for_each(macros.begin(), macros.end(), [this, &newValue](std::string macro) 
    {

      std::size_t pos(0);
      while ((pos = newValue.find(macro)) != std::string::npos)
      {
        newValue.replace(pos, macro.size(), GetMacroValue(macro));

      }

  });


  return newValue;
} //ParseValue(const std::string & sValue)