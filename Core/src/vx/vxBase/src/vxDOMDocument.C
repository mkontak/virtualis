// $Id: vxDOMDocument.C,v 1.3 2007/01/09 19:15:35 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxDOMAttribute.h"

// define declarations
#define FILE_REVISION "$Revision: 1.3 $"

// namespace declarations
using namespace XERCES_CPP_NAMESPACE;

// static declarations
XERCES_CPP_NAMESPACE::XercesDOMParser * vxDOMDocument::m_pParser = NULL;
vxDOMParseErrorHandler vxDOMDocument::m_errorHandler;

/**
 * Default Constructor
 */
vxDOMDocument::vxDOMDocument( const std::string & sTagName ) : m_pDocument( NULL )
{
  try
  {
    XMLCh * feature = XMLString::transcode( "Core" );
    DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation( feature );
    XMLString::release( &feature);

    XMLCh * tagName = XMLString::transcode( sTagName.c_str() );
    m_pDocument = impl->createDocument( 0, tagName, 0 );
    XMLString::release( &tagName );
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "vxDOMDocument" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "vxDOMDocument" );
  }
} // vxDOMDocument()


/**
 * Private constructor
 */
vxDOMDocument::vxDOMDocument( XERCES_CPP_NAMESPACE::DOMDocument * pDocument ) : 
  m_pDocument( pDocument )
{
} // vxDOMDocument()


/**
 * Destructor
 */ 
vxDOMDocument::~vxDOMDocument()
{
  int4 fix_me_george;
  /*if( m_pDocument != NULL )
  {
    m_pDocument->release();
    m_pDocument = NULL;
  }*/
} // ~vxDOMDocument()


/*
 * Gets the root element for the document
 *
 * @return element node
 */
vxDOMElement vxDOMDocument::GetDocumentElement() const
{
  DOMElement * pElement( NULL );
  try
  {
    if (!IsValid())
      throw ex::NullPointerException(LogRec("No document element defined", "vxDocument", "GetDocumentElement"));

    pElement = m_pDocument->getDocumentElement();
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "GetDocumentElement" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "GetDocumentElement" );
  }
  return vxDOMElement( pElement );
} // GetDocumentElement()


/*
 * Creates an element node
 *
 * @param     sTagName    node name
 * @return    element node
 */
vxDOMElement vxDOMDocument::CreateElement( const std::string & sTagName )
{
  DOMElement * pElement( NULL );
  try
  {
    if (!IsValid())
      throw ex::NullPointerException(LogRec("No document element defined", "vxDocument", "GetDocumentElement"));

    XMLCh * sName = XMLString::transcode( sTagName.c_str() );
    pElement = m_pDocument->createElement( sName );
    XMLString::release( &sName );
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "CreateElement" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "CreateElement" );
  }
  return vxDOMElement( pElement );
} // CreateElement()


/*
 * Creates an attribute node
 *
 * @param     sTagName    node name
 * @return    attribute name
 */
vxDOMAttribute vxDOMDocument::CreateAttribute( const std::string & sTagName )
{
  DOMAttr * pAttr( NULL );
  try
  {
    if (!IsValid())
      throw ex::NullPointerException(LogRec("No document element defined", "vxDocument", "GetDocumentElement"));

    XMLCh * sName = XMLString::transcode( sTagName.c_str() );
    pAttr = m_pDocument->createAttribute( sName );
    XMLString::release( &sName );
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "CreateAttribute" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "CreateAttribute" );
  }
  return vxDOMAttribute( pAttr );
} // CreateAttribute()


/**
 * Saves the current document to disk.
 *
 * @param   sFilename   Filename to save xml as
 */
void vxDOMDocument::Save( const std::string & sFilename )
{
  try
  {
    std::fstream file( sFilename.c_str(), std::ios::out ); 
    file << ToString();
    file.close();
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "Save" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "Save" );
  }
} // Save()


/**
 * Returns the contents of the document as a string
 *
 * @return    Document as string
 */
std::string vxDOMDocument::ToString()
{
  std::string sXml;

  if (!IsValid())
    throw ex::NullPointerException(LogRec("No document element defined", "vxDocument", "GetDocumentElement"));

  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  DOMImplementation *impl = 
     DOMImplementationRegistry::getDOMImplementation(gLS);


  DOMLSSerializer* pWriter = ((DOMImplementationLS*)impl)->createLSSerializer();

  // serialize the DOMNode to a UTF-16 string
  XMLCh* theXMLString_Unicode = pWriter->writeToString( m_pDocument->getDocumentElement() );

  char * sText = XMLString::transcode( theXMLString_Unicode );
  sXml = std::string("<?xml version=\"1.0\"?>\n") + sText;
  XMLString::release( &sText );

  // release the memory
  XMLString::release(&theXMLString_Unicode); 
  
  pWriter->release();

  return sXml;
} // ToString()

