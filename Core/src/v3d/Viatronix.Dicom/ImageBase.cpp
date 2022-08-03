#include "Stdafx.h"
#include "Utility.h"
#include "ImageBase.h"
#include "Element.h"
#include "Buffer.h"
#include "Sequence.h"
#include "ElementCollection.h"
#include "Element.h"
#include "ImageCollection.h"
#include "DcmTriple.h"

using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

USING_DICOM_NS

/**
 * Constructor
 */
ImageBase::ImageBase() : 
m_pDcmImage(scoped_ptr<std::shared_ptr<dcm::IDcmImage>>(__nullptr)),
m_mpMinMax(nullptr),
m_pSeperators(gcnew array<Char>(1))
{
  m_pSeperators[0] = '/';
} // Image()


/**
 * Destructor
 */
ImageBase::~ImageBase()
{
  this->!ImageBase();
} // ~Image()
  
/**
 * Finalizer
 */
ImageBase::!ImageBase()
{
  m_pDcmImage.reset(__nullptr);
} // !Image

/**
 * Returns the std::shared_ptr 
 */
System::IntPtr ImageBase::GetIDcmImagePtr()
{
  return System::IntPtr((void *)(m_pDcmImage.get()));
} // GetIDcmImagePtr()


/**
 * Determines if the element exists in this object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 *
 * @return  Return true if present, false otherwise
 */
bool ImageBase::Contains(unsigned short iGroup, unsigned short iElement)
{

  return std::dynamic_pointer_cast<dcm::DcmImage>(*m_pDcmImage)->Contains(DcmTag(iGroup,iElement));

} // Contains(unsigned short iGroup, unsigned short iElement)

/**
 * Gets the Multi-Frame collections
 */
ImageCollection ^ ImageBase::GetFrames()
{
  if ( ! IsFrame )
  {
    std::shared_ptr<dcm::DcmImageCollection> * pImage(new std::shared_ptr<dcm::DcmImageCollection>((std::dynamic_pointer_cast<dcm::DcmImage>(*m_pDcmImage)->GetFrames())));

    return ImageCollection::Create(System::IntPtr(pImage));
  }
  else
    return ImageCollection::Create();

} // GetMultiFrameCollections()

/**
 * Gets an element list
 */
ElementCollection ^ ImageBase::GetElements()
{

  std::shared_ptr<dcm::DcmElementCollection> * pElementCollection( new std::shared_ptr<dcm::DcmElementCollection>((*m_pDcmImage)->GetElements()));

  return ElementCollection::Create(System::IntPtr((void *)pElementCollection));

}

/**
 * Gets the image size in bytes
 *
 * @return Image size in bytes
 */
int ImageBase::GetImageSize()
{
  return (*m_pDcmImage)->GetImageSize<char>();
} // GetImageSize()


/** 
 * Gets the pixel value specified by the row and column
 *
 * @param iRow      Row
 * @param iCol      Column
 *
 * @return pixel value
 */
int ImageBase::GetPixelValue(unsigned int x, unsigned int y)
{
  return (*m_pDcmImage)->GetPixelValue(x,y);
} // GetPixelValue()

/** 
 * Gets the HU value for the pixel for the pixel
 *
 * @param iRow    Row within the pixel data
 * @param iCol    Column within the pixel data
 * 
 * @return Hounsfield Unit for the pixel value requested
 */
int ImageBase::GetHUValue(unsigned int x, unsigned int y)
{
  String ^ msManufacturer(this->Manufacturer->ToUpper());
  String ^ msModality(this->Modality->ToUpper());
  float rescaleSlope(this->RescaleSlope);
  float rescaleIntercept(this->RescaleIntercept);

  // Account for bug in TOSHIBA datasets where the rescale intercept is set to 0 erroneously
  //if ( msManufacturer->Contains("TOSHIBA") && msModality == "CT" && rescaleIntercept == 0.0F  )
  //  rescaleIntercept = -1024.0F;

  return (int)( ( (float)GetPixelValue(x,y) * rescaleSlope ) + rescaleIntercept );

} // GetHUValue(unsigned int iRow, unsigned int iCol)

