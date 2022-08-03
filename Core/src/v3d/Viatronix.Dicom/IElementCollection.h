
#pragma once

#include "IElement.h"



using namespace System;
using namespace System::Xml;
using namespace System::Collections;

OPEN_DICOM_NS

ref class Element;

public interface class  IElementCollection : public Generic::IEnumerable<Element ^>, public Generic::ICollection<Element ^>, public Generic::IEnumerator<Element ^>
{

public:

  /// Try and get the specified element returning true if successful, false if not found
  bool TryGetElement(unsigned short iGroup, unsigned short iElement, Element ^% mpElement);

  void Reset();

  bool MoveNext();

  property bool IsReadOnly
  { bool get(); void set(bool); }

  property Element ^ Current
  { Element ^ get(); }

   Generic::IEnumerator<Element ^> ^ GetEnumerator();

  virtual property unsigned short Level
  {
    unsigned short get();
  }

}; // interface IElementCollection

CLOSE_DICOM_NS