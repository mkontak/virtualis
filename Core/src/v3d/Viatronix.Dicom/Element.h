
#pragma once

/// Includes
#include "scoped_ptr.h"
#include "IElement.h"



/// Using
using namespace System;
using namespace System::Xml;
using namespace GalaSoft::MvvmLight;
using namespace GalaSoft::MvvmLight::Helpers;
using namespace GalaSoft::MvvmLight::Command;
using namespace GalaSoft::MvvmLight::Messaging;
using namespace GalaSoft::MvvmLight::Threading;

 


OPEN_DICOM_NS

/**
 * Implements a DICOM element used for extract and insertion into an ElementCollection. Inherits from the 
 * Tag object which implements the element tag containing a group number and element number.
 */
 public ref class Element : public IComparable,  public IElement, public ViewModelBase
{

#pragma region constructors/destructors

protected:

  /// Constructor
  Element();

  /// Copy constructor
  Element(Element ^ mpElement)
  { }

public:

  /// Element constructir
  Element(unsigned short iGroup, unsigned short iElement, String ^ msValue);

  /// Destructors
  ~Element();

  /// Finalizer
  !Element();

#pragma endregion

#pragma region methods

public:

  /// Compares the objects
  virtual int CompareTo(Object ^ mpOther) 
  { return ( Equals(mpOther) ? 0 : ( ((Element ^)mpOther)->Id < Id ? -1 : 1 ) ); } 


  /// Returns the Hash code used in lists
  virtual int GetHashCode() override
  { return Id; }

  /// Returns true if the elements are the same
  virtual bool Equals(Object ^ mpObject) override
  { return Equals(dynamic_cast<Element ^>(mpObject)); }

  /// Returns true if the elements are the same
  bool Equals(Element ^ mpElement)
  { return ( mpElement != nullptr && mpElement->Id == Id ); }


  /// Returns the string representation if the Element
  virtual String ^ ToString() override;

  /// Gets the internal shared_ptr
  System::IntPtr GetDcmElementPtr();

#pragma region create methods


public:


  /// Create the element
  static Element ^ Create(unsigned short iGroup, unsigned short iElement, String ^ msValue);

   /// Create the element
  static Element ^ Create(System::IntPtr dcmElementPtr);

#pragma endregion

#pragma endregion

#pragma region IElement

public:

  /// Gets the tag
  virtual property String ^ Tag
  { String ^ get(); }

  //? Gets the ID for this element
  virtual property int Id
  { int get(); }

  // Gets the group id
  virtual property unsigned short GroupId
  { unsigned short get(); }

  // Gets the element id
  virtual property unsigned short ElementId
  {  unsigned short get(); }


  /// Gets the Value as a string
  virtual property String ^ Value
  { 
    String ^ get(); 
    void set(String ^ msValue);
  } // Value

 
  /// Gets/Sets the name
  virtual property String ^ Name
  { String ^ get(); } // Name

  /// Gets/Sets the VR
  virtual property String ^ VR
  { String ^ get(); }
  

  /// Gets/Sets the Length
  virtual property unsigned long Length
  {  unsigned long get(); } // Length

  /// Gets/Sets the VM
  virtual property unsigned short VM
  { unsigned short get(); } // VM

  /// Gets/Sets the private creator for this element
  virtual property String ^ PrivateCreator
  {  String ^ get(); }
  

  /// Return true if the element is a sequence
  virtual property bool IsSequence
  { bool get(); }

    /// Return true if the element is a private
  virtual property bool IsPrivate
  { bool get(); }

 /// Return true if the element is a sub-element
  virtual property bool IsSubElement
  { bool get(); }

  /// Returns true if there are sub-elements
  virtual property bool HasSubElements
  { bool get(); }

  /// Expanded flag
  virtual property bool Expanded
  { 
    bool get();
    void set(bool bValue);
  }

    /// Level indicator
  virtual property unsigned short Level
  { unsigned short get(); }


  property System::Windows::Media::Brush ^ ForeColor
  { 
    System::Windows::Media::Brush ^ get() { return m_foreColor; }
    void set(System::Windows::Media::Brush ^ color) 
    { 
      m_foreColor = color; 
      RaisePropertyChanged("ForeColor"); 
    }

  }

#pragma endregion

#pragma region fields

protected:

  scoped_ptr<std::shared_ptr<dcm::DcmElement>> m_pElement;

  System::Windows::Media::Brush ^ m_foreColor;

#pragma endregion

};  // class Element

CLOSE_DICOM_NS
