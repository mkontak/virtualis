// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmFindParameters.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmQueryRetrieveParameters.h"

namespace dcm
{

/**
 * Implements a class that wraps the find parameters used in a query
 */
class DCM_DLL DcmFindParameters : public DcmQueryRetrieveParameters
{
public:

  /// Constructor
  DcmFindParameters();

  /// Copy Constructor
  DcmFindParameters(const DcmFindParameters & parameters);

  /// Destructor
  virtual ~DcmFindParameters();

  /// Creates the DcmFindParameter from the DcmImage
  static std::shared_ptr<DcmFindParameters> Create(std::shared_ptr<DcmImage> pDcmImage);

  /// Creates a default DcmFinParameters object
  static std::shared_ptr<DcmFindParameters> Create();


  /// Sets the level
  virtual void SetLevel(const std::string & sLevel);


};  // class DcmFindParameters

};  // namespace dcm