//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Mark Kontak (mailto:mkontak@viatronix.com)


// pragmas
#pragma once

#include "Registry.h"

namespace sys
{




#pragma region class SoftwareRegistry32

  /*
  *  Implements a software registry class
  */
  class VX_BASE_DLL SoftwareRegistry32 : public Registry32
  {
  protected:

    SoftwareRegistry32() { }

    // functions
  public:

    /// Constructor 
    SoftwareRegistry32(const std::string & sSubKey, AccessOptions access = AccessOptions::ACCESS_OPTION_ALL) :
      Registry32(HKEY_LOCAL_MACHINE, util::Strings::Format("SOFTWARE\\%s", sSubKey.c_str()), access) { }

    // Destructor
    virtual ~SoftwareRegistry32() {}

  }; // class SoftwareRegistry32

#pragma endregion


#pragma region class SoftwareRegistry64

  /*
  *  Implements a software registry class
  */
  class VX_BASE_DLL SoftwareRegistry64 : public Registry64
  {
  protected:

    SoftwareRegistry64() { }

    // functions
  public:

    /// Constructor 
    SoftwareRegistry64(const std::string & sSubKey, AccessOptions access = AccessOptions::ACCESS_OPTION_ALL) :
      Registry64(HKEY_LOCAL_MACHINE, util::Strings::Format("SOFTWARE\\%s", sSubKey.c_str()), access) { }

    // Destructor
    virtual ~SoftwareRegistry64() {}

  }; // class SoftwareRegistry64

#pragma endregion


} // namespace sys


