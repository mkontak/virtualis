#include "Stdafx.h"
#include "Enums.h"
#include "ClientEventArgs.h"
#include "DcmCallback.h"
#include "DcmImage.h"
#include "Image.h"
#include "Frame.h"
#include "ImageFactory.h"


USING_DICOM_NS


/**
 * Constructor
 */
ClientEventArgs::ClientEventArgs() :
BaseEventArgs(),
m_iRemaining(0),
m_iWarning(0),
m_iFailed(0),
m_msDestination(String::Empty),
m_msStudyUid(String::Empty),
m_msSeriesUid(String::Empty),
m_msInstanceUid(String::Empty)
{
} // ClientEventArgs()

/**
 * Creates the ClientEventArgs from the calback data structure
 *
 * @param dcmCallbackParametersPtr      Callback data structure
 *
 * @returns Returns the ClientEventArgs object
 */
ClientEventArgs ^ ClientEventArgs::Create(System::IntPtr dcmCallbackParameters, System::IntPtr lpCallbackDataPtr)
{

  ClientEventArgs ^ mpArgs = gcnew ClientEventArgs();

  if ( mpArgs == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ClientEventArgs");

  mpArgs->BaseEventArgs::Initialize(dcmCallbackParameters, lpCallbackDataPtr);

  dcm::PDCM_CALLBACK_PARAMETERS pCallbackParameters = reinterpret_cast<dcm::PDCM_CALLBACK_PARAMETERS>(dcmCallbackParameters.ToPointer());


  switch ( mpArgs->m_commandType )
  {
  case Commands::None:
    break;
  case Commands::Echo:
    break;
  case Commands::Store:
    {
      if ( mpArgs->m_eventType == Events::Progress && pCallbackParameters->Command.Store.Image != __nullptr )
      {
        std::shared_ptr<dcm::IDcmImage> * ptr = new std::shared_ptr<dcm::IDcmImage>(pCallbackParameters->Command.Store.Image);
        mpArgs->m_mpImage = (Viatronix::Dicom::Image ^)Viatronix::Dicom::ImageFactory::Create(System::IntPtr(ptr));
        pCallbackParameters->Command.Store.Image = __nullptr;           // Setting to NULL means that we are taking ownership of the image and it mempory
      }
      else
       mpArgs->m_mpImage = nullptr;

      mpArgs->m_iRemaining  = pCallbackParameters->Command.Store.Remaining;



    }
    break;
  case Commands::Move:
    {
      mpArgs->m_iRemaining    = pCallbackParameters->Command.Move.Remaining;
      mpArgs->m_iWarning      = pCallbackParameters->Command.Move.Warning;
      mpArgs->m_iFailed       = pCallbackParameters->Command.Move.Failed;

      //mpArgs->m_msStudyUid     = gcnew String(pCallbackParameters->Command.Move.StudyUid);
      //mpArgs->m_msSeriesUid    = gcnew String(pCallbackParameters->Command.Move.StudyUid);
      //mpArgs->m_msInstanceUid  = gcnew String(pCallbackParameters->Command.Move.StudyUid);

    }
    break;

  case Commands::Get:
    break;

  case Commands::Find:
    {
      if ( mpArgs->m_eventType == Events::Progress )
      {
        std::shared_ptr<dcm::IDcmImage> * ptr = new std::shared_ptr<dcm::IDcmImage>(pCallbackParameters->Command.Find.Image);
        mpArgs->m_mpImage = (Viatronix::Dicom::Image ^)Viatronix::Dicom::ImageFactory::Create(IntPtr(ptr));
        pCallbackParameters->Command.Find.Image = __nullptr;
        mpArgs->m_msQueryResults = ( pCallbackParameters->Command.Find.Results == __nullptr ? String::Empty : gcnew String(pCallbackParameters->Command.Find.Results));
        mpArgs->m_msQueryLevel = ( pCallbackParameters->Command.Find.Parameters == __nullptr ? String::Empty : gcnew String( pCallbackParameters->Command.Find.Parameters->GetLevel().c_str()));
      }
      else
       mpArgs->m_mpImage = nullptr;

    }
    break;


  }

  return mpArgs;

} // Craete(System::IntPtr dcmCallbackParametersPtr)

