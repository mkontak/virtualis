#pragma once


namespace Viatronix { namespace IO {

public ref class TransportLayer
{
private:

  TransportLayer() {}

public:

  // Sets the transport layer using the settings string
  static void Initialize ( System::String ^ msSettings );

  // Sets the transport layer using the xml node
  static void Initialize ( System::Xml::XmlNode ^ mpNode );
  
};  // class TreansportLayer

} }