// $Id: Processor.C,v 1.2 2005/09/23 12:09:36 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "Processor.h"
#include "ProcessorEnvironment.h"
#include "FindingCollection.h"
#include "LoggerCom.h"

/**
 * Default Constructor
 */
Processor::Processor()
{
} // Processor()


/**
 * Called after the object is fully instantiated.  Peform all code that would
 * execute in the constrictor here.
 *
 * @return    S_OK on success
 */
HRESULT Processor::FinalConstruct()
{
  CComCreator< CComObject< ProcessorEnvironment > >::CreateInstance( NULL, __uuidof( IProcessorEnvironment ), reinterpret_cast< void ** >( &m_spEnvironment ) );
  CComCreator< CComObject< FindingCollection > >::CreateInstance( NULL, __uuidof( IFindingCollection ), reinterpret_cast< void ** >( &m_spFindings ) );
  CComCreator< CComObject< Logger > >::CreateInstance( NULL, __uuidof( ILogger ), reinterpret_cast< void ** >( &m_spLogger ) );
  return S_OK;
} // FinalConstruct()


/**
 * Called after the final reference to the instance has been released.
 */
void Processor::FinalRelease()
{
  m_spEnvironment = NULL;
  m_spFindings = NULL;
  m_spLogger = NULL;
} // FinalRelease()


/**
 * Gets the start point
 *
 * @param   ppiFindings    upon return contains the findings
 * @return                  S_OK for success
 */
STDMETHODIMP Processor::get_Environment( IProcessorEnvironment ** ppiEnvironment )
{
	if( !ppiEnvironment )
    return E_INVALIDARG;
  *ppiEnvironment = NULL;
  return m_spEnvironment.QueryInterface( ppiEnvironment );
} // get_Environment()


/**
 * Gets the findings
 *
 * @param   ppiFindings    upon return contains the findings
 * @return                  S_OK for success
 */
STDMETHODIMP Processor::get_Findings( IFindingCollection ** ppiFindings )
{
	if( !ppiFindings )
    return E_INVALIDARG;
  *ppiFindings = NULL;
  return m_spFindings.QueryInterface( ppiFindings );
} // get_Findings()


/**
 * Gets the logger
 *
 * @param   ppiLogger    upon return contains the logger
 * @return                  S_OK for success
 */
STDMETHODIMP Processor::get_Logger( ILogger ** ppiLogger )
{
	if( !ppiLogger )
    return E_INVALIDARG;
  *ppiLogger = NULL;
  return m_spLogger.QueryInterface( ppiLogger );
} // get_Logger()


// $Log: Processor.C,v $
// Revision 1.2  2005/09/23 12:09:36  geconomos
// added Logger property
//
// Revision 1.1  2005/09/16 14:48:15  geconomos
// moved from vxCAD
//
// Revision 1.1  2005/09/15 16:38:27  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/Processor.C,v 1.2 2005/09/23 12:09:36 geconomos Exp $
// $Id: Processor.C,v 1.2 2005/09/23 12:09:36 geconomos Exp $
