// Copyright© 2013, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen dongqing@viatronix.net

#pragma once

#include "CvnLog.h"


namespace cvn
{
  // Forward declarations
  class CVN_DLL CvnUnitTest;

  /**
   * Implements unit test on colon digital phantom.
   */
  class CVN_DLL CvnUnitTest 
  {
	public:
    /// Constructor
    CvnUnitTest( const std::string & sVolumeFilePath );

    /// Destructor
    ~CvnUnitTest();

    /// Run the test
		void Run();

  private:
    /// Bias used for pixel data of the vx block volume
    const std::string & m_sVolumeFilePath;

  };  // class CvnUnitTest

};  // namespace cvn