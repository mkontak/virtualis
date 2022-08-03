#pragma once

/// Using
using namespace System;
using namespace System::Xml;
using namespace System::Collections;
using namespace GalaSoft::MvvmLight;
using namespace GalaSoft::MvvmLight::Helpers;
using namespace GalaSoft::MvvmLight::Command;
using namespace GalaSoft::MvvmLight::Messaging;
using namespace GalaSoft::MvvmLight::Threading;

// foreward declarations
class dcm::DcmGroup;

// namespaces
OPEN_DICOM_NS


/**
 * Wrapper DcmGroup
 */
 public ref class Group : public IComparable, public ViewModelBase
{
// construction
public:

  /// Default constructor.
  Group(unsigned short iId, String ^ msDescription);

  /// Destructor.
  ~Group();

  /// Finalizer.
  !Group();

#pragma region methods

public:
  
  /// Compares the objects
  virtual int CompareTo(Object ^ mpOther) 
  { return ( Equals(mpOther) ? 0 : ( ((Group ^)mpOther)->Id < Id ? -1 : 1 ) ); } 

  /// Override the get hash code used for lists
  virtual int GetHashCode() override
  { return Id; }

  /// Returns true if the elements are the same
  virtual bool Equals(Object ^ mpObject) override
  { return Equals(dynamic_cast<Group ^>(mpObject)); }

  /// Returns true if the elements are the same
  bool Equals(Group ^ mpGroup)
  { return ( mpGroup != nullptr && mpGroup->Id == this->Id); }

#pragma endregion

// properties
public:

  property String ^ FormattedId
  { String ^ get(); }

  /// Gets or sets the row x.
  property unsigned short Id
  { unsigned short get(); } 


  /// Gets or sets the row y.
  property String ^ Description
  { String ^  get(); } 


// fields
private:

  // The low level object.
  dcm::DcmGroup * m_pGroup;

}; // class Group


CLOSE_DICOM_NS
