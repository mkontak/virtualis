
#pragma once

#include "LogEnums.h"

using namespace System;
using namespace System::Configuration;


OPEN_LOGGING_NS


/**
 * Implements a logger element in the configuration file
 */
public ref class LoggerElement :  ConfigurationElement
{

public:

  /// Constructor
  static LoggerElement()
  {

    m_mpName        = gcnew ConfigurationProperty("name",   String::typeid, nullptr, ConfigurationPropertyOptions::IsKey | ConfigurationPropertyOptions::IsRequired );
    m_mpFile        = gcnew ConfigurationProperty("file",   String::typeid, nullptr, ConfigurationPropertyOptions::None  );
    m_mpLevel       = gcnew ConfigurationProperty("level",  String::typeid, nullptr, ConfigurationPropertyOptions::IsRequired );
    m_mpFormat      = gcnew ConfigurationProperty("format",  String::typeid, nullptr, ConfigurationPropertyOptions::None );
    m_mpServer      = gcnew ConfigurationProperty("server", String::typeid, nullptr, ConfigurationPropertyOptions::None );
    m_mpPort        = gcnew ConfigurationProperty("port",   String::typeid, nullptr, ConfigurationPropertyOptions::None );
    m_mpDefault     = gcnew ConfigurationProperty("default", String::typeid, nullptr );

    m_mpProperties = gcnew ConfigurationPropertyCollection();
    m_mpProperties->Add(m_mpName);
    m_mpProperties->Add(m_mpFile);
    m_mpProperties->Add(m_mpLevel);
    m_mpProperties->Add(m_mpFormat);
    m_mpProperties->Add(m_mpServer);
    m_mpProperties->Add(m_mpPort);
    m_mpProperties->Add(m_mpDefault);


  } // LoggerElement()

public:

  // Gets the log name
  property String ^ Name
  {
    String ^ get() { return (String ^)this[m_mpName]; }
  } // Name

  // Gets the file (Account for NULL since it is not a required field )
  property String ^ File
  {
    String ^ get() { return ( this[m_mpFile] == nullptr ? String::Empty : (String ^)this[m_mpFile] ); }
  } // File

  // Gets the logging level
  property LogLevel Level
  {
    LogLevel get() { return (LogLevel)Enum::Parse(LogLevel::typeid, (String ^)this[m_mpLevel]); }
  } // LogLevel

  // Gets the default flag
  property bool Default
  {
    bool get() { return System::Convert::ToBoolean((String ^)this[m_mpDefault]); }
  } // Default

  /// Gets the server
  property String ^ Server
  {
    String ^ get() { return ( this[m_mpServer] == nullptr ? String::Empty : (String ^)this[m_mpServer] ); }
  } // Server

  property unsigned int Port
  {
    unsigned int get() { return ( this[m_mpPort] == nullptr ? 0 : UInt32::Parse((String ^)this[m_mpPort]) ); }
  }


  // Gets the Format
  property MessageFormat Format
  {
    MessageFormat get() 
    {
      String ^ sFormat = (String ^)this[m_mpFormat];
      return  (( sFormat != nullptr && sFormat->Length > 0 ) ? (MessageFormat)Enum::Parse(MessageFormat::typeid, sFormat) : MessageFormat::Default);
     }
  } // Format

protected:

  virtual property ConfigurationPropertyCollection ^ Properties 
  {
    ConfigurationPropertyCollection ^ get() override { return m_mpProperties; }
  } // Properties

private:

  // Logger name
  static ConfigurationProperty ^ m_mpName;

  // Logger file
  static ConfigurationProperty ^ m_mpFile;

  // Logging level
  static ConfigurationProperty ^ m_mpLevel;

  // Message Format
  static ConfigurationProperty ^ m_mpFormat;

  // Default logger flag
  static ConfigurationProperty ^ m_mpDefault;

  // Server name/ip
  static ConfigurationProperty ^ m_mpServer;

  // Server poty
  static ConfigurationProperty ^ m_mpPort;


  // Property collection
  static ConfigurationPropertyCollection ^ m_mpProperties;


}; // class LoggerElement


CLOSE_LOGGING_NS

