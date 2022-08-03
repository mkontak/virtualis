// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DicomPacsService.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "IPacsService.h"


class DicomPacsService : public IPacsService
{
public:

  // destructor
  virtual ~DicomPacsService();
  
  // Acquires a context
  void Send( const std::string & uid, std::shared_ptr<pdf::Pdf> & pPdf, Images images );

}; //class DicomPacsService

