#pragma once

/// Includes
#include "Enums.h"
#include "IElementCollection.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;


OPEN_DICOM_NS

ref class Element;

/**
 * Implements an ElementCollectionEnumerator
 */
 public ref class ElementCollectionEnumerator : public  Generic::IEnumerator<Element ^>
{

public:

  ElementCollectionEnumerator(IElementCollection ^ mpCollection);


  ~ElementCollectionEnumerator() 
  { this->!ElementCollectionEnumerator(); }

  !ElementCollectionEnumerator()
  { m_mpCollection = nullptr; }
 

  virtual bool MoveNext();

  virtual void Reset();



public:


  virtual property Element ^ Current
  { Element ^ get(); }

 
private:

  property Object ^ CurrentObject
  {
    virtual Object^ get()  sealed = IEnumerator::Current::get
    {   return (Object ^)this->Current;  }
  };


private:

  IElementCollection ^ m_mpCollection;

};


CLOSE_DICOM_NS