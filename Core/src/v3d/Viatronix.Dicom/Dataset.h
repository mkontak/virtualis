#pragma once

/// includes
#include "Enums.h"
#include "scoped_ptr.h"
#include "IElement.h"
#include "Buffer.h"


/// Usings
using namespace System;
using namespace System::Xml;
using namespace System::Collections;


OPEN_DICOM_NS


// Forward declarations
ref class Buffer;
ref class Element;
ref class ElementCollection;


/** 
 * Implements a DICOM dataset which consists of an list of elements
 */
public ref class Dataset 
{

#pragma region constructor/destructors

public:
  /// constructor
  Dataset();

#pragma endregion

#pragma region constructors/destructors

public:

  /// Destructor
  ~Dataset();

  /// Finalizer
  !Dataset();

  /// Clones the dataset
  Dataset ^ Clone();

#pragma endregion

#pragma region Create Methods

public:

  /// Creates the element list from the unmanaged DcmDataset pointer
  static Dataset ^ Create(IntPtr dcmImagePtr);

  /// Creates the element list from the file specified
  static Dataset ^ Create(String ^ msParameter, ParameterType type);

  /// Creates the element list from the file specified
  static Dataset ^ Create(String ^ msFilename, String ^ msHeader);

  /// Creates the element list from the xml node 
  static Dataset ^ Create(XmlNode ^ mpNode);
 
  /// Creates the dataset using the seeding header and the bitmap
  static Dataset ^ Create(String ^ msHeader, System::Drawing::Bitmap ^ mpBitmap);

#pragma endregion

#pragma region methods

public:

  ///
  String ^ ToXml();

  /// Get the underlying image.
  System::IntPtr GetElementPtr();

  /// Gets the serialized header.
  String ^ GetSerializedHeader();

  // Gets the pixel value
  int GetPixelValue(unsigned int iRow, unsigned int iCol);

  // Gets the HU value
  int GetHUValue(unsigned int iRow, unsigned int iCol);

  /// Serializes the element list out as xml ( only plac holders for the binary will be there )
  void Serialize(String ^ msFile);

  //// Save the dataset to the specified file
  void Save(String ^ sFilePath);

  /// Sets the pixel data
  void SetPixelData(System::Drawing::Bitmap ^ mpBitmap);

  /// Gets the element list
  ElementCollection ^ GetElementCollection();

  /// Modifies the dicom header using the xml
  void Modify(String ^ sXml);

  /// Returns true if the dataset contains the element
  bool Contains(unsigned short iGroup, unsigned short iElement);

  /// Clears the list
  void Clear();

  void Remove(unsigned short iGroup, unsigned short iElement);

  /// Inserts the element into the system
  //void Insert(IElement ^ mpElement);

  /// Gets the Min/Max Pixel Values
  Generic::List<short> ^ GetMinMaxPixelValues()
  {

      std::pair<short, short> minmax(SHRT_MAX,SHRT_MIN);

      (*m_pDcmImage)->GetMinMaxPixelValues<short>(minmax);

      Generic::List<short> ^ mpMinMax =  gcnew Generic::List<short>();

      mpMinMax->Add(minmax.first);
      mpMinMax->Add(minmax.second);

      return mpMinMax;

  }



private:

  /// Gets the date time structure contain the dicom date and time elements combined
  DateTime ToDateTime(String ^ msDate, String ^ msTime);

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

      (*m_pDcmImage)->SetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue);

    }
    else if ( T::typeid == int::typeid )
    {
      int iValue(System::Convert::ToInt32(value));

      (*m_pDcmImage)->SetValue<int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt32::typeid )
    {
      unsigned int iValue(System::Convert::ToUInt32(value));

      (*m_pDcmImage)->SetValue<unsigned int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == short::typeid )
    {
      short iValue(System::Convert::ToInt16(value));

      (*m_pDcmImage)->SetValue<short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt16::typeid )
    {
      unsigned short iValue(System::Convert::ToUInt16(value));

      (*m_pDcmImage)->SetValue<unsigned short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == float::typeid )
    {
      float fValue(System::Convert::ToSingle(value));

      (*m_pDcmImage)->SetValue<float>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
    }
    else if ( T::typeid == double::typeid )
    {
      double fValue(System::Convert::ToDouble(value));

      (*m_pDcmImage)->SetValue<double>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
    }
    else if ( T::typeid == Int64::typeid )
    {
      __int64 iValue(System::Convert::ToInt64(value));

      (*m_pDcmImage)->SetValue<__int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
    }
    else if ( T::typeid == UInt64::typeid )
    {
       unsigned __int64 iValue(System::Convert::ToUInt64(value));

      (*m_pDcmImage)->SetValue<unsigned __int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);

    }

  }


    /// Sets a generic value
  generic<typename T> 
  void SetValue(unsigned short iGroup, unsigned short iElement, String ^ value)
  {
    std::string sValue(ss(System::Convert::ToString(value)));

    (*m_pDcmImage)->SetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue);

  }


