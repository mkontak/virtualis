// $Id: ThinClientLoadParams.h,v 1.2 2007/03/09 00:11:04 jmeade Exp $
//
// Copyright© 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson (gdavidson@viatronix.com)

#pragma once

// includes
#include "StudyLoadParams.h"


class ThinClientLoadParams
{
public:

  /// constructor
  ThinClientLoadParams( const std::string & sFilename );

  /// parses the thin client load params
  bool ParseParams();

  /// gets the load parameters
  ReaderLib::StudyLoadParams & GetStudyLoadParams() { return m_params; }

private:

  /// thin client load params
  std::string m_sFilename;

  /// return results
  ReaderLib::StudyLoadParams m_params;

}; // class ThinClientLoadParams
 

// $Log: ThinClientLoadParams.h,v $
// Revision 1.2  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.1  2006/11/11 01:36:42  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ThinClientLoadParams.h,v 1.2 2007/03/09 00:11:04 jmeade Exp $
// $Id: ThinClientLoadParams.h,v 1.2 2007/03/09 00:11:04 jmeade Exp $
