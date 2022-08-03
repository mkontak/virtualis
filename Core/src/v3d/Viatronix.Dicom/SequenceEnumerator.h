#pragma once

/// Includes
#include "Enums.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;


OPEN_DICOM_NS


ref class Sequence;
ref class ElementCollection;

/**
 * Implements an ElementCollectionEnumerator
 */
public ref class SequenceEnumerator : public  Generic::IEnumerator<ElementCollection ^>
{

public:

  SequenceEnumerator(Sequence ^ mpSequnece);

  ~SequenceEnumerator() 
  { this->!SequenceEnumerator(); }

  !SequenceEnumerator() 
  { m_mpSequence = nullptr; }

  virtual bool MoveNext();

  virtual void Reset();

public:

  virtual property ElementCollection ^ Current
  { ElementCollection ^ get(); }

private:

  property Object ^ CurrentObject
  { 
    virtual Object ^ get() sealed = IEnumerator::Current::get
    { return this->Current; }
  }


private:

  Sequence ^ m_mpSequence;

};


CLOSE_DICOM_NS