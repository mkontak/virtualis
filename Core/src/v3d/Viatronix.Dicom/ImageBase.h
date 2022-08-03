#pragma once

/// includes
#include "Enums.h"
#include "scoped_ptr.h"
#include "IElement.h"
#include "Buffer.h"
#include "IImage.h"


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
public ref class ImageBase : public IImage
{

#pragma region constructor/destructors

protected:
  /// constructor
  ImageBase();

#pragma endregion

#pragma region constructors/destructors

public:

  /// Destructor
  ~ImageBase();

  /// Finalizer
  !ImageBase();

  /// Clones the image
  virtual IImage ^ Clone() override
  { throw gcnew NotImplementedException("Not implemented"); }

#pragma endregion



#pragma region methods

public:

  virtual bool Contains(unsigned short iGroup, unsigned short iElement) override;

  virtual ImageCollection ^ GetFrames() override;

  /// Gets the internal shared_ptr
  virtual System::IntPtr GetIDcmImagePtr() override;
  
  
  /// Sets the internal shared_ptr
  virtual void SetIDcmImagePtr(System::IntPtr dcmImagePtr) override
  { m_pDcmImage.reset((std::shared_ptr<dcm::IDcmImage> *)(dcmImagePtr.ToPointer())); }


  ///
  virtual String ^ ToXml() override;


  // Gets the pixel value
  virtual int GetPixelValue(unsigned int iRow, unsigned int iCol) override;

  // Gets the HU value
  virtual int GetHUValue(unsigned int iRow, unsigned int iCol) override;

  /// Serializes the element list out as xml ( only plac holders for the binary will be there )
  virtual void Serialize(String ^ msFile) override;


  /// Gets the element list
  virtual ElementCollection ^ GetElements() override;


  /// Gets the Min/Max Pixel Values
  virtual Generic::List<short> ^ GetMinMaxPixelValues() override;

  /// Gets the image size in bytes
  virtual int GetImageSize() override;


private:

  /// Gets the date time structure contain the dicom date and time elements combined
  DateTime ToDateTime(String ^ msDate, String ^ msTime);

#pragma endregion




#pragma region TryGetValue template methods

public:


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value, bool searchSubElements ) override;

  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value, bool searchSubElements ) override;


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value, bool searchSubElements ) override;


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value, bool searchSubElements ) override;

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, float %  value, bool searchSubElements ) override;

  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value, bool searchSubElements ) override;

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value, bool searchSubElements ) override;

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64 %  value, bool searchSubElements ) override;


  /// Sets a string value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, String ^% msValue, bool searchSubElements ) override;

    
  /// Sets a string value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^% mpBuffer, bool searchSubElements ) override;

#pragma endregion



#pragma region properties

