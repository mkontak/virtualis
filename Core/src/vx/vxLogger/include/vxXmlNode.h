// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlNode.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once


// includes
#include <libxml/parser.h>
#include <libxml/tree.h>


// pargamas
#pragma warning(push)
#pragma warning(disable:4251)


// namespace
namespace xml
{

/**
 * Matches the xmlElementNode type from the library
 */
typedef enum XmlNodeType 
{
  XML_EMPTY_NODE  = 0,
  XML_ELEMENT_NODE=		1,
  XML_ATTRIBUTE_NODE=		2,
  XML_TEXT_NODE=		3,
  XML_CDATA_SECTION_NODE=	4,
  XML_ENTITY_REF_NODE=	5,
  XML_ENTITY_NODE=		6,
  XML_PI_NODE=		7,
  XML_COMMENT_NODE=		8,
  XML_DOCUMENT_NODE=		9,
  XML_DOCUMENT_TYPE_NODE=	10,
  XML_DOCUMENT_FRAG_NODE=	11,
  XML_NOTATION_NODE=		12,
  XML_HTML_DOCUMENT_NODE=	13,
  XML_DTD_NODE=		14,
  XML_ELEMENT_DECL=		15,
  XML_ATTRIBUTE_DECL=		16,
  XML_ENTITY_DECL=		17,
  XML_NAMESPACE_DECL=		18,
  XML_XINCLUDE_START=		19,
  XML_XINCLUDE_END=		20
#ifdef LIBXML_DOCB_ENABLED
  ,XML_DOCB_DOCUMENT_NODE=	21
#endif
} XmlNodeType;


/// Xml Node List
typedef std::vector<xml::XmlNode> XmlNodeList;

/**
 * Implements a xmlNodePtr wrapper adding additional functionality that emulates the .NET 
 * framework.
 */
class VX_LOGGER_DLL XmlNode
{
protected:


  // Constructor
  XmlNode ( xmlDocPtr doc, xmlNodePtr node );


public:

  // Copy constructor
  XmlNode(const XmlNode & rhs) ;

  /// Creates and XmlNode
  static XmlNode Create(const std::string & sName);

  // Determines if the node is valid
  inline bool IsValid() const
  { return ( m_node != __nullptr ); }

  // True if there are children
  inline bool HasChildren() const
  { return  ( m_node != __nullptr && m_node->children != __nullptr ); }

  // Returns true of the node has attributes
  inline bool HasAttributes() const
  { return ( m_node != __nullptr && m_node->properties != __nullptr ); }

  // Returns true of the node has content
  inline bool HasContent() const
  { return ( m_node != __nullptr && m_node->content != __nullptr ); }

  // Gets the first child
  inline XmlNode GetFirstChild()
  { m_currentChild = ( m_node != __nullptr ? m_node->children : __nullptr ); return XmlNode( m_doc, m_currentChild ); }

  // Gets the last child
  inline XmlNode GetLastChild() const
  { return XmlNode( m_doc, m_node != __nullptr ? xmlGetLastChild(m_node) : __nullptr ); }

  /// Gets the Next child sibling
  inline XmlNode GetNextChild() 
  { m_currentChild = ( m_currentChild != __nullptr ? m_currentChild->next : __nullptr ); return XmlNode( m_doc, m_currentChild); }


  // Gets the next sibling
  inline XmlNode GetNext()
  { return ( m_node != __nullptr ? XmlNode( m_doc, m_node->next ) : XmlNode( m_doc, __nullptr ) ); }

   // Gets the previous sibling
  inline XmlNode GetPrev()
 { return ( m_node != __nullptr ? XmlNode( m_doc, m_node->prev ) : XmlNode( m_doc, __nullptr ) ); }


  // Selects a single node
  XmlNode SelectSingleNode(const std::string & sRegEx, bool throwException = false);

  // Select matching child nodes
  XmlNodeList SelectNodes(const std::string & sRegEx);

  /// Get the xml attribute and returns the default if not found
  template <typename T>
  bool TryGetNodeAttributeValue(const std::string & sNodeName, const std::string & sAttributeName, T & value)
  {
  
    bool found(false);
 
    XmlNode node = SelectSingleNode(sNodeName);

    if ( node.IsValid() )
      found = node.TryGetAttributeValue<T>(sAttributeName, value);

      
    return found;

  } // TryGetNodeAttributeValue<std::string>(const std::string & sAttributeName, T defaultValue)





  /// Get the xml attribute and returns the default if not found
  template <typename T>
  bool TryGetAttributeValue(const std::string & sAttributeName, T & value)
  {
  
    bool found(false);
 

    if ( HasAttributes() )
    {
      const char * attName(sAttributeName.c_str());
      size_t size(sAttributeName.size());

      for ( xmlAttrPtr pAttNode = m_node->properties; pAttNode != __nullptr; pAttNode = pAttNode->next )
      {
        const char * nodeName((const char *)pAttNode->name);

        if ( pAttNode->type == XML_ATTRIBUTE_NODE && strcmp(nodeName, attName) == 0 )
        {
          std::istringstream is((const char *)pAttNode->children->content);

          is >> value;

          found = true;

          break;
        }  
      }
    }

      
    return found;

  } // TryGetAttributeValue<std::string>(const std::string & sAttributeName, T defaultValue)


   /// Get the xml attribute and returns the default if not found
  template <typename T>
  bool TryGetAttributeValue(const std::string & sAttributeName, std::string & value)
  {
  
    bool found(false);
 

    if ( HasAttributes() )
    {
      const char * attName(sAttributeName.c_str());
      size_t size(sAttributeName.size());

      
      for ( xmlAttrPtr pAttNode = m_node->properties; pAttNode != __nullptr; pAttNode = pAttNode->next )
      {
        const char * nodeName((const char *)pAttNode->name);

        if ( pAttNode->type == XML_ATTRIBUTE_NODE && strcmp(nodeName, attName) == 0 )
        {
          value = std::string((const char *)pAttNode->children->content);

          found = true;

          break;
        }  
      }
    }

      
    return found;

  } // TryGetAttributeValue<std::string>(const std::string & sAttributeName, T defaultValue)


  /// Generates the xml as a string
  std::string ToXml();
 
public:

  /// Get the NODE name
  const std::string & GetName() const
  { return m_sName; }

  /// Gets the content
  const std::string& GetContent() const
  { return m_sContent; }

  /// Sets the content
  void SetContent(const std::string & sContent);

  /// Gets the type
  const XmlNodeType GetType() const
  { return ( m_node != __nullptr ? (XmlNodeType)m_node->type : XML_EMPTY_NODE ); }

protected:

  /// Gets the node pointer
  const xmlNodePtr GetNodePtr()  const
  { return m_node; }

private:

  // Current iterator
  xmlNodePtr m_currentChild;

  /// Current sibling
  xmlNodePtr m_current;

  // Node name
  std::string m_sName;

  // Nodes content
  std::string m_sContent;

  // Node
  xmlNodePtr m_node;

  // Nodes doc
  xmlDocPtr m_doc;

  // Friend
  friend class VX_LOGGER_DLL XmlDocument;

}; // class XmlNode


} // namespace xml

#pragma warning(pop)