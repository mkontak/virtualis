// $Id: conversion.h,v 1.1 2006/02/09 19:27:54 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

// pragma declarations
#pragma once

/*
 *Supports different conversions
 */
class Conversion
{
private:
  ///Private constructor
  Conversion(void){}

  ///Private destructor
  ~Conversion(void){};

public:
  ///Static method converts UNC path to local path 
  static std::string UNC2Local( const std::string & sUNCPath );
};


// $Log: conversion.h,v $
// Revision 1.1  2006/02/09 19:27:54  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/conversion.h,v 1.1 2006/02/09 19:27:54 romy Exp $
// $Id: conversion.h,v 1.1 2006/02/09 19:27:54 romy Exp $