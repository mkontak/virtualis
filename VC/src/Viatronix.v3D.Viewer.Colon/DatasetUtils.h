// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatasetUtils.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

#include "ReaderGlobal.h"


OPEN_UTILITIES_NS


class Datasets
{
public:

  /// retruns the locked status of the application
  static inline bool IsLocked() { return ::ReaderLib::rdrGlobal.m_bSPLocked; }

  /// locks/unlocks the datasets
  static void Lock( bool bLocked );

  /// toggles the dataset lock
  static inline void ToggleLock() { Lock( !IsLocked() ); }

private:

  /// private constructor
  Datasets();

}; // class Datasets


CLOSE_UTILITIES_NS