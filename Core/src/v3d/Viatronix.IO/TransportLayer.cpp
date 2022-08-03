#include "Stdafx.h"
#include "TransportLayer.h"
#include "vxTransportLayer.h"


using namespace Viatronix::IO;

void TransportLayer::Initialize( System::String ^ msSettings )
{
    /// Sets the transport layer
  io::TransportLayer::GetCurrentTransportLayer().Initialize(ss(msSettings));

}

void TransportLayer::Initialize( System::Xml::XmlNode ^ mpNode )
{
    /// Sets the transport layer
  io::TransportLayer::GetCurrentTransportLayer().Initialize(ss(mpNode->OuterXml));

}