// Pragmas
#pragma once

#include "Enums.h"

// Namespaces
using namespace System;
using namespace Viatronix::Enterprise::Processing;

OPEN_CONVERSION_NS


/**
 * Implements a basis set of thread event arguments.
 */
 public ref class PipelineEventArgsFactory :  PipelineEventArgs 
{


public:


  //// Creates the client args using the callback data
  static PipelineEventArgs ^ Create(System::IntPtr cvnCallbackParametersPtr);




};  // class PipelineEventArgsFactory


CLOSE_CONVERSION_NS
