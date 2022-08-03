// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlNode.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)

// includes
#include "stdafx.h"
#include "vxXmlNode.h"
#include "vxXmlExceptions.h"
#include "vxUtilStrings.h"

#include <regex>


// using
using namespace xml;



#pragma region constructors

/**
 * Constructor
 *
 * @param doc     Document this node belongs to
 * @param node    Node this object encapsulates
 */
XmlNode::XmlNode(xmlDocPtr doc, xmlNodePtr node) :
m_doc(doc),
m_current(__nullptr),
m_node(node)
{
  m_sName = ( m_node != __nullptr && m_node->name ?  (char *)m_node->name : "" );
  m_sContent = ( m_node != __nullptr && m_node->content != __nullptr ? (char *)m_node->content : "" );
} // XmlNode(xmlDocPtr doc, xmlNodePtr node)


/**
 * Copy constructor
 */
XmlNode::XmlNode(const XmlNode & rhs) : 
m_sName(rhs.m_sName),
m_sContent(rhs.m_sContent),
m_current(rhs.m_current),
m_node(rhs.m_node)
{

}   // XmlNode(const XmlNode & rhs)


#pragma endregion



/**
 * Creates a new node
 *
 * @param sName     Node name
 *
 *@return XMlNOde
 */
XmlNode XmlNode::Create(const std::string & sName)
{

  return XmlNode( __nullptr, xmlNewNode(__nullptr, (xmlChar *)sName.c_str() ) );

} // CreateNode(const std::string & sName)


/**
 * Selects a single node ( the first ) from the supplied node parameter that matches the supplied name. 
 *
 * @param sName     Node name to search for
 *
 * @return Returns the node if found or throws an exception
 */
XmlNode XmlNode::SelectSingleNode(const std::string & sRegEx, bool throwException)
{

  xmlNodePtr pNode(__nullptr);

  std::regex rx(sRegEx);


  if ( HasChildren() )
  {
    for ( xmlNodePtr pChild = m_node->children; pChild != __nullptr; pChild = pChild->next )
    {
      if ( std::regex_match((const char *)pChild->name, rx)  )
      {
        pNode =  pChild;
        break;
      }  
    }
  }


  // =====================================================================================
  // If the throw exception flag and the section  was not found then throw the exception
  // =====================================================================================
  if ( throwException && pNode == __nullptr )
    throw XmlNodeNotFoundException(LogRec("Xml node specified [" + sRegEx + "] was not found " , "XmlDocument", "SelectSingleNode"));
  else
    return XmlNode(m_doc, pNode);
  
 
} // SelectSingleNode(const std::string & sName)


/**
 * Selects a single node ( the first ) from the supplied node parameter that matches the supplied name. 
 *
 * @param sName     Node name to search for
 *
 * @return Returns the node if found or throws an exception
 */
XmlNodeList  XmlNode::SelectNodes(const std::string & sRegEx)
{

  std::vector<XmlNode>  nodes;


  std::regex rx(sRegEx);

  if ( HasChildren() )
  {
    for ( xmlNodePtr pChild = m_node->children; pChild != __nullptr; pChild = pChild->next )
    {
      if ( std::regex_match((const char *)pChild->name, rx)  )
      {
        nodes.push_back(XmlNode(m_doc, pChild));
      }  
    }
  }

  return nodes;
  
 
} // SelectNodes( const std::string & sName)

/**
 * Sets the content
 */
void XmlNode::SetContent(const std::string & sContent)
{
  m_sContent = sContent;

  xmlNodeAddContent(m_node, (const xmlChar *)sContent.c_str());
} // SetContent(const std::string & sContent)


/**
 * Generates the xml string for the NODE
 *
 * @return XMl string
 */
std::string XmlNode::ToXml()
{
  std::string xml("");

  if ( m_node != __nullptr )
  {
    xmlBufferPtr nodeBuffer = xmlBufferCreate(); 

    xmlNodeDump(nodeBuffer, m_doc, m_node, 0, 0 );

    xml = std::string((char *)nodeBuffer->content);
  }

  return xml;
} // ToXml()