#pragma endregion


#pragma region TryGetValue template methods

public:


  /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value);

  /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value);


  /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value);


  /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value);

   /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, float %  value);

  /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value);

   /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value);

   /// Sets a generic value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64 %  value);


  /// Sets a string value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^% mpBuffer);


  /// Sets a string value
  bool TryGetValue(unsigned short iGroup, unsigned short iElement, String ^% msValue);


#pragma endregion



#pragma region properties

public:

  // Gets the encapsulated pdf flag
  property bool IsEncapsulatedPdf
  {
    bool get();
  }

  // Gets the pixel value range
  property int PixelValueRange
  {
    int get();
  } // PixelValueRange

  /// Gets/Sets the query Retrieval level ( PATIENT/STUDY/SERIES )
  property String ^ QueryRetrievalLevel
  {
    String ^ get();
    void set(String ^ msValue);
  } // QueryRetrievalLevel


  /// Gets the manufacturer
  property String ^ Manufacturer
  { String ^ get(); }

  /// Gets the manufacturer model number
  property String ^ ManufacturerModel
  { String ^ get(); }

  property String ^ Institution
  { String ^ get(); }

  /// Gets the acquisition Number
  property __int64 AcquisitionNumber
  { 
    __int64 get(); 
    void set(__int64 iValue); 
  }
 
  /// Gets the patient name
  property String ^ PatientName
  { 
    String ^ get(); 
    void set(String ^ msValue);
  }

  /// Gets the patient dob
  property String ^ PatientDob
  { String ^ get(); }

   /// Gets the acquisition date
  property String ^ AcquisitionDate
  { String ^ get(); }

  /// Gets the series date
  property String ^ SeriesDate
  { String ^ get(); }

  /// Gets the study date
  property String ^ StudyDate
  { String ^ get(); }

  /// Gets the study description
  property String ^ StudyDescription
  { String ^ get(); }

  /// Gets the series description
  property String ^ SeriesDescription
  { String ^ get(); }

    /// Gets the study instance uid
  property String ^ StudyInstanceUid
  { 
    String ^ get();
    void set(String ^ msValue);
  }

  /// Gets the series instance Uid
  property String ^ SeriesInstanceUid
  { 
    String ^ get();
    void set(String ^ msValue);
  }

   /// Gets the study id
  property String ^ StudyId
  { String ^ get(); }

  /// Gets the series number
  property String ^ SeriesNumber
  { 
    String ^ get();
    void set(String ^ msValue);
  }

 
  /// Gets the instance uid
  property String ^ InstanceUid
  { 
    String ^ get(); 
    void set(String ^ msValue);
  }

  /// Gets the Instance Number
  property unsigned int InstanceNumber
  { 
    unsigned int get(); 
    void set (unsigned int iValue);
  }

  /// Gets the SOP class uid
  property String ^ AffectedSopClassUid
  { 
    String ^ get(); 
  }

   /// Gets the Number of Frames
  property unsigned int NumberOfFrames
  { unsigned int get(); }

  /// Gets the rows of the image
  property unsigned int Rows
  { unsigned int get(); }
  
  /// Gets the Columns of the image
  property unsigned int Columns
  { unsigned int get(); }

   /// Gets the Bits Allocated per pixel
  property unsigned short BitsAllocated
  { unsigned short get(); }
 
  /// Gets the Rescale Slope
  property float RescaleSlope
  { float get(); }

  /// Gets the Rescale Intercept
  property float RescaleIntercept
  { float get(); }

  /// Gets the Samples per pixel of the image
  property unsigned int SamplesPerPixel
  { unsigned int get(); }
 
  /// Gets the PixelPaddingValue
  property unsigned int PixelPaddingValue
  { unsigned int get(); }

  /// Gets the WindowCenter
  property double WindowCenter
  { double get(); }

  /// Gets the WindowWidth
  property double WindowWidth
  {  double get(); }

  /// Gets the image position 
  property System::Collections::Generic::List<double> ^ ImagePosition
  { System::Collections::Generic::List<double> ^ get(); }

  /// Gets the pixel spacing and returns the x/y in a list
  property System::Collections::Generic::List<double> ^ PixelSpacing
  { System::Collections::Generic::List<double> ^ get(); }
 
  /// Gets the image orientation x/y/z/x/y/z in a list
  property System::Collections::Generic::List<double> ^ ImageOrientation
  { System::Collections::Generic::List<double> ^ get(); }

  /// Gets the plane to image corresponds to
  property ImagePlanes ImagePlane
  { ImagePlanes get(); }

  // Gets the ImageDateTime 
  property DateTime ImageDateTime
  { DateTime get(); }

  // Gets the AcquisitionDateTime 
  property DateTime AcquisitionDateTime
  { DateTime get(); }

  // Gets the Series date and time 
  property DateTime SeriesDateTime
  { DateTime get(); }

  // Gets the study date and time 
  property DateTime StudyDateTime
  { DateTime get(); }

  /// Gets the patient sex
  property String ^ PatientSex
  { String ^ get(); }

  /// Gets the patient weight
  property unsigned int PatientWeight
  { unsigned int get(); }

  /// Gets the patient age with units
  property String ^ PatientAge
  {  String ^ get(); }

  /// Gets the patient birth date and time from the element list
  property DateTime PatientBirthDateTime
  {  DateTime get(); }

  /// Gets the additional patient history
  property String ^ AdditionalPatientHistory
  { String ^ get(); }

  /// Gets the operator name
  property String ^ OperatorName
  { String ^ get(); }

  /// Gets the referring physician
  property String ^ ReferringPhysician
  { String ^ get(); }

  /// Gets the reconstruction diameter
  property float ReconstructionDiameter
  { float get(); }

  /// Gets the kvp
  property float Kvp
  { float get(); }

  /// Gets the xray tube current
  property float XrayTubeCurrent
  { float get(); }

 
  /// Gets the patient position
  property String ^ PatientPosition
  { String ^ get(); }

  /// Gets the institution name
  property String ^ InstitutionName
  { String ^ get(); }

  /// Gets the accession number
  property String ^ AccessionNumber
  { String ^ get(); }

  /// Gets the medical id (Same as patient id)
  property String ^ MedicalId
  { String ^ get() { return this->PatientId; } }

  /// Gets the patient id 
  property String ^ PatientId
  { String ^ get(); }

  /// Sets/Gets the modality
  property String ^ Modality
  { 
    String ^ get(); 
    void set(String ^ msValue);
  }

  /// Gets the pixel data
  property Viatronix::Dicom::Buffer ^ PixelData
  { Viatronix::Dicom::Buffer ^ get(); }

  // Gets/Sets the pixel representation
  property unsigned short PixelRepresentation
  { 
    unsigned short get(); 
    void set(unsigned short iPixelRepresentation); 
  }

#pragma endregion

#pragma region fields

private:

  /// Separator used in dicom
  array<Char> ^ m_pSeperators;

  /// DcmImage
  scoped_ptr<std::shared_ptr<dcm::DcmImage>> m_pDcmImage;

  Generic::List<int> ^ m_mpMinMax;

 
#pragma endregion



};  // class Dataset


CLOSE_DICOM_NS