/** 
 * Converts the element list to an xml string
 *
 * @return  Xml representation of the dicom header
 */
String ^ ImageBase::ToXml()
{

  return gcnew String((*m_pDcmImage)->GetHeader().c_str());

} // ToXml()

/**
 * Gets the date and time values specified by the group and elements combining them into a single
 * DateTime structure. If either dicom element is missing the current date and or time will be used
 *
 * @param iDateGoup     Dates group 
 * @param iDateElement  Dates element 
 * @param iTimeGroup    Times group
 * @param iTimeElement  Times element
 *
 * @return DateTime object containing the dicom date and time specified.
 */
DateTime ImageBase::ToDateTime(String ^ msDate, String ^ msTime)
{
  DateTime now = DateTime::Now;

  if ( msDate->Length < 8 )
    msDate = now.ToString("yyyyMMdd");

  if ( msTime->Length < 6 )
        msTime = msTime + "000000";

  //=======================================================================================
  // Required to convert the time due to invalid times received within some dicom fields
  //=======================================================================================
  int hour(Int32::Parse(msTime->Substring(0,2)));
  int minute(Int32::Parse(msTime->Substring(2,2)));
  int second(Int32::Parse(msTime->Substring(4,2)));


  // Comparisons are maed to make sure the time does not exceed any values.
  String ^ msDateTime = String::Format("{0}-{1}-{2}", msDate->Substring(0,4), msDate->Substring(4,2), msDate->Substring(6,2) ) + " " +
                        String::Format("{0:D2}:{1:D2}:{2:D2}", ( hour < 24 ? hour : 23 ), ( minute < 60 ? minute : 59 ), ( second < 60 ? second : 59 ) ); 

  return DateTime::Parse(msDateTime);

} // GetDateTime(unsigned short iDateGroup, unsigned short iDateElement, unsigned short iTimeGroup, unsigned short iTimeElement)

/**
 * Serializes the element list out the the specified file as xml. Only place holders 
 * for the binary data will be present
 *
 * @param msFile      File to serialize to
 *
 */
void ImageBase::Serialize(String ^ msFile)
{

  XmlTextWriter ^ mpWriter = nullptr;

  try
  {
    XmlDocument ^ mpDoc = gcnew XmlDocument();

    mpDoc->LoadXml(ToXml());

    mpWriter = gcnew XmlTextWriter(msFile, Encoding::ASCII);

    mpWriter->Formatting = Formatting::Indented;

    mpDoc->Save(mpWriter);
  }
  finally
  {
    if ( mpWriter != nullptr )
      mpWriter->Close();
  }

} // Serialize(String ^ msFile)




#pragma region properties


/**
 * Gets the file name associated with the image
 */
String ^ ImageBase::Filename::get()
{
  return gcnew String( (*m_pDcmImage)->GetFilename().c_str() );
}


/**
 * Determines if the image is a frame 
 */
bool ImageBase::IsFrame::get()
{
  return  (*m_pDcmImage)->IsFrame();
}


/**
 * Determines if the image ia monchorome of crgb
 */
bool ImageBase::IsMonochrome::get()
{
  return  (*m_pDcmImage)->IsMonochrome();
}

/**
 * Gets the encapsulated pdf
 */
bool ImageBase::IsEncapsulatedPdf::get()
{
  return (*m_pDcmImage)->IsEncapsulatedPdf();
} // IsEncapsulatedPdf::get()


/** 
 * Gets the image date and time
 */
DateTime ImageBase::ImageDateTime::get()
{
  std::string sDate("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_InstanceCreationDate, sDate);

  std::string sTime("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_InstanceCreationTime, sTime);

  return ToDateTime(  gcnew String(sDate.c_str()),
                       gcnew String(sTime.c_str()) );

} // ImageDateTime


/**
 * Gets the acquisition number
 */
__int64 ImageBase::AcquisitionNumber::get()
{
  __int64 iNumber(0);
  (*m_pDcmImage)->TryGetValue<__int64>(DCM_AccessionNumber, iNumber);
  return iNumber;
} // AcquisitionNumber::get()



/**
 * Gets the series instance Uid
 */
String ^ ImageBase::StudyId::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_StudyID, sValue);

  return gcnew String(sValue.c_str());
} // StudyId

