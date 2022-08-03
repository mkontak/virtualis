#pragma once


#include "LoggerElementCollection.h"
#include "LoggerElement.h"

using namespace System;
using namespace System::Configuration;



OPEN_LOGGING_NS



/**
 * Implements a loggers section handler
 */
public ref class LoggersSection :  ConfigurationSection
{


public:

  /// Constructor
  static LoggersSection()
  {

    /// Section
    m_mpSection = nullptr;

    /// Elements
    m_mpLists = gcnew ConfigurationProperty("", LoggerElementCollection::typeid, nullptr, ConfigurationPropertyOptions::IsRequired | ConfigurationPropertyOptions::IsDefaultCollection);

    /// Properties
    m_mpProprietes = gcnew ConfigurationPropertyCollection();

    /// Add element list as a property
    m_mpProprietes->Add(m_mpLists);

  } // LoggersSection()

 
  /// Gets the loggers section using the default name
  static LoggersSection ^ GetSection()
  { return GetSection("loggers"); }

  /// Gets the loggers section using the supplies name
  static LoggersSection ^ GetSection(String ^ msName);


public:

  /// Gets the logger element collection
  property LoggerElementCollection ^ Lists
  {
    LoggerElementCollection ^ get()
    { return (LoggerElementCollection ^)this[m_mpLists]; }
  }


  /// Gets the 
  property LoggerElement ^ default[String ^]
  {
    LoggerElement ^ get(String ^ msName)
    { return (LoggerElement ^)Lists[msName]; }
  }

protected:

  /// Gets the properties
  property virtual ConfigurationPropertyCollection ^ Properties
  {
    ConfigurationPropertyCollection ^ get() override 
    { return m_mpProprietes; }
  }

private:
    
  /// Section
  static LoggersSection ^ m_mpSection;

  /// Properties collection
  static ConfigurationPropertyCollection ^ m_mpProprietes;

  /// Properties list
  static ConfigurationProperty ^ m_mpLists;

}; // class LoggersSection



  


CLOSE_LOGGING_NS