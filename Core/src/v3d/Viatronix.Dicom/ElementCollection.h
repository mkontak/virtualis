#pragma once

/// Includes
#include "scoped_ptr.h"
#include "IElementCollection.h"
#include "Element.h"
#include "Enums.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;
using namespace GalaSoft::MvvmLight;
using namespace GalaSoft::MvvmLight::Helpers;
using namespace GalaSoft::MvvmLight::Command;
using namespace GalaSoft::MvvmLight::Messaging;
using namespace GalaSoft::MvvmLight::Threading;

OPEN_DICOM_NS

// Forward declaration
ref class Group;

/** 
 * Wrapper for the unmanaged DcmElementCollection object
 */
public ref class ElementCollection : public IElementCollection
{

#pragma region constructor/destructors

protected:

  /// Constructor
  ElementCollection();  


public:

  /// Destructor
  ~ElementCollection();

  /// Finalizer
  !ElementCollection();

#pragma endregion

#pragma region methods

#pragma region Create Methods

public:

  /// Creates an empty element list
  static ElementCollection ^ Create();


  /// Creates the element list from the string
  static ElementCollection ^ Create(String ^ msParameer, ParameterType type);

  /// Creates the element list from the string
  static ElementCollection ^ Create(XmlNode ^ mpNode);


  /// Creates the element list from the string
  static ElementCollection ^ Create(System::IntPtr dcmElementCollectionPtr);

#pragma endregion

#pragma region methods


public:

  /// Writes the element list as XML to the files specified
  virtual void WriteXml(String ^ msFilePath);

  /// Try and get the specified element returning true if successful, false if not found
  virtual bool TryGetElement(unsigned short iGroup, unsigned short iElement, Element ^% mpElement);
  
  /// Gets the groups this list contains
  Generic::List<Group ^> ^ GetGroups();

  /// Gets the unmanaged object
  System::IntPtr GetDcmElementCollectionPtr();

  /// Gets the element value
  String ^ GetElementValue(unsigned short iGroup, unsigned short iElement, String ^ msDefault)
  { Element ^ mpElement(nullptr); return ( TryGetElement(iGroup,iElement,mpElement) ? mpElement->Value : msDefault ); }

#pragma endregion

#pragma region enumeration implementation

public:

  // Adds the specified element
  virtual void Add(Element ^ mpElement);

  // Removes the specified element
  virtual bool Remove(Element ^ mpElement);

  // Inserts the specified element
  bool Contains(unsigned short groupId, unsigned short elementId);

  // Inserts the specified element
  virtual bool Contains(Element ^ mpElement);

  /// Move to next item
  virtual bool MoveNext();

  /// Resets iterator
  virtual void Reset();

  /// Copy to array
  virtual void CopyTo(array<Element ^> ^ mpArray, int index);

  /// Clears the collection
  virtual void Clear();


  /// Get the enumerator
  virtual  Generic::IEnumerator<Element ^> ^ GetEnumerator();

private:

  /// Get Enumerator 
  virtual IEnumerator ^ GetEnumeratorBase() sealed = IEnumerable::GetEnumerator
  { return GetEnumerator(); }
 
#pragma endregion

#pragma endregion


#pragma region properties

public:

  /// Gets the patient Name.
  property String ^ PatientName
  {
    String ^ get() { return GetElementValue(0x0010, 0x0010, String::Empty); }
  } // PatientName


  /// Gets the patient's Id.
  property String ^ PatientId
  {
    String ^ get() { return GetElementValue(0x0010, 0x0020, String::Empty); }
  } // PatientId


  /// Gets the patient weight.
  property float PatientWeight
  {
    float get() { return Convert::ToSingle(GetElementValue(0x0010, 0x1030, "0.0")); }
  } // PatientWeight


  /// Gets the patient's Date of Birth.
  property String ^ PatientDob
  {
    String ^ get() { return GetElementValue(0x0010, 0x0030, "190101"); }
  } // PatientDob

  
  /// Gets the patient's Age.
  property String ^ PatientAge
  {
    String ^ get() { return GetElementValue(0x0010, 0x1010, String::Empty); }
  } // PatientAge


  /// Gets the patient's Gender.
  property String ^ PatientSex
  {
    String ^ get() { return GetElementValue(0x0010, 0x0040, "U"); }
  } // PatientSex


  /// Gets the patient's AdditionalHistory.
  property String ^ AdditionalPatientHistory
  {
    String ^ get() { return GetElementValue(0x0010, 0x21b0, String::Empty); }
  } // AdditionalPatientHistory


  /// Gets the study's Instance Uid.
  property String ^ StudyInstanceUid
  {
    String ^ get() { return GetElementValue(0x0020, 0x000D, String::Empty); }
  } // StudyInstanceUid


  /// Gets the Institution Name.
  property String ^ InstitutionName
  {
    String ^ get() { return GetElementValue(0x0008, 0x0080, String::Empty); }
  } // InstitutionName


  /// Gets the study's Operator Name.
  property String ^ OperatorName
  {
    String ^ get() { return GetElementValue(0x0008, 0x1070, String::Empty); }
  } // OperatorName


  /// Gets the study's Referring Physician.
  property String ^ ReferringPhysician
  {
    String ^ get() { return GetElementValue(0x0008, 0x0090, String::Empty); }
  } // ReferringPhysician


  /// Gets the study's Id.
  property String ^ StudyId
  {
    String ^ get() { return GetElementValue(0x0020, 0x0010, String::Empty); }
  } // StudyId


  /// Gets the study's Accession Number.
  property String ^ AccessionNumber
  {
    String ^ get() { return GetElementValue(0x0008, 0x0050, String::Empty); }
  } // AccessionNumber


  /// Gets the study's Date.
  property String ^ StudyDate
  {
    String ^ get() { return GetElementValue(0x0008, 0x0020, "19010101"); }
  } // StudyDate


  /// Gets the study's Time.
  property String ^ StudyTime
  {
    String ^ get() { return GetElementValue(0x0008, 0x0030, "000000"); }
  } // StudyTime


  /// Gets the study's DateTime.
  property DateTime StudyDateTime
  {
    DateTime get();
  } // StudyDateTime


  /// Gets the study's Description.
  property String ^ StudyDescription
  {
    String ^ get() { return GetElementValue(0x008, 0x1030, String::Empty); }
  } // StudyDescription


  /// Gets the series' Instance Uid.
  property String ^ SeriesInstanceUid
  {
    String ^ get() { return GetElementValue(0x0020, 0x000E, String::Empty); }
  } // SeriesInstanceUid


  /// Gets the series' Date.
  property String ^ SeriesDate
  {
    String ^ get() { return GetElementValue(0x0008, 0x0021, "19010101"); }
  } // SeriesDate


  /// Gets the series' Date.
  property String ^ SeriesTime
  {
    String ^ get() { return GetElementValue(0x0008, 0x0031, "000000"); }
  } // SeriesDate


  /// Gets the series' DateTime.
  property DateTime SeriesDateTime
  {
    DateTime get();
  } // SeriesDateTime


  /// Gets the series' Modality.
  property String ^ Modality
  {
    String ^ get() { return GetElementValue(0x0008, 0x0060, String::Empty); }
  } // Modality


  /// Gets the series' Number.
  property String ^ SeriesNumber
  {
    String ^ get() { return GetElementValue(0x0020, 0x0011, String::Empty); }
  } // SeriesNumber


  /// Gets the series' Description.
  property String ^ SeriesDescription
  {
    String ^ get() { return GetElementValue(0x0008, 0x103e, String::Empty); }
  } // SeriesDescription


  /// Gets the serialize header
  virtual property String ^ SerializedHeader
  {
    String ^ get();
  } // SerializedHeader


  /// Determines if the collection is read only
  virtual property bool IsReadOnly
  { 
    bool get() { return m_bIsReadOnly; } 
    void set(bool bValue) { m_bIsReadOnly = bValue; } 
  } // IsReadOnly

  /// Synchronized flag
  virtual property bool IsSynchronized
  { bool get(); }

  /// Synchronized flag
  virtual property Object ^ SyncRoot
  { Object ^ get(); }

  /// Current element
  virtual property Element ^ Current
  { Element ^ get(); }

 
  /// Get the count of elements
  virtual property int Count
  { int get(); }

  
  /// Level indicator
  virtual property unsigned short Level
  {   unsigned short get();  }

private:

  property Object ^ CurrentObject
  { 
    virtual Object ^ get() sealed = IEnumerator::Current::get
    { return (Object ^)Current; }
  }


#pragma endregion

#pragma region fields

protected:

  /// Unmanaged element list
  scoped_ptr<std::shared_ptr<dcm::DcmElementCollection>> m_pElementCollection;

  /// Read only flag
  bool m_bIsReadOnly;

#pragma endregion

};  // class ElementCollection



CLOSE_DICOM_NS