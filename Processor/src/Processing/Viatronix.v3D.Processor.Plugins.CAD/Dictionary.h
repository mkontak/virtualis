// $Id: Dictionary.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// include declarations
#include "vxComCAD.h"

// class definition
class ATL_NO_VTABLE Dictionary : 
  public CComObjectRootEx< CComSingleThreadModel >,
  public CComCoClass< Dictionary >,
  public IDispatchImpl< IDictionary, &__uuidof( IDictionary) >
{
/// ATL macros and defines
public:
	
// protects your object from being deleted if the reference count decrements the count to 0 during constructions
DECLARE_PROTECT_FINAL_CONSTRUCT()

// indicates coclass is not self-registering
DECLARE_NO_REGISTRY()

// indicates object cannot be aggregated
DECLARE_NOT_AGGREGATABLE( Dictionary )

// interface map
BEGIN_COM_MAP( Dictionary )
  COM_INTERFACE_ENTRY( IDictionary )
  COM_INTERFACE_ENTRY( IDispatch )
  COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

// IDicionary implementation
public:

  /// adds the specified key / value pair to the dictionary
  STDMETHOD( Add )( BSTR bsKey, BSTR bsValue );

  /// removes the value asssociated with the specified key from the dictionary
  STDMETHOD( Remove )( BSTR bsKey );

  /// gets the value asscoiated with the specified key
  STDMETHOD( GetValue )( BSTR bsKey, BSTR * pbsValue );

  /// gets if the specified key is contained in the dictionary
  STDMETHOD( Contains )( BSTR sbKey,  VARIANT_BOOL * pbContains );


// member functions
public:
  
  /// map typedef 
  typedef std::map< std::string, std::string >  DictionaryMap;
  
  /// default constructor
  Dictionary();

  /// gets the internal vector
  const DictionaryMap * GetInternalPtr() const { return m_pMap; }

  /// sets the internal vector
  void SetInternalPtr( DictionaryMap * pMap ) { m_pMap = pMap; }


// member variables
private:

  /// map
  DictionaryMap * m_pMap;

}; // coclass Dictionary

// $Log: Dictionary.h,v $
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2005/12/08 16:59:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Dictionary.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
// $Id: Dictionary.h,v 1.2 2006/01/27 20:31:37 jmeade Exp $
