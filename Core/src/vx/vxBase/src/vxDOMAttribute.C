// $Id: vxDOMAttribute.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "stdafx.h"
#include "vxDOMAttribute.h"
#include "vxDOMDocument.h"

// namesapce declarations
using namespace XERCES_CPP_NAMESPACE;


/** 
 * Default constructor
 */
vxDOMAttribute::vxDOMAttribute() : m_pAttribute( NULL )
{
} // vxDOMAttribute()


/**
 * Private constructor
 *
 * @param pAttribute xerce attribute implementation
 */
vxDOMAttribute::vxDOMAttribute( XERCES_CPP_NAMESPACE::DOMAttr * pAttribute ) : 
  vxDOMNode( ( XERCES_CPP_NAMESPACE::DOMNode * )( void* ) pAttribute ),
  m_pAttribute( pAttribute )
{
} // vxDOMAttribute


/**
 * Gets the name of the attribute 
 *
 * @return  attribute name
 */
std::string vxDOMAttribute::GetName() const
{
  std::string sName;
  try
  {
    char * name = XMLString::transcode( m_pAttribute->getName() );
    sName = name;
    XMLString::release( &name );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetName" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetName" );
  }
  return sName;
} // GetName()


/**
 * Gets a value indicating if the node's value is set in the document or not 
 *
 * @return  true if the value has been set
 */
bool vxDOMAttribute::GetSpecified() const
{
  bool bSpecified( false );
  try
  {
    bSpecified = m_pAttribute->getSpecified();
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetSpecified" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetSpecified" );
  }
  return bSpecified;
} // GetSpecified()


/**
 * Gets the value of the attribute 
 *
 * @return  attribute value
 */
std::string vxDOMAttribute::GetValue() const
{
  std::string sValue;
  try
  {
    char * value = XMLString::transcode( m_pAttribute->getValue() );
    sValue = value;
    XMLString::release( &value );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetValue" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "GetValue" );
  }
  return sValue;
} // GetValue()


/**
 * Sets the value of the attribute 
 *
 * @param     sValue     Value of the attribute
 */
void vxDOMAttribute::SetValue( const std::string & sValue )
{
  try
  {
    XMLCh * value = XMLString::transcode( sValue.c_str() );
    m_pAttribute->setValue( value );
    XMLString::release( &value );
  }
  catch( XERCES_CPP_NAMESPACE::XMLException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "SetValue" );
  }
  catch( XERCES_CPP_NAMESPACE::DOMException & e )
  {
    vxDOMDocument::Exception( e, "vxDOMAttribute", "SetValue" );
  }
} // SetValue()


/**
 * Assignment operator
 *
 * @param     rhs   right hand side of asignment
 * @return    reference to this instance
 */
vxDOMAttribute & vxDOMAttribute::operator=( const vxDOMAttribute & rhs )
{
  if( this == &rhs )
    return *this;

  vxDOMNode::operator=( rhs );

  m_pAttribute = rhs.m_pAttribute;

  return *this;
} // operator=()


// $Log: vxDOMAttribute.C,v $
// Revision 1.1  2005/04/12 15:20:57  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxDOMAttribute.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
// $Id: vxDOMAttribute.C,v 1.1 2005/04/12 15:20:57 geconomos Exp $
