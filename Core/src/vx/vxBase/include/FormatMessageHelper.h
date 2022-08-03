// $Id: FormatMessageHelper.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#pragma once


/// class delcaration
class FormatMessageHelper
{
// member functions
public:

  /// gets the message asscociated with the specified last error code
  static std::string Format( DWORD dwLastError = GetLastError() );

}; // class FormatMessageHelper


// $Log: FormatMessageHelper.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/FormatMessageHelper.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: FormatMessageHelper.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
