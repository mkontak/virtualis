// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: IToolTipProvider.h
// Owner: Greg Davidson  

#pragma once

#include <string>

OPEN_WINUTILS_NS


class IToolTipProvider
{
public:
	/// get tooltip
  virtual std::string GetToolTip() = 0;

};

CLOSE_WINUTILS_NS