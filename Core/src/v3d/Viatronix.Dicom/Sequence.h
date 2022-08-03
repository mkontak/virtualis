#pragma once

/// Includes
#include "scoped_ptr.h"
#include "Element.h"
#include "ElementCollection.h"


// Using
using namespace System;
using namespace System::Xml;
using namespace System::Collections;

OPEN_DICOM_NS


/**
 * Implements a DICOM sequence element
 */
public ref class Sequence : public Element,  public  Generic::ICollection<ElementCollection ^>, public  Generic::IEnumerable<ElementCollection ^>, public Generic::IEnumerator<ElementCollection ^>
{

#pragma region constructors/destructors

protected:
  /// Constructor
  Sequence();

public:

  /// Constructor (Tag)
  Sequence(unsigned short iGroup, unsigned short iElement);

    /// Destructor
  ~Sequence();

  /// Finalizer
  !Sequence();

#pragma endregion

#pragma region methods

public:
 
  /// Adds sequence item to this sequence
  virtual void Add(ElementCollection ^ mpCollection);

  /// Removes the sequence item
  virtual bool Remove(ElementCollection ^ mpCollection);

  /// Determines if the sequence item is contained in the collection
  virtual bool Contains(ElementCollection ^ mpCollection);


   /// Resets the enumeration
  virtual void Reset();

    /// Move to next item
  virtual bool MoveNext();

  /// Copy to array
  virtual void CopyTo(array<ElementCollection ^> ^ mpArray, int index);

  /// Clears the collection
  virtual void Clear();

  /// Get the enumerator
  virtual  Generic::IEnumerator<ElementCollection ^> ^ GetEnumerator();

private:

  /// Get Enumerator 
  virtual IEnumerator ^ GetEnumeratorBase() sealed = IEnumerable::GetEnumerator
  { return GetEnumerator(); }

#pragma region create methods
  
public:

  /// Create the sequence
  static Sequence ^ Create(unsigned short iGroup, unsigned short iElement);

  /// Create the sequence
  static Sequence ^ Create(System::IntPtr dcmSequencePtr);

#pragma endregion


  
#pragma region properties

public:

  virtual property bool IsReadOnly
  { bool get() { return m_bIsReadOnly; } }

  /// Syncronized flag
  virtual property bool IsSynchronized
  { bool get(); }

  /// Syncronized flag
  virtual property Object ^ SyncRoot
  { Object ^ get(); }


  /// Get the count of elements
  virtual property int Count
  { int get(); }

  /// Current element
  virtual property ElementCollection ^ Current
  { ElementCollection ^ get(); }

private:

  property Object ^ CurrentBase
  { 
    virtual Object ^ get() sealed = IEnumerator::Current::get
    { return (Object ^)Current; }
  }


#pragma endregion




#pragma region fields

private:

  /// List of items
  //scoped_ptr<std::shared_ptr<dcm::DcmSequence>> m_pSequence;

  /// Read only flag
  bool m_bIsReadOnly;
  
#pragma endregion

};  // class Sequence


CLOSE_DICOM_NS