/**
 * Gets the series number
 */
String ^ ImageBase::SeriesNumber::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_SeriesNumber, sValue);

  return gcnew String(sValue.c_str());
} // SeriesNumber


/**
 * Gets the instance number
 */
unsigned int ImageBase::InstanceNumber::get()
{

  unsigned int iNumber(0);
  (*m_pDcmImage)->TryGetValue<unsigned int>(DCM_InstanceNumber, iNumber);

  return iNumber;
} // InstanceNumber::get()


/*
 * Gets the patient age from the dicom element list with the units indicator as the last character.
 * If no units are supplier the YEAR is assumed.
 *
 * @return  The patient age with teh units indicator as the last character
 */
String ^ ImageBase::PatientAge::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_PatientAge, sValue);

  return gcnew String(sValue.c_str());
} // PatientAge::get()

/*
 * Gets the patient weight from the dicom element list
 *
 * @return  The patient weight
 */
float ImageBase::PatientWeight::get()
{
  float fNumber(0);
  (*m_pDcmImage)->TryGetValue<float>(DCM_PatientWeight, fNumber);

  return fNumber;
} // PatientWeight::get()


/*
 * Gets the patient sex from the dicom element list
 *
 * @return  Returns the gender ( M, F or O ).
 */
String ^ ImageBase::PatientSex::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_PatientSex, sValue);

  return gcnew String(sValue.c_str());
} // PatientSex::get()

/** 
 * Gets the acquisition date and time
 */
String ^ ImageBase::AcquisitionDate::get()
{
  std::string sDate("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_AcquisitionDate, sDate);

  return gcnew String(sDate.c_str());
} // AcquisitionDateTime

/** 
 * Gets the acquisition date and time
 */
DateTime ImageBase::AcquisitionDateTime::get()
{
  std::string sDate("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_AcquisitionDate, sDate);

  std::string sTime("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_AcquisitionTime, sTime);

  return ToDateTime(  gcnew String(sDate.c_str()),
                       gcnew String(sTime.c_str()) );

} // AcquisitionDateTime

/** 
 * Gets the series date and time
 */
DateTime ImageBase::SeriesDateTime::get()
{
  return ToDateTime(  gcnew String((*m_pDcmImage)->GetSeriesDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetSeriesTime().ToString().c_str()) );

} // SeriesDateTime

/** 
 * Gets the study date and time
 */
DateTime ImageBase::StudyDateTime::get()
{
   return ToDateTime(  gcnew String((*m_pDcmImage)->GetStudyDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetStudyTime().ToString().c_str()) );


} // StudyDateTime



/** 
 * Gets the image plane
 */
ImagePlanes ImageBase::ImagePlane::get()
{
  return Utility::GetImagePlane(ImageOrientation);
} // ImagePlane

/**
 * Gets the manufacturer
 */
String ^ ImageBase::Manufacturer::get()
{
  return gcnew String((*m_pDcmImage)->GetManufacturer().c_str());
} // Manufacturer::get()

/**
 * Gets the Manufacturer Model
 */
String ^ ImageBase::ManufacturerModel::get()
{
  return gcnew String((*m_pDcmImage)->GetManufacturerModel().c_str());
} // ManufacturerModel::get()

/**
 * Gets the Institution name
 */
String ^ ImageBase::Institution::get()
{
  return gcnew String((*m_pDcmImage)->GetInstitutionName().c_str());
} // Institution::get()

/**
 * Gets the time zone offset from UTC
 */
String ^ ImageBase::TimeZoneOffsetFromUTC::get()
{
  return gcnew String((*m_pDcmImage)->GetTimeZoneOffsetFromUTC().c_str());
} // TimeZoneOffsetFromUTC::get()



/**
 * Gets the number of frames
 */
unsigned int ImageBase::NumberOfFrames::get()
{
  return (*m_pDcmImage)->GetNumberOfFrames();
} // NumberOfFrames::get()

/**
 * Gets the patient name
 */
String ^ ImageBase::PatientName::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientName().ToString().c_str());
} // PatientName::get()

/**
 * Gets the patient dob
 */
String ^ ImageBase::PatientDob::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientDob().ToString().c_str());
} // PatientDob

