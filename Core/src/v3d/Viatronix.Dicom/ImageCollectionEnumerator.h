#pragma once

/// Includes
#include "Enums.h"
#include "ImageCollection.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;


OPEN_DICOM_NS

ref class IImage;

/**
 * Implements an ElementCollectionEnumerator
 */
 public ref class ImageCollectionEnumerator : public  Generic::IEnumerator<IImage ^>
{

public:

  ImageCollectionEnumerator(ImageCollection ^ mpCollection);


  ~ImageCollectionEnumerator() 
  { this->!ImageCollectionEnumerator(); }

  !ImageCollectionEnumerator()
  { m_mpCollection = nullptr; }
 

  virtual bool MoveNext();

  virtual void Reset();



public:


  virtual property IImage ^ Current
  { IImage ^ get(); }

 
private:

  property Object ^ CurrentObject
  {
    virtual Object^ get()  sealed = IEnumerator::Current::get
    {   return (Object ^)this->Current;  }
  };


private:

  ImageCollection ^ m_mpCollection;

};


CLOSE_DICOM_NS