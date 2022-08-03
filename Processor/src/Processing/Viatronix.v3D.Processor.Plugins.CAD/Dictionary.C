// $Id: Dictionary.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )


#include "StdAfx.h"
#include "Dictionary.h"


/**
 * Default Constructor
 */
Dictionary::Dictionary() : m_pMap ( NULL )
{
} // Dictionary()


/**
 * Adds the specified key / value pair to the dictionary
 *
 * @param     bsKey     key
 * @param     bsValue   value to assciate with key
 * @return    S_OK on success
 */
STDMETHODIMP Dictionary::Add( BSTR bsKey, BSTR bsValue )
{
  USES_CONVERSION; 

  std::string sKey( W2A( bsKey ) );

  DictionaryMap::const_iterator iter = m_pMap->find( sKey );
  if( iter != m_pMap->end() )
    return E_INVALIDARG;

  m_pMap->insert( DictionaryMap::value_type( sKey, W2A( bsValue ) ) );

  return S_OK;
} // Add( BSTR sKey, BSTR sValue )


/**
 * Removes the value asssociated with the specified key from the dictionary
 * @param    bsKey     key whose associated value is to be removed
 * @return   S_OK on success
 */
STDMETHODIMP Dictionary::Remove( BSTR bsKey )
{
  USES_CONVERSION; 
  std::string sKey( W2A( bsKey ) );

  DictionaryMap::const_iterator iter = m_pMap->find( sKey );
  if( iter == m_pMap->end() )
    return E_INVALIDARG;

  m_pMap->erase( sKey );

  return S_OK;
} // Remove( BSTR sKey )


/**
 * Gets the value asscoiated with the specified key
 * @param     bsKey      key
 * @param     pbsValue   upon return contains the value associated with the key
 * @return    S_OK on success
 */
STDMETHODIMP Dictionary::GetValue( BSTR bsKey, BSTR * pbsValue )
{
  USES_CONVERSION;

  if( !pbsValue )
    return E_INVALIDARG;
  *pbsValue = NULL;

  DictionaryMap::const_iterator iter = m_pMap->find( W2A( bsKey ) );
  if( iter == m_pMap->end() )
    return E_INVALIDARG;

  return CComBSTR( iter->second.c_str() ).CopyTo( pbsValue );
} // GetValue( BSTR sKey, BSTR * psValue )


/**
 * Gets if the specified key is contained in the dictionary
 * @param     bsKey         key
 * @param     pConatins   upon return contains if the specified key is contained in the dictionary
 * @return    S_OK on success
 */
STDMETHODIMP Dictionary::Contains( BSTR bsKey,  VARIANT_BOOL * pbContains )
{
  USES_CONVERSION;
  if( !pbContains )
    return E_INVALIDARG;
  
  *pbContains = m_pMap->find( W2A( bsKey ) ) != m_pMap->end()? VARIANT_TRUE : VARIANT_FALSE;

  return S_OK;
} // Contains( BSTR sKey,  VARIANT_BOOL * pbContains )



// $Log: Dictionary.C,v $
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2005/12/08 16:59:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Dictionary.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
// $Id: Dictionary.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