/**
 * Gets the series date
 */
String ^ ImageBase::SeriesDate::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesDate().ToString().c_str());
} // SeriesDate

/**
 * Gets the series date
 */
String ^ ImageBase::StudyDate::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyDate().ToString().c_str());
} // StudyDate

/**
 * Gets the study description
 */
String ^ ImageBase::StudyDescription::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyDescription().c_str());
} // StudyDescription

/**
 * Gets the series description
 */
String ^ ImageBase::SeriesDescription::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesDescription().c_str());
} // SeriesDescription

/**
 * Gets the study instance uid
 */
String ^ ImageBase::StudyInstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyInstanceUid().c_str());
 } // StudyInstanceUid

/**
 * Gets the series instance Uid
 */
String ^ ImageBase::SeriesInstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesInstanceUid().c_str());
} // SeriesInstanceUid


/**
 * Gets the instance uid
 */
String ^ ImageBase::InstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetInstanceUid().c_str());
} // InstanceUid::get()


/**
 * Gets the instance uid
 */
String ^ ImageBase::AffectedSopClassUid::get()
{
  return gcnew String((*m_pDcmImage)->GetAffectedSopClassUid().c_str());
} // AffectedSopClassUid::get()


/**
 * Gets the rows of the image
 */
unsigned int ImageBase::Rows::get()
{
  return (*m_pDcmImage)->GetRows();
} // Rows::get()
  
/**
 * Gets the Columns of the image
 */
unsigned int ImageBase::Columns::get()
{
  return (*m_pDcmImage)->GetColumns();
} // Columns::get()

/**
 * Gets the Bits Allocated per pixel
 */
unsigned short ImageBase::BitsAllocated::get()
{
  return (*m_pDcmImage)->GetBitsAllocated(); 
} // BitsAllocated::get()

/** 
 * Gets the Rescale Slope
 */
float ImageBase::RescaleSlope::get()
{
 return (*m_pDcmImage)->GetRescaleSlope();
} // get_RescaleSlope()

/**
 * Gets the Rescale Intercept
 */
float ImageBase::RescaleIntercept::get()
{
  return (*m_pDcmImage)->GetRescaleIntercept();
} // RescaleIntercept::get()


/**
 * Gets the Samples per pixel of the image
 */
unsigned int ImageBase::SamplesPerPixel::get()
{
  return (*m_pDcmImage)->GetSamplesPerPixel();
} // SamplesPerPixel::get()

/**
 * Gets the PixelPaddingValue
 */
unsigned int ImageBase::PixelPaddingValue::get()
{
  return (*m_pDcmImage)->GetPixelPaddingValue();
} // PixelPaddingValue::get()

/** 
 * Gets the WindowCenter
 */
double ImageBase::WindowCenter::get()
{
  return (*m_pDcmImage)->GetWindowCenter();
} // WindowCenter::get()


/**
 * Gets the WindowWidth
 */
double ImageBase::WindowWidth::get()
{
return (*m_pDcmImage)->GetWindowWidth();
} // WindowWidth::get()

/**
 * Gets the pixel value range
 */
int ImageBase::PixelValueRange::get()
{
  Generic::List<short> ^ mpMinMax = GetMinMaxPixelValues();
  return (mpMinMax[1] - mpMinMax[0]);
} // PixelValueRange::get()

/**
 * Gets the image position and returns the x/y/z in a list
 */
System::Collections::Generic::List<double> ^ ImageBase::ImagePosition::get()
{
  System::Collections::Generic::List<double> ^ mpImagePosition = gcnew System::Collections::Generic::List<double>();

  dcm::DcmTriple<float> imagePosition = (*m_pDcmImage)->GetImagePosition();

  mpImagePosition->Add(imagePosition.GetX());
  mpImagePosition->Add(imagePosition.GetY());
  mpImagePosition->Add(imagePosition.GetZ());

  return mpImagePosition;

} // ImagePosition::get()


