#pragma once

// includes
#include "scoped_ptr.h"
#include "IImage.h"

using namespace System;
using namespace System::Collections;

// forward declarations.
class dcm::DcmImageCollection;

// namespaces
OPEN_DICOM_NS

// foreward declarations.
ref class Cosines;
ref class Image;

/**
 * Wraps the underlying dcm::DcmImageCollection.
 */
public ref class ImageCollection : public Generic::IEnumerable<IImage ^>, public Generic::ICollection<IImage ^>, public Generic::IEnumerator<IImage ^>
{
// construction
protected:

  /// Constructor.
  ImageCollection();


public:

  /// Destructor.
  ~ImageCollection();

  /// Finalizer.
  !ImageCollection();


#pragma region properties

// properties
public:

  /// Determines if the collection is read only
  virtual property bool IsReadOnly
  { 
    bool get() { return m_bIsReadOnly; } 
    void set(bool bValue) { m_bIsReadOnly = bValue; } 
  }

  /// Syncronized flag
  virtual property bool IsSynchronized
  { bool get(); }

  /// Syncronized flag
  virtual property Object ^ SyncRoot
  { Object ^ get(); }

  /// Current element
  virtual property IImage ^ Current
  { IImage ^ get(); }

 
  /// Get the count of elements
  virtual property int Count
  { int get(); }

  
  /// Gets the slice spacing.
  property double SliceSpacing
  {
    double get();
  } // SliceSpacing

  /// Gets the dimensions.
  property array<double> ^ Dimensions
  {
    array<double> ^ get();
  } // Dimensions

  /// Gets the pixel spacing.
  property array<double> ^ PixelSpacing
  {
    array<double> ^ get();
  } // PixelSpacing

  /// Gets the reference point from the first image in the collection.
  property array<float> ^ ReferencePoint
  {
    array<float> ^ get();
  } // ReferencePoint

  /// Gets the cosines for the series.
  property Viatronix::Dicom::Cosines ^ ImageOrientation
  {
    Viatronix::Dicom::Cosines ^ get();
  } // ImageOrientation

private:

  property Object ^ CurrentObject
  {
    virtual Object^ get()  sealed = IEnumerator::Current::get
    {   return (Object ^)this->Current;  }
  };


#pragma endregion

#pragma region methods

#pragma region create

public:

   /// Create the element
  static ImageCollection ^ Create(System::IntPtr dcmImageCollectionPtr);


  /// Create the element
  static ImageCollection ^ Create();

#pragma endregion

#pragma region enumerator implementations

public:

    // Adds the specified element
  virtual void Add(IImage ^ mpImage);

  // Removes the specified element
  virtual bool Remove(IImage ^ mpImage);

  // Inserts the specified element
  bool Contains(String ^ msUid);

  // Inserts the specified element
  virtual bool Contains(IImage ^ mpImage);

  /// Move to next item
  virtual bool MoveNext();

  /// Resets iterator
  virtual void Reset();

  /// Copy to array
  virtual void CopyTo(array<IImage ^> ^ mpArray, int index);

  /// Clears the image collection
  virtual void Clear();
  

  //// Sort
  ImageCollection ^ Sort();

  
  /// Get the enumerator
  virtual  Generic::IEnumerator<IImage ^> ^ GetEnumerator();

private:

  /// Get Enumerator 
  virtual IEnumerator ^ GetEnumeratorBase() sealed = IEnumerable::GetEnumerator
  { return GetEnumerator(); }

#pragma endregion

// methods
public:

  /// Adds the dicom file to the list.
  void Add(System::String ^ mpFilePath);

  /// Adds the files within the specified folder.
  void AddDirectory(System::String ^ mpFolderPath);

  /// Adds the files within the specified folder.
  void AddDirectory(System::String ^ mpFolderPath, System::String ^ mpPattern);


#pragma endregion

// fields
private:

  /// Read only
  bool m_bIsReadOnly;

  /// The unmanaged image collection.
  scoped_ptr<std::shared_ptr<dcm::DcmImageCollection>> m_pImageCollection;

}; // class ImageCollection

CLOSE_DICOM_NS
