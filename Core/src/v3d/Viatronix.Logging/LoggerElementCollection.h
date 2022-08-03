#pragma once

#include "LoggerElement.h"

using namespace System;
using namespace System::Configuration;

OPEN_LOGGING_NS

/**
 * Implements a element collection class for the loggers
 */
public ref class LoggerElementCollection : ConfigurationElementCollection
{

#pragma region properties

protected:

  /// Gets the element name
  property virtual String ^ ElementName
  {
    String ^ get() override 
    { return gcnew String("logger"); }
  }

  /// Gets the properties
  property virtual ConfigurationPropertyCollection ^ Properties
  {
    ConfigurationPropertyCollection ^get() override
    { return gcnew ConfigurationPropertyCollection(); }
  }


public:

  /// Gets the Collection type
  property virtual ConfigurationElementCollectionType CollectionType
  {
    ConfigurationElementCollectionType get() override 
    { return ConfigurationElementCollectionType::BasicMap; }
  }

  /// Gets the logger at the specified index
  property LoggerElement ^ default[int]
  {
    LoggerElement^ get(int index)
    { return (LoggerElement^)BaseGet(index); }

    void set(int index, LoggerElement ^ mpElement)
    {
      if (BaseGet(index) != nullptr)
      {
        BaseRemoveAt(index);
      }
      BaseAdd(index, mpElement);
    }
  }

  /// Gets the named logger
  property LoggerElement ^ default[String ^]
  {
    LoggerElement ^ get(String ^ msName) new 
    { return (LoggerElement ^ )BaseGet(msName); }
  }

#pragma endregion


#pragma region methods


protected:

  /// Creates a new element for this collection
  virtual ConfigurationElement^ CreateNewElement() override 
  { return gcnew LoggerElement(); }

  /// Gets the key for the supplied element
  virtual Object ^ GetElementKey(ConfigurationElement ^ mpElement) override;

public:

  /// Adds the logger
  void Add(LoggerElement ^ mpItem)
  {  BaseAdd(mpItem); }

  /// Removes the specified item
  void Remove(LoggerElement ^ mpItem)
  { BaseRemove(mpItem); }

  /// Removes the item at the specified index
  inline void RemoveAt(int index)
  { BaseRemoveAt(index); }

  /// Clears the collection
  inline void Clear()
  { BaseClear(); }
 
#pragma endregion

}; // class LoggerElementCollection



  CLOSE_LOGGING_NS