/**
 * Gets the pixel spacing and returns the x/y in a list
 */
System::Collections::Generic::List<double> ^ ImageBase::PixelSpacing::get()
{
  System::Collections::Generic::List<double> ^ mpPixelSpacing = gcnew System::Collections::Generic::List<double>();

  dcm::DcmDouble<double> pixelSpacing = (*m_pDcmImage)->GetPixelSpacing();

  mpPixelSpacing->Add(pixelSpacing.GetX());
  mpPixelSpacing->Add(pixelSpacing.GetY());


  return mpPixelSpacing;

} // PixelSpacing::get()

/**
 * Gets the pixel spacing and returns the x/y in a list
 */
System::Collections::Generic::List<double> ^ ImageBase::ImageOrientation::get()
{
  System::Collections::Generic::List<double> ^ mpCosines = gcnew System::Collections::Generic::List<double>();

  dcm::DcmCosines cosines = (*m_pDcmImage)->GetImageOrientation();

  mpCosines->Add(cosines.GetRowX());
  mpCosines->Add(cosines.GetRowY());
  mpCosines->Add(cosines.GetRowZ());
  mpCosines->Add(cosines.GetColumnX());
  mpCosines->Add(cosines.GetColumnY());
  mpCosines->Add(cosines.GetColumnZ());


  return mpCosines;

} // ImageOrientation::get()


/** 
 * Gets the patients birth date and time
 */
DateTime ImageBase::PatientBirthDateTime::get()
{

  std::string sBirthTime("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_PatientBirthTime, sBirthTime);

  return ToDateTime(  gcnew String((*m_pDcmImage)->GetPatientDob().ToString().c_str()),
                      gcnew String(sBirthTime.c_str()) );

} // PatientBirthDateTime


/*
 * Gets the additional patient history from the dicom element list.
 *
 * @return  any additinal patient history
 */
String ^ ImageBase::AdditionalPatientHistory::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_AdditionalPatientHistory, sValue);


  return gcnew String(sValue.c_str());
} // AdditionalPatientHistory::get()

/*
 * Gets the operator name
 *
 * @return  The operator name
 */
String ^ ImageBase::OperatorName::get()
{
    std::string sValue("");
  (*m_pDcmImage)->TryGetValue<std::string>(DCM_OperatorsName, sValue);


  return gcnew String(sValue.c_str());
} // OperatorName::get()

/*
 * Gets the operator name
 *
 * @return  The operator name
 */
String ^ ImageBase::ReferringPhysician::get()
{
  return gcnew String((*m_pDcmImage)->GetReferringPhysician().ToString().c_str());
} // ReferringPhysician::get()

/*
 * Gets the reconstruction diameter
 *
 * @return  The reconstruction diameter
 */
float ImageBase::ReconstructionDiameter::get()
{   
  float fValue(0.0F);
  (*m_pDcmImage)->TryGetValue<float>(DCM_ReconstructionDiameter, fValue);
  return fValue;
 } // ReconstructionDiameter::get()



/*
 * Gets the kvp
 *
 * @return  The kvp
 */
float ImageBase::Kvp::get()
{
   float fValue(0.0F);
  (*m_pDcmImage)->TryGetValue<float>(DCM_KVP, fValue);
  return fValue;
 } // Kvp::get()


/*
 * Gets the xray tube current
 *
 * @return  The xray tube current
 */
float ImageBase::XrayTubeCurrent::get()
{
   float fValue(0.0F);
  (*m_pDcmImage)->TryGetValue<float>(DCM_XRayTubeCurrent, fValue);
  return fValue;
} // XrayTubeCurrent::get()



/*
 * Gets the patient position
 *
 * @return  The patient position
 */
String ^ ImageBase::PatientPosition::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientPosition().c_str());
} // PatientPosition::get()


/*
 * Gets the accession number 
 *
 * @return  The accession number
 */
String ^ ImageBase::AccessionNumber::get()
{
  return gcnew String((*m_pDcmImage)->GetAccessionNumber().c_str());
} // AccessionNumber::get()