/**
 * Creates a vxDOMDocument from the specified file
 *
 * @param   sXml    xml to create document from
 * @return  document
 */
vxDOMDocument vxDOMDocument::FromFile( const std::string & sFile )
{
  XERCES_CPP_NAMESPACE::DOMDocument * pDocument( NULL );
  try
  {

    m_pParser->reset();
    m_pParser->parse( sFile.c_str() );
    pDocument = m_pParser->getDocument();
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "FromFile" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "FromFile" );
  }
  return vxDOMDocument( pDocument );
} // FromFile()


/**
 * Creates a vxDOMDocument from the specified xml
 *
 * @param   sXml    xml to create document from
 * @return  document
 */
vxDOMDocument vxDOMDocument::FromXml( const std::string & sXml )
{
  XERCES_CPP_NAMESPACE::DOMDocument * pDocument( NULL );
  try
  {
    MemBufInputSource source( ( const XMLByte* ) sXml.c_str(), sXml.length(), "", false );

    m_pParser->parse( source );

    pDocument = m_pParser->getDocument();
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
  return vxDOMDocument( pDocument );
} // FromXml()


/**
 * Logs and rethrows a xerces XMLException.
 *
 * @param   e         xerces XMLException
 * @param   sClass    class exception was caught
 * @param   sMethod   method exception was caught
 */
void vxDOMDocument::Exception( XMLException & e, const std::string & sClass,  const std::string & sMethod )
{
  // convert from XMLCh * to std::string
  char * sMsg = XMLString::transcode( e.getMessage() );
  std::string sErrorMsg = sMsg;
  XMLString::release( &sMsg );
  
  // wrap the exception and rethrow
  throw ex::VException( LogRec( sMsg, sClass.c_str(), sMethod.c_str() ) );
} // Exception()


/**
 * Logs and rethrows a xerces DOMException.
 *
 * @param   e         xerces DOMException
 * @param   sClass    class exception was caught
 * @param   sMethod   method exception was caught
 */
void vxDOMDocument::Exception(XERCES_CPP_NAMESPACE::DOMException & e, const std::string & sClass, const std::string & sMethod)
{
  // convert from XMLCh * to std::string
  char * sMsg = XMLString::transcode( e.getMessage() );
  std::string sErrorMsg = sMsg;
  XMLString::release( &sMsg );
  
  // wrap the exception and rethrow
  throw ex::VException( LogRec( sMsg, sClass.c_str(), sMethod.c_str() ) );
} // Exception()


/**
 * Assignment operator
 *
 * @param     rhs   right hand side of asignment
 * @return    reference to this instance
 */
vxDOMDocument & vxDOMDocument::operator=( const vxDOMDocument & rhs )
{
  if( this == &rhs )
    return *this;

  m_pDocument = rhs.m_pDocument;

  return *this;
} // operator=()

/**
 * Initializes the xml implementation (needed for xerces)
 */
void vxDOMDocument::Initialize()
{
  try
  {
    XMLPlatformUtils::Initialize();
    m_pParser = new XercesDOMParser();
    m_pParser->setErrorHandler(&m_errorHandler);
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
} // Initialize()


/**
 * Uninitializes the xml implementation (needed for xerces)
 */
void vxDOMDocument::Terminate()
{
  try
  {
    if( m_pParser != NULL )
    {
      delete m_pParser;
      m_pParser = NULL;
    }
    XMLPlatformUtils::Terminate();  
  }
  catch( XMLException & e ) 
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
  catch (XERCES_CPP_NAMESPACE::DOMException & e)
  {
    Exception( e, "vxDOMDocument", "FromXml" );
  }
} // Terminate()



// $Log: vxDOMDocument.C,v $
// Revision 1.3  2007/01/09 19:15:35  jmeade
// add xml version string to output.
//
// Revision 1.2  2006/12/05 20:47:37  jmeade
// ensuring pointers are checked b4 they crash the software.
//
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMDocument.C,v 1.3 2007/01/09 19:15:35 jmeade Exp $
// $Id: vxDOMDocument.C,v 1.3 2007/01/09 19:15:35 jmeade Exp $
