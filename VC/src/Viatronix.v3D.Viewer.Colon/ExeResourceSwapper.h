// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ExeResourceSwapper.h
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


#pragma once


/**
 * Switches to the resources in the exe
 */
class ExeResourceSwapper
{
public:

  /// constructor
  ExeResourceSwapper( bool bUseSatellite = false );

  /// destructor
  ~ExeResourceSwapper();

private:

  /// current resource handle
  HINSTANCE m_currentResource;

};