public:

  /// Gets the file name
  virtual property String ^ Filename
  { String ^ get() override; }

  /// Gets the flag indicating if the image is a frame
  virtual property bool IsFrame
  {  bool get() override; }

  virtual property bool IsMonochrome
  {  bool get() override; }

  // Gets the encapsulated pdf flag
  virtual property bool IsEncapsulatedPdf
  {  bool get() override; }

  // Gets the pixel value range
  virtual property int PixelValueRange
  {   int get() override; } // PixelValueRange

  /// Gets the manufacturer
  virtual property String ^ Manufacturer
  { String ^ get() override; }

  /// Gets the manufacturer model number
  virtual property String ^ ManufacturerModel
  { String ^ get()  override; }


  virtual property String ^ Institution
  { String ^ get() override; }

  // Gets the time zone offset from utc
  virtual property String ^ TimeZoneOffsetFromUTC
  { String ^ get() override; }

  /// Gets the acquisition Number
  virtual property __int64 AcquisitionNumber
  { __int64 get() override; }
 
  /// Gets the patient name
  virtual property String ^ PatientName
  { String ^ get() override; }

  /// Gets the patient dob
  virtual property String ^ PatientDob
  { String ^ get() override; }

   /// Gets the acquisition date
  virtual property String ^ AcquisitionDate
  { String ^ get() override; }

  /// Gets the series date
  virtual property String ^ SeriesDate
  { String ^ get() override; }

  /// Gets the study date
  virtual property String ^ StudyDate
  { String ^ get() override; }

  /// Gets the study description
  virtual property String ^ StudyDescription
  { String ^ get() override; }

  /// Gets the series description
  virtual property String ^ SeriesDescription
  { String ^ get() override; }

    /// Gets the study instance uid
  virtual property String ^ StudyInstanceUid
  { String ^ get() override; }

  /// Gets the series instance Uid
  virtual property String ^ SeriesInstanceUid
  { String ^ get()  override; }

   /// Gets the study id
  virtual property String ^ StudyId
  { String ^ get() override; }

  /// Gets the series number
  virtual property String ^ SeriesNumber
  { String ^ get() override; }

 
  /// Gets the instance uid
  virtual property String ^ InstanceUid
  { String ^ get()  override; }
  

  /// Gets the Instance Number
  virtual property unsigned int InstanceNumber
  { unsigned int get()  override; }


  /// Gets the SOP class uid
  virtual property String ^ AffectedSopClassUid
  { String ^ get()  override; }

   /// Gets the Number of Frames
  virtual property unsigned int NumberOfFrames
  { unsigned int get()  override; }

  /// Gets the rows of the image
  virtual property unsigned int Rows
  { unsigned int get()  override; }
  
  /// Gets the Columns of the image
  virtual property unsigned int Columns
  { unsigned int get() override; }

   /// Gets the Bits Allocated per pixel
  virtual property unsigned short BitsAllocated
  { unsigned short get() override; }
 
  /// Gets the Rescale Slope
  virtual property float RescaleSlope
  { float get() override; }

  /// Gets the Rescale Intercept
  virtual property float RescaleIntercept
  { float get() override; }

  /// Gets the Samples per pixel of the image
  virtual property unsigned int SamplesPerPixel
  { unsigned int get() override; }
 
  /// Gets the PixelPaddingValue
  virtual property unsigned int PixelPaddingValue
  { unsigned int get() override; }

  /// Gets the WindowCenter
  virtual property double WindowCenter
  { double get() override; }

  /// Gets the WindowWidth
  virtual property double WindowWidth
  {  double get() override; }

  /// Gets the image position 
  virtual property System::Collections::Generic::List<double> ^ ImagePosition
  { System::Collections::Generic::List<double> ^ get() override; }

  /// Gets the pixel spacing and returns the x/y in a list
  virtual property System::Collections::Generic::List<double> ^ PixelSpacing
  { System::Collections::Generic::List<double> ^ get() override; }
 
  /// Gets the image orientation x/y/z/x/y/z in a list
  virtual property System::Collections::Generic::List<double> ^ ImageOrientation
  { System::Collections::Generic::List<double> ^ get() override; }

  /// Gets the plane to image corresponds to
  virtual property ImagePlanes ImagePlane
  { ImagePlanes get() override; }

  // Gets the ImageDateTime 
  virtual property DateTime ImageDateTime
  { DateTime get() override; }

  // Gets the AcquisitionDateTime 
  virtual property DateTime AcquisitionDateTime
  { DateTime get() override; }

  // Gets the Series date and time 
  virtual property DateTime SeriesDateTime
  { DateTime get() override; }

  // Gets the study date and time 
  virtual property DateTime StudyDateTime
  { DateTime get()  override; }

  /// Gets the patient sex
  virtual property String ^ PatientSex
  { String ^ get()  override; }

  /// Gets the patient weight
  virtual property float PatientWeight
  { float get()  override; }

  /// Gets the patient age with units
  virtual property String ^ PatientAge
  {  String ^ get()  override; }

  /// Gets the patient birth date and time from the element list
  virtual property DateTime PatientBirthDateTime
  {  DateTime get()  override; }

  /// Gets the additional patient history
  virtual property String ^ AdditionalPatientHistory
  { String ^ get()  override; }

  /// Gets the operator name
  virtual  property String ^ OperatorName
  { String ^ get()  override; }

  /// Gets the referring physician
  virtual property String ^ ReferringPhysician
  { String ^ get()  override; }

  /// Gets the reconstruction diameter
  virtual property float ReconstructionDiameter
  { float get()  override; }

  /// Gets the kvp
  virtual property float Kvp
  { float get()  override; }

  /// Gets the xray tube current
  virtual property float XrayTubeCurrent
  { float get()  override; }

 
  /// Gets the patient position
  virtual property String ^ PatientPosition
  { String ^ get()  override; }

  /// Gets the institution name
  virtual property String ^ InstitutionName
  { String ^ get()  override; }

  /// Gets the accession number
  virtual property String ^ AccessionNumber
  { String ^ get()  override; }

  /// Gets the medical id (Same as patient id)
  virtual property String ^ MedicalId
  { String ^ get() override  { return this->PatientId; } }

  /// Gets the patient id 
  virtual property String ^ PatientId
  { String ^ get()  override; }

  /// Sets/Gets the modality
  virtual property String ^ Modality
  { 
    String ^ get()  override; 
  }

  /// Gets the pixel data
  virtual property Viatronix::Dicom::Buffer ^ PixelData
  { Viatronix::Dicom::Buffer ^ get()  override; }

  // Gets/Sets the pixel representation
  virtual property unsigned short PixelRepresentation
  { 
    unsigned short get()  override; 
  }

#pragma endregion

#pragma region fields

protected:

  /// Frames
  ImageCollection ^ m_pFrames;

  /// DcmImage
  scoped_ptr<std::shared_ptr<dcm::IDcmImage>> m_pDcmImage;

private:

  /// Separator used in dicom
  array<Char> ^ m_pSeperators;


  Generic::List<int> ^ m_mpMinMax;

 
#pragma endregion



};  // class Image


CLOSE_DICOM_NS