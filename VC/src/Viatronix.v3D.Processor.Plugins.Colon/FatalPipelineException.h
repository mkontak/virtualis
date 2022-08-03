// $Id: FatalPipelineException.h,v 1.3 2007/03/06 21:42:50 gdavidson Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: David dave@viatronix.net
//
// Complete history on bottom of file


#ifndef FatalPipelineException_h
#define FatalPipelineException_h

#include "Global.h"

namespace ex
{

  /**
   * Pipeline execution must be aborted due occurence of very very highly unlikely circumstances.
   */
  class FataPipelineException : public RuntimeException 
  {
  public:

    /// constructor
    FataPipelineException(const vxStrings eString) : RuntimeException(eString) { }

    /// constructor
    FataPipelineException(const std::string & desc = "") : RuntimeException(desc) { }
  };
} // namespace ex

// $Log: FatalPipelineException.h,v $
// Revision 1.3  2007/03/06 21:42:50  gdavidson
// Added comments
//
// Revision 1.2  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:45  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:37:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:24:00   ingmar
// Initial revision.
// Revision 1.1  2001/05/29 01:38:51  dave
// added
//
// $Id: FatalPipelineException.h,v 1.3 2007/03/06 21:42:50 gdavidson Exp $