/*
 * Gets the institution name 
 *
 * @return  The institution name
 */
String ^ ImageBase::InstitutionName::get()
{
  return gcnew String((*m_pDcmImage)->GetInstitutionName().c_str());
} // InstitutionName::get()

/*
 * Gets the patient id
 *
 * @return  The patient id
 */
String ^ ImageBase::PatientId::get()
{
  return gcnew String((*m_pDcmImage)->GetMedicalId().c_str());
} // PatientId::get()



/*
 * Gets the modality
 *
 * @return  The modality
 */
String ^ ImageBase::Modality::get()
{
  return gcnew String((*m_pDcmImage)->GetModality().c_str());
} // Modality::get()


/*
 * Gets the pixel data
 *
 * @return Buffer containing the pixel data
 */
Viatronix::Dicom::Buffer ^ ImageBase::PixelData::get()
{

  std::shared_ptr<dcm::DcmBuffer> pDcmBuffer((*m_pDcmImage)->GetPixelData());

 
  return Buffer::Create(IntPtr((void *)&pDcmBuffer));

} // PixelDtata::get()


/*
 * Gets the pixel representation
 *
 * @return  pixel representation
 */
unsigned short ImageBase::PixelRepresentation::get()
{
  return (*m_pDcmImage)->GetPixelRepresentation();
} // PixelRepresentation::get()


/**
 * Gets the Min/Max values
 */
Generic::List<short> ^ ImageBase::GetMinMaxPixelValues()
{

    std::pair<short, short> minmax(SHRT_MAX,SHRT_MIN);

    (*m_pDcmImage)->GetMinMaxPixelValues<short>(minmax);

    Generic::List<short> ^ mpMinMax =  gcnew Generic::List<short>();

    mpMinMax->Add(minmax.first);
    mpMinMax->Add(minmax.second);

    return mpMinMax;

 }  // GetMinMaxPixelValues()

#pragma endregion



#pragma region TryGetValue template methods


/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^% mpBuffer, bool searchSubElements )
{

  std::shared_ptr<dcm::DcmBuffer> buffer = std::shared_ptr<dcm::DcmBuffer>(new dcm::DcmBuffer());
  bool bFound =  std::dynamic_pointer_cast<dcm::IDcmImage>((*m_pDcmImage))->TryGetValue<std::shared_ptr<dcm::DcmBuffer>>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), buffer, searchSubElements);
  mpBuffer->Reset(System::IntPtr((void *)&buffer));

  return bFound;

} // TryGetValue(unsigned short iGroup, unsigned short iElement, String ^ value)

/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value, bool searchSubElements)
{
  int iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}

/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value, bool searchSubElements)
{
  unsigned int iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<unsigned int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}


/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value, bool searchSubElements)
{
  short iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}


/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value, bool searchSubElements)
{
  unsigned short iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<unsigned short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}

/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, float %  value, bool searchSubElements)
{
  float fValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<float>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue, searchSubElements);
  value = fValue;

  return bFound;
}


/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value, bool searchSubElements)
{
  double fValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<double>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue, searchSubElements);
  value = fValue;

  return bFound;
}

/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value, bool searchSubElements)
{
  __int64 iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<__int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}

/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */

bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64 %  value, bool searchSubElements)
{
  unsigned __int64 iValue;
  bool  bFound =  (*m_pDcmImage)->TryGetValue<unsigned __int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue, searchSubElements);
  value = iValue;

  return bFound;
}


/**
 * Attempts to retrieve the element from the dicom image object
 *
 * @param iGroup      Group id
 * @param iElement    Element id
 * @param value       Value to be set
 *
 * @returns True is found, false if not
 */
bool ImageBase::TryGetValue(unsigned short iGroup, unsigned short iElement, String ^% msValue, bool searchSubElements)
{

  std::string sValue;

  bool bReturn((*m_pDcmImage)->TryGetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue, searchSubElements));

  msValue = gcnew String(sValue.c_str());

  return bReturn;

} // TryGetValue(unsigned short iGroup, unsigned short iElement, String ^ value)


#pragma endregion
