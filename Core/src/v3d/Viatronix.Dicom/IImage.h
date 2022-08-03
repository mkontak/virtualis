#pragma once

/// includes
#include "Enums.h"


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
 * Dicom Image implementation
 */
public ref class IImage abstract : public ViewModelBase
{

#pragma region constructor/destructors

protected:
  /// constructor
  IImage() { }



#pragma endregion



#pragma region methods

public:

  /// Clones the image
  virtual IImage ^ Clone() = 0;

  virtual ImageCollection ^ GetFrames() = 0;

  /// Gets the internal shared_ptr
  virtual System::IntPtr GetIDcmImagePtr() = 0;
  

  /// Sets the internal shared_ptr
  virtual void SetIDcmImagePtr(System::IntPtr dcmImagePtr) = 0;

  ///
  virtual String ^ ToXml() = 0;


  // Gets the pixel value
  virtual int GetPixelValue(unsigned int iRow, unsigned int iCol) = 0;

  // Gets the HU value
  virtual int GetHUValue(unsigned int iRow, unsigned int iCol) = 0;

  /// Serializes the element list out as xml ( only plac holders for the binary will be there )
  virtual void Serialize(String ^ msFile) = 0;


  /// Gets the element list
  virtual ElementCollection ^ GetElements() = 0;

  /// Gets the Min/Max Pixel Values
  virtual Generic::List<short> ^ GetMinMaxPixelValues() = 0;
 
  /// Determines if the element exists
  virtual bool Contains(unsigned short iGroup, unsigned short iElement) = 0;
  

  /// Gets the image size in bytes
  virtual int GetImageSize() = 0;

#pragma endregion





#pragma region TryGetValue template methods

public:


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value)
  { return TryGetValue(iGroup, iElement, value, false); }

  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value, bool searchSubElements) = 0;
 virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value)
  { return TryGetValue(iGroup, iElement, value, false); }


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value, bool searchSubElements) = 0;
 virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value)
  { return TryGetValue(iGroup, iElement, value, false); }


  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value, bool searchSubElements) = 0;
 virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value)
  { return TryGetValue(iGroup, iElement, value, false); }

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, float %  value, bool searchSubElements) = 0;
 virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, float %   value)
  { return TryGetValue(iGroup, iElement, value, false); }

  /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value)
  { return TryGetValue(iGroup, iElement, value, false); }

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value)
  { return TryGetValue(iGroup, iElement, value, false); }

   /// Sets a generic value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64 %  value, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64  %   value)
  { return TryGetValue(iGroup, iElement, value, false); }


  /// Sets a string value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, String ^% msValue, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, String ^%   value)
  { return TryGetValue(iGroup, iElement, value, false); }

    
  /// Sets a string value
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^% mpBuffer, bool searchSubElements) = 0;
  virtual bool TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^%   mpBuffer)
  { return TryGetValue(iGroup, iElement, mpBuffer, false); }

#pragma endregion



#pragma region properties

