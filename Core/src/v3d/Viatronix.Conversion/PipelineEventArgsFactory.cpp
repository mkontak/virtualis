#include "Stdafx.h"
#include "PipelineEventArgsFactory.h"
#include "CvnCallback.h"



USING_CONVERSION_NS




/**
 * Creates the ClientEventArgs from the callback data structure
 *
 * @param dcmCallbackParametersPtr      Callback data structure
 *
 * @returns Returns the ClientEventArgs object
 */
PipelineEventArgs ^ PipelineEventArgsFactory::Create(System::IntPtr cvnCallbackParameters)
{

  PipelineEventArgs ^ mpArgs = gcnew PipelineEventArgs();

  if ( mpArgs == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate PipelineEventArgs");

  cvn::PCVN_CALLBACK_PARAMETERS pCallbackParameters = reinterpret_cast<cvn::PCVN_CALLBACK_PARAMETERS>(cvnCallbackParameters.ToPointer());

  if ( pCallbackParameters != __nullptr )
  {

    mpArgs->SeriesUid       = ( pCallbackParameters->SeriesUid != __nullptr ? gcnew String(pCallbackParameters->SeriesUid): String::Empty );

    /// Stage
    switch ( pCallbackParameters->StageType )
    {

    case cvn::CvnStages::S_NONE:
      mpArgs->Stage = "None";
      break;

    case cvn::CvnStages::S_LOADING:
      mpArgs->Stage = "Loading";
      break;

    case cvn::CvnStages::S_CREATION:
      mpArgs->Stage = "Creation";
      break;

    case cvn::CvnStages::S_RETRIEVAL:
      mpArgs->Stage = "Retreival";
      break;

    default:
      mpArgs->Stage = "";
      break;
    };

    mpArgs->ModuleName       = "conversion";
    mpArgs->Remaining        = pCallbackParameters->Remaining;
    mpArgs->Completed        = pCallbackParameters->Completed;
    mpArgs->Total            = mpArgs->Remaining + mpArgs->Completed;
    mpArgs->Message         = ( pCallbackParameters->Message != __nullptr ? gcnew String(pCallbackParameters->Message) : String::Empty );

    if ( mpArgs->Total > 0 )
      mpArgs->Progress  = (float)mpArgs->Completed / (float)mpArgs->Total  * 100.0F;

  }
  else
    Viatronix::Logging::Log::Error("Callback parameters are null", "PipelineEventArgsFactory", "Create");

  return mpArgs;

} // Create(System::IntPtr dcmCallbackParametersPtr)
