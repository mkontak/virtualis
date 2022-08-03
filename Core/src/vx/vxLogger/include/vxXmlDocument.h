// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxXmlDocument.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// include
#include <libxml/parser.h>
#include <libxml/tree.h>

// pragma
#pragma warning(push)
#pragma warning(disable:4251)


// namespace
namespace xml
{


  
// Forward declarations
class VX_LOGGER_DLL XmlNode;



/**
 * Implements a Document wrapper for the libxml document. This will clean up when 
 * deleted or falls out of scope.
 */
class VX_LOGGER_DLL XmlDocument
{
public:

  /// Constructor
  XmlDocument();

  /// Destructor
  ~XmlDocument();

  /// Loads the XML
  virtual void LoadXml(const std::string & sXml);

  /// Loads Xml File
  virtual void LoadFile(const std::string & sFilePath);

  /// Save the xml
  virtual void Save(const std::string & sFilePath );
      
  /// Gets the document element
  XmlNode GetDocumentElement();

  /// Sets the root element
  XmlNode  SetDocumentElement(XmlNode root);

  /// Returns the XML as a string
  std::string ToXml();


protected:

  /// Release resources
  virtual void ReleaseResources();

protected: 

  /// Xml Document Pointer
  xmlDocPtr m_document;

  /// standard indentation
  static std::string m_sIndent;

  // Friends
  friend class VX_LOGGER_DLL XmlSchema;

}; // class XmlDocument



} // namespace xml

#pragma warning(pop)