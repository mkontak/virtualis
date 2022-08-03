#pragma once

/// includes
#include "Enums.h"
#include "scoped_ptr.h"
#include "IElement.h"
#include "Buffer.h"
#include "ImageBase.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;
using namespace GalaSoft::MvvmLight;


OPEN_DICOM_NS


// Forward declarations
ref class Buffer;
ref class Element;
ref class ElementCollection;
ref class ImageCollection;


/** 
 * Implements a DICOM dataset which consists of an list of elements
 */
public ref class Image : public ImageBase 
{

#pragma region constructor/destructors

public:
  /// constructor
  Image();

#pragma endregion

#pragma region constructors/destructors

public:

  /// Destructor
  ~Image();

  /// Finalizer
  !Image();

  /// CLones the image
  virtual IImage ^ Clone() override;

#pragma endregion

#pragma region methods

public:


  //// Save the dataset to the specified file
  void Save(String ^ sFilePath);

  /// Sets the pixel data
  void SetPixelData(System::Drawing::Bitmap ^ mpBitmap);

  /// Clears the list
  void Clear();

  /// Adds or Updates the element specified
  void Insert(IElement ^ mpElement);


#pragma endregion




#pragma region SetValue template methods

public:

  /// Sets a generic value
  generic<typename T> 
  void SetValue(unsigned short iGroup, unsigned short iElement, T value)
  {
    if ( T::typeid == String::typeid )
    {
      std::string sValue(ss(System::Convert::ToString(value)));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue);

    }
    else if ( T::typeid == int::typeid )
    {
      int iValue(System::Convert::ToInt32(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt32::typeid )
    {
      unsigned int iValue(System::Convert::ToUInt32(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<unsigned int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == short::typeid )
    {
      short iValue(System::Convert::ToInt16(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt16::typeid )
    {
      unsigned short iValue(System::Convert::ToUInt16(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<unsigned short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == float::typeid )
    {
      float fValue(System::Convert::ToSingle(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<float>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
    }
    else if ( T::typeid == double::typeid )
    {
      double fValue(System::Convert::ToDouble(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<double>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
    }
    else if ( T::typeid == Int64::typeid )
    {
      __int64 iValue(System::Convert::ToInt64(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<__int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt64::typeid )
    {
       unsigned __int64 iValue(System::Convert::ToUInt64(value));

      std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<unsigned __int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);

    }

  }


    /// Sets a generic value
  generic<typename T> 
  void SetValue(unsigned short iGroup, unsigned short iElement, String ^ value)
  {
    std::string sValue(ss(System::Convert::ToString(value)));

    std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue);

  }


#pragma endregion



#pragma region properties

public:

  /// Gets/Sets the query Retrieval level ( PATIENT/STUDY/SERIES )
  virtual property String ^ QueryRetrievalLevel
  {
    String ^ get();
    void set(String ^ msValue);
  } // QueryRetrievalLevel

  /// Gets the acquisition Number
  property __int64 AcquisitionNumber
  { 
    void set(__int64 iValue); 
    __int64 get() new { return ImageBase::AcquisitionNumber; }
  }
 
  /// Gets the patient name
  property String ^ PatientName
  { 
    void set(String ^ msValue);
    String ^ get() new { return ImageBase::PatientName; }  

  }

  property String ^ StudyInstanceUid
  { 
    void set(String ^ msValue);
    String ^ get()  new { return ImageBase::StudyInstanceUid; }  
  }

  /// Gets the series instance Uid
  property String ^ SeriesInstanceUid
  { 
    void set(String ^ msValue);
    String ^ get() new  { return ImageBase::SeriesInstanceUid; }  

  }


  /// Gets the series number
  property String ^ SeriesNumber
  { 
    void set(String ^ msValue);
    String ^ get() new { return ImageBase::SeriesNumber; }  

  }

 
  /// Gets the instance uid
  property String ^ InstanceUid
  { 
    void set(String ^ msValue);
    String ^ get() new { return ImageBase::InstanceUid; }  

  }


  /// Gets the Instance Number
  property unsigned int InstanceNumber
  { 
    void set (unsigned int iValue);
    unsigned int get() new { return ImageBase::InstanceNumber; }  
  }

  /// Sets/Gets the modality
  property String ^ Modality
  { 
    void set(String ^ msValue);
    String ^ get() new { return ImageBase::Modality; }  

  }

 
#pragma endregion



};  // class Image


CLOSE_DICOM_NS