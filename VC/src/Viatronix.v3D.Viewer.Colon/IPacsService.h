// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: IPacsService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net


#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Pdf.h"
#include <GdiPlusHeaders.h>


/**
 * PACS service implementation
 */
class IPacsService
{
public:

  typedef std::vector< std::shared_ptr< Gdiplus::Bitmap >> Images;

  // destructor
  virtual ~IPacsService() = 0 {}
  
  // send to pacs
  virtual void Send( const std::string & uid, std::shared_ptr<pdf::Pdf> & pdf, Images images ) = 0;

}; // class IPacsService

