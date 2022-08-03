// $Id: ProcessorFactory.C,v 1.5 2007/03/07 15:59:39 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#include "StdAfx.h"
#include "ProcessorFactory.h"
#include "Processor.h"


/**
 * Creates a new Processor instance and returns it's IProcessor interface 
 *
 * @param   ppiProcessor  upon return contains an IProcessor interface
 * @return  S_OK on success
 */
HRESULT ProcessorFactory::Create( void ** ppiProcessor )
{
  HRESULT hr( S_OK );

  // validate the inputs
  if( !ppiProcessor )
    return E_INVALIDARG;
  *ppiProcessor = NULL;

  // create an instance of the processor object
  if( FAILED( hr = CComCreator< CComObject< Processor > >::CreateInstance( NULL, __uuidof( IProcessor ), ppiProcessor ) ) )
    return hr;
  return S_OK;
} // Create( IProcessor ** ppiProcessor )


// $Log: ProcessorFactory.C,v $
// Revision 1.5  2007/03/07 15:59:39  gdavidson
// Code review
//
// Revision 1.4  2007/03/01 20:40:36  geconomos
// code review preparation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/ProcessorFactory.C,v 1.5 2007/03/07 15:59:39 gdavidson Exp $
// $Id: ProcessorFactory.C,v 1.5 2007/03/07 15:59:39 gdavidson Exp $
