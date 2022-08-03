#pragma once

using namespace System::Xml;

OPEN_REGISTRY_NS

public ref  class SystemSettingsSectionHandler : public System::Configuration::IConfigurationSectionHandler
{

public:

  // Returns the section
  virtual System::Object ^ Create( System::Object  ^ pParent, System::Object ^ pConfigContext, System::Xml::XmlNode ^pSection )
  {

    XmlAttribute ^ pAttribute = pSection->Attributes["configSoure"];

    if ( pAttribute != __nullptr )
    {
      XmlDocument ^ pDoc(gcnew XmlDocument());

      pDoc->Load(pAttribute->Value->ToString());

      return pDoc->DocumentElement;
    }
    else
    {
      return pSection;
    }
  } // Create( System::Object  * pParent, System::Object ^ pConfigContext, System::Xml::XmlNode ^pSection )


};  // class SystemSettingsSectionHandler 

CLOSE_REGISTRY_NS