// $Id: LicenseValidator.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragma declarations
#pragma once

// include declarations
#include "LicenseRequestInfo.h"
#include "Thread.h"

/// import decarations
#import "..\..\..\libs\tlb\include\Viatronix.v3D.Licensing.tlb" no_namespace named_guids raw_interfaces_only

// class declaration
class LicenseValidator : public Thread
{
// member variables
public:

  /// constructor
  LicenseValidator();
  
  /// destructor
  virtual ~LicenseValidator();

  /// requests the license
  bool Request( const LicenseRequestInfo & requestInfo, bool bStartThread = true );

  /// releases the license
  bool Release();

  /// gets the last error message
  std::string GetLastErrorMessage() const;

protected:

  /// license renewal thread
  void Run();

private:

  /// logs the last licensing error
  void LogLastError( const std::string & sMsg, const std::string & sMethod, const HRESULT hr );

// member variables
private:

  /// licensing client
  CComPtr< _UnmanagedLicenseClient > m_spLicenseClient;

  /// used to marshal the licensing instance
  CComPtr< IStream > m_spMarshalStream;
   
  /// abort handle
  HANDLE m_hAbortRenewalThread;

}; // class LicenseValidator


// $Log: LicenseValidator.h,v $
// Revision 1.3  2006/06/20 19:48:03  gdavidson
// Changed import for Viatronix.v3D.Licensing
//
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.1  2006/01/16 15:42:28  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LicenseValidator.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
// $Id: LicenseValidator.h,v 1.3 2006/06/20 19:48:03 gdavidson Exp $
