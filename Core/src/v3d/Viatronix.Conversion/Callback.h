// $Id: DicomEvents.h,v 1.6 2006/06/01 12:20:45 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

// Pragmas
#pragma once


// Namespaces
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

OPEN_CONVERSION_NS

  

// ================================
// Event types
// ================================
public enum class EventTypes
{
  Started,
  Updated,
  Failed,
  Completed
};  // EventTypes

/// Thread Event Handler Delegate
public delegate void CallbackDelegate(EventTypes eType, int iRemaining, int iCompleted, String ^ msMessage);


CLOSE_CONVERSION_NS
