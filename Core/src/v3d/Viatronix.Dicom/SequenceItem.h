#pragma once

/// Includes
#include "scoped_ptr.h"
#include "ElementCollection.h"


// Using
using namespace System;
using namespace System::Xml;
using namespace System::Collections;

OPEN_DICOM_NS




/**
 * Implements a DICOM sequence element
 */
 public ref class SequenceItem : public ElementCollection
{

#pragma region constructors/destructors

protected:

  SequenceItem();

public:
  /// Constructor
  SequenceItem(unsigned int iCard);
  
  /// Destructor
  ~SequenceItem();

  /// Finalizer
  !SequenceItem();

#pragma endregion


#pragma region methods

public:

  /// Gets the internal unmanaged std::shared_ptr<dcm::DcmSequenceItem> *
  System::IntPtr GetDcmSequenceItemPtr();

#pragma region create methods

public:

  /// Creates the sequence item
  static SequenceItem ^ Create(unsigned int iCard);

  /// Creates the sequence item
  static SequenceItem ^ Create(System::IntPtr dcmSequenceItem);

#pragma endregion

#pragma endregion

#pragma region propertied

public:

  /// Gets the item card
  property unsigned int Card
  { unsigned int get() { return m_iCard; } }

 

#pragma endregion

#pragma region fields

private:

  /// Item card number
  unsigned int m_iCard;

#pragma endregion

};  // class SequenceItem


CLOSE_DICOM_NS