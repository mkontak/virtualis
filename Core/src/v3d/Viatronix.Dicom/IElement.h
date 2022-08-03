#pragma once

//#include "Tag.h"

using namespace System;
using namespace System::Xml;

OPEN_DICOM_NS


public interface class  IElement
{


#pragma region properties

public:
  // Gets the tag
  virtual property String ^ Tag
  {
    String ^ get();
  }

  /// Gets the id
  virtual property int Id
  {
    int get();
  }

   // Gets the group id
  virtual property unsigned short GroupId
  {
    unsigned short get();
  }

  // Gets the element id
  virtual property unsigned short ElementId
  {
    unsigned short get();
  }


  /// Gets/Sets the name
  virtual property String ^ Name
  {
    String ^ get();
  } // Name

  /// Gets/Sets the VR
  virtual property String ^ VR
  {
    String ^ get();
  } // VR

    /// Gets/Sets the Value
  virtual property String ^ Value
  {
    String ^ get();
  } // Value


  /// Gets/Sets the Length
  virtual property unsigned long Length
  {
    unsigned long get();
  } // Length

  /// Gets/Sets the VM
  virtual property unsigned short VM
  {
    unsigned short get();
  } // VM

  //// Gets the sequence flag
  virtual property bool IsSequence
  { 
    bool get();
  }

  //// Gets the private flag
  virtual property bool IsPrivate
  { 
    bool get();
  }

  //// Gets the sub-element flag
  virtual property bool IsSubElement
  { 
    bool get();
  }

  /// Gets the flag indicating we have sub-elements
  virtual property bool HasSubElements
  { 
    bool get();
  }

  /// Gets the private creator
  virtual property String ^ PrivateCreator
  {
    String ^ get();
  } // Value

  /// Expanded flag
  virtual property bool Expanded
  {
    bool get();
    void set(bool);
  }

  virtual property unsigned short Level
  {
    unsigned short get();
  }

#pragma endregion




}; // interface IElement

CLOSE_DICOM_NS