public:

  /// Gets the filename associated with the image
  virtual property String ^ Filename
  { String ^ get() = 0; }

  /// Gets the flag indicating if the image is a frame
  virtual property bool IsFrame
  {  bool get() = 0; }

  /// Gets the flag indicatinga that the image is monochrome
  virtual property bool IsMonochrome
  {  bool get() = 0; }

  // Gets the encapsulated pdf flag
  virtual property bool IsEncapsulatedPdf
  {  bool get() = 0; }

  // Gets the pixel value range
  virtual property int PixelValueRange
  {  int get()  = 0; } // PixelValueRange


  /// Gets the manufacturer
  virtual property String ^ Manufacturer
  { String ^ get()  = 0; }

  /// Gets the manufacturer model number
  virtual property String ^ ManufacturerModel
  { String ^ get()  = 0; }

  virtual property String ^ Institution
  { String ^ get()  = 0; }

  /// Gets the time zone offset from utc
  virtual property String ^ TimeZoneOffsetFromUTC
  { String ^ get()  = 0; }

  /// Gets the acquisition Number
  virtual property __int64 AcquisitionNumber
  {  __int64 get()  = 0;  }
 
  /// Gets the patient name
  virtual property String ^ PatientName
  {  String ^ get()  = 0;  }

  /// Gets the patient dob
  virtual property String ^ PatientDob
  { String ^ get()  = 0; }

   /// Gets the acquisition date
  virtual property String ^ AcquisitionDate
  { String ^ get()  = 0; }

  /// Gets the series date
  virtual property String ^ SeriesDate
  { String ^ get()  = 0; }

  /// Gets the study date
  virtual property String ^ StudyDate
  { String ^ get()  = 0; }

  /// Gets the study description
  virtual property String ^ StudyDescription
  { String ^ get()  = 0; }

  /// Gets the series description
  virtual property String ^ SeriesDescription
  { String ^ get()  = 0; }

    /// Gets the study instance uid
  virtual property String ^ StudyInstanceUid
  {  String ^ get()  = 0; }

  /// Gets the series instance Uid
  virtual property String ^ SeriesInstanceUid
  { String ^ get()  = 0; }

   /// Gets the study id
  virtual property String ^ StudyId
  { String ^ get()  = 0; }

  /// Gets the series number
  virtual property String ^ SeriesNumber
  { String ^ get()  = 0; }

 
  /// Gets the instance uid
  virtual property String ^ InstanceUid
  { String ^ get()  = 0;  }

  /// Gets the Instance Number
  virtual property unsigned int InstanceNumber
  { unsigned int get()  = 0;  }

  /// Gets the SOP class uid
  virtual property String ^ AffectedSopClassUid
  { String ^ get()  = 0; }

   /// Gets the Number of Frames
  virtual property unsigned int NumberOfFrames
  { unsigned int get()  = 0; }

  /// Gets the rows of the image
  virtual property unsigned int Rows
  { unsigned int get()  = 0; }
  
  /// Gets the Columns of the image
  virtual property unsigned int Columns
  { unsigned int get()  = 0; }

   /// Gets the Bits Allocated per pixel
  virtual property unsigned short BitsAllocated
  { unsigned short get()  = 0; }
 
  /// Gets the Rescale Slope
  virtual property float RescaleSlope
  { float get()  = 0; }

  /// Gets the Rescale Intercept
  virtual property float RescaleIntercept
  { float get()  = 0; }

  /// Gets the Samples per pixel of the image
  virtual property unsigned int SamplesPerPixel
  { unsigned int get()  = 0; }
 
  /// Gets the PixelPaddingValue
  virtual property unsigned int PixelPaddingValue
  { unsigned int get()  = 0; }

  /// Gets the WindowCenter
  virtual property double WindowCenter
  { double get()  = 0; }

  /// Gets the WindowWidth
  virtual property double WindowWidth
  {  double get()  = 0; }

  /// Gets the image position 
  virtual property System::Collections::Generic::List<double> ^ ImagePosition
  { System::Collections::Generic::List<double> ^ get()  = 0; }

  /// Gets the pixel spacing and returns the x/y in a list
  virtual property System::Collections::Generic::List<double> ^ PixelSpacing
  { System::Collections::Generic::List<double> ^ get()  = 0; }
 
  /// Gets the image orientation x/y/z/x/y/z in a list
  virtual property System::Collections::Generic::List<double> ^ ImageOrientation
  { System::Collections::Generic::List<double> ^ get()  = 0; }

  /// Gets the plane to image corresponds to
  virtual property ImagePlanes ImagePlane
  { ImagePlanes get()  = 0; }

  // Gets the ImageDateTime 
  virtual property DateTime ImageDateTime
  { DateTime get()  = 0; }

  // Gets the AcquisitionDateTime 
  virtual property DateTime AcquisitionDateTime
  { DateTime get()  = 0; }

  // Gets the Series date and time 
  virtual property DateTime SeriesDateTime
  { DateTime get()  = 0; }

  // Gets the study date and time 
  virtual property DateTime StudyDateTime
  { DateTime get()  = 0; }

  /// Gets the patient sex
  virtual property String ^ PatientSex
  { String ^ get()  = 0; }

  /// Gets the patient weight
  virtual property float PatientWeight
  { float get()  = 0; }

  /// Gets the patient age with units
  virtual property String ^ PatientAge
  {  String ^ get()  = 0; }

  /// Gets the patient birth date and time from the element list
  virtual property DateTime PatientBirthDateTime
  {  DateTime get()  = 0; }

  /// Gets the additional patient history
  virtual property String ^ AdditionalPatientHistory
  { String ^ get()  = 0; }

  /// Gets the operator name
  virtual  property String ^ OperatorName
  { String ^ get()  = 0; }

  /// Gets the referring physician
  virtual property String ^ ReferringPhysician
  { String ^ get()  = 0; }

  /// Gets the reconstruction diameter
  virtual property float ReconstructionDiameter
  { float get()  = 0; }

  /// Gets the kvp
  virtual property float Kvp
  { float get()  = 0; }

  /// Gets the xray tube current
  virtual property float XrayTubeCurrent
  { float get()  = 0; }

 
  /// Gets the patient position
  virtual property String ^ PatientPosition
  { String ^ get()  = 0; }

  /// Gets the institution name
  virtual property String ^ InstitutionName
  { String ^ get()  = 0; }

  /// Gets the accession number
  virtual property String ^ AccessionNumber
  { String ^ get()  = 0; }

  /// Gets the medical id (Same as patient id)
  virtual property String ^ MedicalId
  { String ^ get() { return this->PatientId; } }

  /// Gets the patient id 
  virtual property String ^ PatientId
  { String ^ get()  = 0; }

  /// Sets/Gets the modality
  virtual property String ^ Modality
  { String ^ get()  = 0;  }

  /// Gets the pixel data
  virtual property Viatronix::Dicom::Buffer ^ PixelData
  { Viatronix::Dicom::Buffer ^ get()  = 0; }

  // Gets/Sets the pixel representation
  virtual property unsigned short PixelRepresentation
  { unsigned short get()  = 0; }

#pragma endregion




};  // class Image


CLOSE_DICOM_NS