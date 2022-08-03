#include "Stdafx.h"
#include "Enums.h"
#include "ServerEventArgs.h"
#include "DcmCallback.h"
#include "DcmImage.h"
#include "QueryParameters.h"


using namespace System::Text;
using namespace System::IO;
using namespace System::Xml;
USING_DICOM_NS

/**
 * 
 */
ServerEventArgs::ServerEventArgs() :
BaseEventArgs(),
m_msHeader(String::Empty),
m_mpVersion("3.0"),
m_mpQueryParameters(gcnew Viatronix::Dicom::QueryParameters())
{
}


/**
 * Creates the server event args from the callback parameters
 *
 * @param dcmCallbackParametersPtr      Callback parameters
 *
 * @return Returns the ServerEventArgs
 */
ServerEventArgs ^ ServerEventArgs::Create(System::IntPtr dcmCallbackParametersPtr, System::IntPtr lpCallbackDataPtr) 
{
  ServerEventArgs ^ mpArgs = gcnew ServerEventArgs();

  if ( mpArgs == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ServerEventArgs");

  mpArgs->BaseEventArgs::Initialize(dcmCallbackParametersPtr, lpCallbackDataPtr);

  dcm::PDCM_CALLBACK_PARAMETERS pParameters = reinterpret_cast<dcm::PDCM_CALLBACK_PARAMETERS>(dcmCallbackParametersPtr.ToPointer());


  if ( mpArgs->m_eventType == Events::Progress )
  {
    switch ( mpArgs->m_commandType )
    {
    case Commands::Store:
      mpArgs->m_msHeader = ( pParameters->Command.Store.Image != __nullptr ? gcnew String(pParameters->Command.Store.Image->GetHeader(true).c_str()) : String::Empty );
      break;
    case Commands::Move:
      mpArgs->m_mpQueryParameters = Viatronix::Dicom::QueryParameters::Create(pParameters->Command.Move.Parameters);
      break;
    case Commands::Find:
      mpArgs->m_mpQueryParameters = Viatronix::Dicom::QueryParameters::Create(pParameters->Command.Find.Parameters);
      break;
    }
  }
  return mpArgs;

} // Create(System::IntPtr dcmCallbackParametersPtr)

/**
 * Generate the image record for insertion into the DB
 *
 * @return  Xml string
 */
String ^ ServerEventArgs::ToXml()
{

  MemoryStream ^ mpStream = gcnew MemoryStream();

  XmlTextWriter ^ mpWriter = gcnew XmlTextWriter(gcnew StreamWriter(mpStream));

  mpWriter->WriteStartElement("image");


  mpWriter->WriteAttributeString("server", System::Environment::MachineName);
  mpWriter->WriteAttributeString("calledAETitle", this->CalledAETitle);
  mpWriter->WriteAttributeString("callingAETitle", this->CallingAETitle );
  mpWriter->WriteAttributeString("assoc", this->Id);
  mpWriter->WriteAttributeString("ver", m_mpVersion);


  mpWriter->WriteStartElement("hdr");
  mpWriter->WriteRaw(this->Header);
  mpWriter->WriteEndElement();

  mpWriter->WriteEndElement();

  mpWriter->Flush();

  ASCIIEncoding ^ mpEncoding = gcnew ASCIIEncoding();

  return mpEncoding->GetString(mpStream->ToArray());



} // ToXml()

