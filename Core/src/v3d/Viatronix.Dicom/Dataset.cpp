#include "Stdafx.h"
#include "Utility.h"
#include "Dataset.h"
#include "Element.h"
#include "Buffer.h"
#include "Sequence.h"
#include "SequenceItem.h"
#include "ElementCollection.h"
#include "Element.h"


using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;
using namespace System::Drawing;
using namespace System::ComponentModel;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

USING_DICOM_NS

/**
 * Constructor
 */
Dataset::Dataset() : 
m_pDcmImage(scoped_ptr<std::shared_ptr<dcm::DcmImage>>(__nullptr)),
m_mpMinMax(nullptr),
m_pSeperators(gcnew array<Char>(1))
{
  m_pSeperators[0] = '/';
} // Dataset()


/**
 * Destructor
 */
Dataset::~Dataset()
{
  this->!Dataset();
} // ~Dataset()
  
/**
 * Finalizer
 */
Dataset::!Dataset()
{
  m_pDcmImage.reset(__nullptr);
} // !Dataset


/**
 * Gets the underlying object.
 *
 * @return  The underlying object.
 */
System::IntPtr Dataset::GetElementPtr()
{
  return System::IntPtr((void *)m_pDcmImage.get());
} // GetElementPtr()


/**
 * Clones the dataset
 */
Dataset ^ Dataset::Clone()
{

  Dataset ^ mpDataset(gcnew Dataset());

  if ( mpDataset == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Dataset for clone");

  mpDataset->m_pDcmImage.reset(new std::shared_ptr<dcm::DcmImage>((*m_pDcmImage)->Clone()));


  return mpDataset;

} // Clone()


#pragma region Create Methods

/** 
 * Creates the element list from the xml node
 *
 * @param msString    String
 *
 * @return Dataset
 */
Dataset ^ Dataset::Create(String ^ msParameter, ParameterType type)
{

  /// Create empty object
  Dataset ^ mpDataset(gcnew Dataset());

  // Make sure memory was allocated
  if ( mpDataset == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Dataset");

  mpDataset->m_pDcmImage.reset(new std::shared_ptr<dcm::DcmImage>(dcm::DcmImageFactory::Create(ss(msParameter), (dcm::DcmParameterType)type)));


  return mpDataset;

} // Create(XmlNode ^ mpNode)

/** 
 * Creates the element list from the xml node
 *
 * @param msFilename      File to use for initialization
 * @param msHeader        Header to initialize with
 *
 * @return Dataset
 */
Dataset ^ Dataset::Create(String ^ msFilename, String ^ msHeader)
{

  /// Create empty object
  Dataset ^ mpDataset(gcnew Dataset());

  // Make sure memory was allocated
  if ( mpDataset == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Dataset");

  mpDataset->m_pDcmImage.reset(new std::shared_ptr<dcm::DcmImage>(dcm::DcmImageFactory::Create(ss(msFilename),ss(msHeader))));


  return mpDataset;

} // Create(XmlNode ^ mpNode)

/** 
 * Creates the element list from the xml node
 *
 * @param mpNode    Xml representation of the dicom header
 *
 * @return Dataset
 */
Dataset ^ Dataset::Create(XmlNode ^ mpNode)
{
  return Create(mpNode->OuterXml, ParameterType::HEADER);
} // Create(XmlNode ^ mpNode)


/** 
 * Creates the element list from the unmanaged dataset pointer
 *
 * @param dcmDatasetPtr     Unmanaged DcmDataset pointer
 *
 * @return Dataset
 */
Dataset ^ Dataset::Create(IntPtr dcmImagePtr)
{
  
  dcm::DcmImage * pDcmImage(reinterpret_cast<dcm::DcmImage *>(dcmImagePtr.ToPointer()));

  XmlDocument ^ mpDoc = gcnew XmlDocument();

  mpDoc->LoadXml(gcnew String(pDcmImage->GetHeader().c_str()));

  /// Create the element list using the dicom document
  Dataset ^ mpDataset = Create(mpDoc->DocumentElement);

  /// Set the DcmImage
  mpDataset->m_pDcmImage.reset(new std::shared_ptr<dcm::DcmImage>(pDcmImage));


  return mpDataset;


} // Create(IntPtr dcmDatasetPtr)

/** 
 * Creates the element list from the xml node
 *
 * @param msHeader      Header
 * @param mpBitmap      Bitmap
 *
 * @return Dataset
 */
Dataset ^ Dataset::Create(String ^ msHeader, Bitmap ^ mpBitmap)
{

  /// Create empty object
  Dataset ^ mpDataset(gcnew Dataset());

  // Make sure memory was allocated
  if ( mpDataset == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Dataset");



  unsigned int iWidth(mpBitmap->Width);
  unsigned int iHeight(mpBitmap->Height);



  BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,iWidth, iHeight), ImageLockMode::ReadOnly, mpBitmap->PixelFormat);


  try
  {
    const unsigned int iBitmapInfoSize  = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
    const unsigned int iImageSize       = mpBitmapData->Height *  abs(mpBitmapData->Stride);
 
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize            = iBitmapInfoSize;
	  bmi.bmiHeader.biWidth           = iWidth;
	  bmi.bmiHeader.biHeight          = iHeight;
	  bmi.bmiHeader.biSizeImage       = iBitmapInfoSize + iImageSize;
    bmi.bmiHeader.biPlanes          = 1;
    bmi.bmiHeader.biBitCount        = 24;
    bmi.bmiHeader.biCompression     = BI_RGB;
	  bmi.bmiHeader.biClrUsed         = 0;
	  bmi.bmiHeader.biClrImportant    = 0;
	  bmi.bmiHeader.biXPelsPerMeter   = 300;
	  bmi.bmiHeader.biYPelsPerMeter   = 300;

 
    std::shared_ptr<Gdiplus::Bitmap> pBitmap(new Gdiplus::Bitmap((LPBITMAPINFO)&bmi,(LPVOID)mpBitmapData->Scan0));


    mpDataset->m_pDcmImage.reset(new std::shared_ptr<dcm::DcmImage>(dcm::DcmImageFactory::Create(ss(msHeader), pBitmap )));

  }
  catch ( dcm::DcmUnsupportedException & ex )
  {
    throw gcnew UnsupportedException(String::Format("Unsupported exception : {0}", gcnew String(ex.GetTextMessage().c_str())));
  }
  catch ( dcm::DcmException & ex )
  {
   throw gcnew DicomException(String::Format("Failed to set pixel data : {0}", gcnew String(ex.GetTextMessage().c_str()))); 
  }
  catch ( ... )
  {
    throw gcnew DicomException("Failed to set pixel data, unspecified error"); 
  }
  finally
  {
    mpBitmap->UnlockBits(mpBitmapData);
  }

 

  return mpDataset;

} // Create(String ^ msHeader, Bitmap ^ mpBitmap)


#pragma endregion

///**
// * Inserts the element
// */
//void Insert(IElement ^ mpElement)
//{
//
//  if ( mpElement->IsSequence )
//  {
//    Sequence ^ mpSequence(dynamic_cast<Sequence ^>(mpElement));
//  }
//  else
//  {
//    Element ^ mpElement(dynamic_cast<Element ^>(mpElement));
//
//
//  }
//
//} // Insert(IElement ^ mpElement)

/**
 * Gets an element list
 */
ElementCollection ^ Dataset::GetElementCollection()
{

  std::shared_ptr<dcm::DcmElementCollection> * pElementCollection( new std::shared_ptr<dcm::DcmElementCollection>((*m_pDcmImage)->GetElementCollection()));

  return ElementCollection::Create(System::IntPtr((void *)pElementCollection));

}


/**
 * Modifies the dicom header
 * 
 * @param sXml
 */
void Dataset::Modify(String ^ sXml)
{
  (*m_pDcmImage)->Modify(ss(sXml));
} // Modify(String ^ sXml)

/**
 * Remove the element specified
 * 
 * @param iGroup    Group id
 * @param iElement  Element Id
 */
void Dataset::Remove(unsigned short iGroup, unsigned short iElement)
{
  (*m_pDcmImage)->Remove(DcmTag(iGroup, iElement));
} // Remove(unsigned short iGroup, unsigned short iElement)

/**
 * Clears the dicom header
 */
void Dataset::Clear()
{
  (*m_pDcmImage)->Clear();
} // Clear()


/**
 * Returns true if the element is in the list false otherwise
 *
 * @param iGroup      Group id
 * @aram iElement     Element id
 *
 * @return TRue or False
 */
bool Dataset::Contains(unsigned short iGroup, unsigned short iElement)
{
  
  return (*m_pDcmImage)->Contains(DcmTag(iGroup, iElement));
} // Contains(unsigned short iGroup, unsigned short iElement)


/**
 * Returns the serialized dicom header.
 */
String ^ Dataset::GetSerializedHeader()
{
  return gcnew String((*m_pDcmImage)->GetSerializedHeader().c_str());
} // GetSerializedHeader()


/** 
 * Gets the pixel value specified by the row and column
 *
 * @param iRow      Row
 * @param iCol      Column
 *
 * @return pixel value
 */
int Dataset::GetPixelValue(unsigned int iRow, unsigned int iCol)
{

  return (*m_pDcmImage)->GetPixelValue(iRow,iCol);

} // GetPixelValue()

/** 
 * Gets the HU value for the pixel for the pixel
 *
 * @param iRow    Row within the pixel data
 * @param iCol    Column within the pixel data
 * 
 * @return Hounsfield Unit for the pixel value requested
 */
int Dataset::GetHUValue(unsigned int iRow, unsigned int iCol)
{
  String ^ msManufacturer(this->Manufacturer->ToUpper());
  String ^ msModality(this->Modality->ToUpper());
  float rescaleSlope(this->RescaleSlope);
  float rescaleIntercept(this->RescaleIntercept);

  // Account for bug in TOSHIBA datasets where the rescale intercept is set to 0 erroneously
  //if ( msManufacturer->Contains("TOSHIBA") && msModality == "CT" && rescaleIntercept == 0.0F  )
  //  rescaleIntercept = -1024.0F;

  return (int)( ( (float)GetPixelValue(iRow,iCol) * rescaleSlope ) + rescaleIntercept );

} // GetHUValue(unsigned int iRow, unsigned int iCol)

/** 
 * Converts the element list to an xml string
 *
 * @return  Xml representation of the dicom header
 */
String ^ Dataset::ToXml()
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
DateTime Dataset::ToDateTime(String ^ msDate, String ^ msTime)
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
void Dataset::Serialize(String ^ msFile)
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



/**
 * Saves the dicom to the file/path specified
 *
 * @param sFilePath
 */
void Dataset::Save(String ^ msFilePath)
{

   String ^ msFile(msFilePath);

  // =============================================================================
  // If the msFilePath is a directory then name the file using the instance uid
  // =============================================================================
  if ( System::IO::Directory::Exists(msFilePath) )
    msFile = System::IO::Path::Combine(msFilePath, InstanceUid->Replace(".","_") + ".dcm");

  (*m_pDcmImage)->Save(ss(msFile));

} // Save(String ^ sFilePath)


/**
 * Sets teh pixel data using the bitmap supplied
 *
 * @param mpBitmap
 */
void Dataset::SetPixelData(System::Drawing::Bitmap ^ mpBitmap)
{
  //m_pMinMax = nullptr;

 unsigned int iWidth(mpBitmap->Width);
  unsigned int iHeight(mpBitmap->Height);



  BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,iWidth, iHeight), ImageLockMode::ReadOnly, mpBitmap->PixelFormat);


  try
  {
    const unsigned int iBitmapInfoSize  = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
    const unsigned int iImageSize       = mpBitmapData->Height *  abs(mpBitmapData->Stride);
 
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize            = iBitmapInfoSize;
	  bmi.bmiHeader.biWidth           = iWidth;
	  bmi.bmiHeader.biHeight          = iHeight;
	  bmi.bmiHeader.biSizeImage       = iBitmapInfoSize + iImageSize;
    bmi.bmiHeader.biPlanes          = 1;
    bmi.bmiHeader.biBitCount        = 24;
    bmi.bmiHeader.biCompression     = BI_RGB;
	  bmi.bmiHeader.biClrUsed         = 0;
	  bmi.bmiHeader.biClrImportant    = 0;
	  bmi.bmiHeader.biXPelsPerMeter   = 300;
	  bmi.bmiHeader.biYPelsPerMeter   = 300;

 
    std::shared_ptr<Gdiplus::Bitmap> pBitmap(new Gdiplus::Bitmap((LPBITMAPINFO)&bmi,(LPVOID)mpBitmapData->Scan0));

    (*m_pDcmImage)->SetPixelData(pBitmap);

  }
  catch ( dcm::DcmUnsupportedException & ex )
  {
    throw gcnew UnsupportedException(String::Format("Unsupported exception : {0}", gcnew String(ex.GetTextMessage().c_str())));
  }
  catch ( dcm::DcmException & ex )
  {
   throw gcnew DicomException(String::Format("Failed to set pixel data : {0}", gcnew String(ex.GetTextMessage().c_str()))); 
  }
  catch ( ... )
  {
    throw gcnew DicomException("Failed to set pixel data, unspecified error"); 
  }
  finally
  {
    mpBitmap->UnlockBits(mpBitmapData);
  }



} // SetPixelData(SetPixelData(System::Drawing::Bitmap ^ mpBitmap)

//generic<typename T>
//List<T> ^ Dataset::GetMinMaxValues()
//{
//  if ( m_pMinMax == nullptr )
//  {
//    m_pMinMax = gcnew List<T>();
//
//    T iMinBalue = System::Convert::ToInt32((T)mpType->GetField("MinValue", System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::Static)->GetValue(nullptr)); 
//    T iMaxValue = System::Convert::ToInt32((T)mpType->GetField("MaxValue", System::Reflection::BindingFlags::Public | System::Reflection::BindingFlags::Static)->GetValue(nullptr)); 
//
//    std::pair<T, T> minmax(iMin,iMax);
//
//    (*m_pDcmImage)->GetMinMaxPixelValues<T>(minmax);
//
//    List<T> ^ mpMinMax =  gcnew List<T>();
//
//    m_pMinMax->Add(minmax.first);
//    m_pMinMax->Add(minmax.second);
//  }
//
//  return m_pMinMax;
//}


#pragma region properties

/**
 * Gets the encapsulated pdf
 */
bool Dataset::IsEncapsulatedPdf::get()
{
  return (*m_pDcmImage)->IsEncapsulatedPdf();
} // IsEncapsulatedPdf::get()


/**
 * Gets the query Retrieval level
 */
String ^ Dataset::QueryRetrievalLevel::get()
{
  return gcnew String((*m_pDcmImage)->GetQueryRetrievalLevel().c_str());
} // QueryRetrievalLevel::get()

/**
 * Sets the query Retrieval level
 */
void Dataset::QueryRetrievalLevel::set(String ^ msValue)
{
  (*m_pDcmImage)->SetQueryRetrievalLevel(ss(msValue));
} // QueryRetrievalLevel::set()


/** 
 * Gets the image date and time
 */
DateTime Dataset::ImageDateTime::get()
{

  return ToDateTime(  gcnew String((*m_pDcmImage)->GetImageDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetImageTime().ToString().c_str()) );

} // ImageDateTime

/** 
 * Gets the acquisition date and time
 */
DateTime Dataset::AcquisitionDateTime::get()
{
  return ToDateTime(  gcnew String((*m_pDcmImage)->GetAcquisitionDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetAcquisitionTime().ToString().c_str()) );

} // AcquisitionDateTime

/** 
 * Gets the series date and time
 */
DateTime Dataset::SeriesDateTime::get()
{
  return ToDateTime(  gcnew String((*m_pDcmImage)->GetSeriesDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetSeriesTime().ToString().c_str()) );

} // SeriesDateTime

/** 
 * Gets the study date and time
 */
DateTime Dataset::StudyDateTime::get()
{
   return ToDateTime(  gcnew String((*m_pDcmImage)->GetStudyDate().ToString().c_str()),
                       gcnew String((*m_pDcmImage)->GetStudyTime().ToString().c_str()) );


} // StudyDateTime



/** 
 * Gets the image plane
 */
ImagePlanes Dataset::ImagePlane::get()
{
  return Utility::GetImagePlane(ImageOrientation);
} // ImagePlane

/**
 * Gets the manufacturer
 */
String ^ Dataset::Manufacturer::get()
{
  return gcnew String((*m_pDcmImage)->GetManufacturer().c_str());
} // Manufacturer::get()

/**
 * Gets the Manufacturer Model
 */
String ^ Dataset::ManufacturerModel::get()
{
  return gcnew String((*m_pDcmImage)->GetManufacturerModel().c_str());
} // ManufacturerModel::get()

/**
 * Gets the Institution name
 */
String ^ Dataset::Institution::get()
{
  return gcnew String((*m_pDcmImage)->GetInstitutionName().c_str());
} // Institution::get()


/**
 * Gets the instance number
 */
unsigned int Dataset::InstanceNumber::get()
{
  return (*m_pDcmImage)->GetInstanceNumber();
} // InstanceNumber::get()

/**
 * Sets the instance number
 */
void Dataset::InstanceNumber::set(unsigned int iValue)
{
  (*m_pDcmImage)->SetInstanceNumber(iValue);
} // InstanceNumber::set()



/**
 * Gets the number of frames
 */
unsigned int Dataset::NumberOfFrames::get()
{
  return (*m_pDcmImage)->GetNumberOfFrames();
} // NumberOfFrames::get()

/**
 * Gets the acquisition number
 */
__int64 Dataset::AcquisitionNumber::get()
{
  return (*m_pDcmImage)->GetAcquisitionNumber();
} // AcquisitionNumber::get()

/**
 * Sets the acquisition number
 */
void Dataset::AcquisitionNumber::set(__int64 iValue)
{
  (*m_pDcmImage)->SetAcquisitionNumber(iValue);
} // AcquisitionNumber::set()

/**
 * Gets the patient name
 */
String ^ Dataset::PatientName::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientName().ToString().c_str());
} // PatientName::get()

/**
 * Sets the patient name
 */
void Dataset::PatientName::set(String ^ msValue)
{
  (*m_pDcmImage)->SetPatientName(ss(msValue));
} // PatientName::set()

/**
 * Gets the patient dob
 */
String ^ Dataset::PatientDob::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientDob().ToString().c_str());
} // PatientDob

/**
 * Gets the series date
 */
String ^ Dataset::AcquisitionDate::get()
{
  return gcnew String((*m_pDcmImage)->GetAcquisitionDate().ToString().c_str());
} // AcquisitionDate

/**
 * Gets the series date
 */
String ^ Dataset::SeriesDate::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesDate().ToString().c_str());
} // SeriesDate

/**
 * Gets the series date
 */
String ^ Dataset::StudyDate::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyDate().ToString().c_str());
} // StudyDate

/**
 * Gets the study description
 */
String ^ Dataset::StudyDescription::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyDescription().c_str());
} // StudyDescription

/**
 * Gets the series description
 */
String ^ Dataset::SeriesDescription::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesDescription().c_str());
} // SeriesDescription

/**
 * Gets the study instance uid
 */
String ^ Dataset::StudyInstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyInstanceUid().c_str());
 } // StudyInstanceUid

/**
 * Sets the study uid
 */
void Dataset::StudyInstanceUid::set(String ^ msValue)
{
  (*m_pDcmImage)->SetStudyInstanceUid(ss(msValue));
} // StudyInstanceUid::set()

/**
 * Gets the series instance Uid
 */
String ^ Dataset::SeriesInstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesInstanceUid().c_str());
} // SeriesInstanceUid


/**
 * Sets the series uid
 */
void Dataset::SeriesInstanceUid::set(String ^ msValue)
{
  (*m_pDcmImage)->SetSeriesInstanceUid(ss(msValue));
} // SeriesInstanceUid::set()

/**
 * Gets the series instance Uid
 */
String ^ Dataset::StudyId::get()
{
  return gcnew String((*m_pDcmImage)->GetStudyId().c_str());
} // StudyId


/**
 * Gets the series number
 */
String ^ Dataset::SeriesNumber::get()
{
  return gcnew String((*m_pDcmImage)->GetSeriesNumber().c_str());
} // SeriesNumber

/**
 * Sets the series number
 */
void Dataset::SeriesNumber::set(String ^ msValue)
{
  (*m_pDcmImage)->SetSeriesNumber(ss(msValue));
} // SeriesNumber


/**
 * Gets the instance uid
 */
String ^ Dataset::InstanceUid::get()
{
  return gcnew String((*m_pDcmImage)->GetInstanceUid().c_str());
} // InstanceUid::get()


/**
 * Gets the instance uid
 */
String ^ Dataset::AffectedSopClassUid::get()
{
  return gcnew String((*m_pDcmImage)->GetAffectedSopClassUid().c_str());
} // AffectedSopClassUid::get()

/**
 * Sets the instance uid
 */
void Dataset::InstanceUid::set(String ^ msValue)
{
  (*m_pDcmImage)->SetInstanceUid(ss(msValue));
} // InstanceUid::set()

/**
 * Gets the rows of the image
 */
unsigned int Dataset::Rows::get()
{
  return (*m_pDcmImage)->GetRows();
} // Rows::get()
  
/**
 * Gets the Columns of the image
 */
unsigned int Dataset::Columns::get()
{
  return (*m_pDcmImage)->GetColumns();
} // Columns::get()

/**
 * Gets the Bits Allocated per pixel
 */
unsigned short Dataset::BitsAllocated::get()
{
  return (*m_pDcmImage)->GetBitsAllocated(); 
} // BitsAllocated::get()

/** 
 * Gets the Rescale Slope
 */
float Dataset::RescaleSlope::get()
{
 return (*m_pDcmImage)->GetRescaleSlope();
} // get_RescaleSlope()

/**
 * Gets the Rescale Intercept
 */
float Dataset::RescaleIntercept::get()
{
  return (*m_pDcmImage)->GetRescaleIntercept();
} // RescaleIntercept::get()


/**
 * Gets the Samples per pixel of the image
 */
unsigned int Dataset::SamplesPerPixel::get()
{
  return (*m_pDcmImage)->GetSamplesPerPixel();
} // SamplesPerPixel::get()

/**
 * Gets the PixelPaddingValue
 */
unsigned int Dataset::PixelPaddingValue::get()
{
  return (*m_pDcmImage)->GetPixelPaddingValue();
} // PixelPaddingValue::get()

/** 
 * Gets the WindowCenter
 */
double Dataset::WindowCenter::get()
{
  double dValue = 0.0;
  if ( ! (*m_pDcmImage)->TryGetValue<double>(DcmTag(0x0028,0x1050), dValue) )
  {
    int iRange = this->PixelValueRange;
    dValue = this->PixelValueRange / 2;
  }

  return dValue;
} // WindowCenter::get()


/**
 * Gets the WindowWidth
 */
double Dataset::WindowWidth::get()
{
  double dValue = 0.0;
  if ( ! (*m_pDcmImage)->TryGetValue<double>(DcmTag(0x0028,0x1051), dValue) )
    dValue = this->PixelValueRange;

  return dValue;
} // WindowWidth::get()

/**
 * Gets the pixel value range
 */
int Dataset::PixelValueRange::get()
{
  Generic::List<short> ^ mpMinMax = GetMinMaxPixelValues();
  return (mpMinMax[1] - mpMinMax[0]);
} // PixelValueRange::get()

/**
 * Gets the image position and returns the x/y/z in a list
 */
System::Collections::Generic::List<double> ^ Dataset::ImagePosition::get()
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
System::Collections::Generic::List<double> ^ Dataset::PixelSpacing::get()
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
System::Collections::Generic::List<double> ^ Dataset::ImageOrientation::get()
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

/*
 * Gets the patient sex from the dicom element list
 *
 * @return  Returns the gender ( M, F or O ).
 */
String ^ Dataset::PatientSex::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientSex().c_str());
} // PatientSex::get()

/*
 * Gets the patient weight from the dicom element list
 *
 * @return  The patient weight
 */
unsigned int Dataset::PatientWeight::get()
{
  return (*m_pDcmImage)->GetPatientWeight();
} // PatientWeight::get()


/*
 * Gets the patient age from the dicom element list with the units indicator as the last character.
 * If no units are supplier the YEAR is assumed.
 *
 * @return  The patient age with teh units indicator as the last character
 */
String ^ Dataset::PatientAge::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientAge().c_str());
} // PatientAge::get()

/** 
 * Gets the patients birth date and time
 */
DateTime Dataset::PatientBirthDateTime::get()
{
  return ToDateTime(  gcnew String((*m_pDcmImage)->GetPatientDob().ToString().c_str()),
                      gcnew String((*m_pDcmImage)->GetPatientTob().ToString().c_str()) );

} // PatientBirthDateTime


/*
 * Gets the additional patient history from the dicom element list.
 *
 * @return  any additinal patient history
 */
String ^ Dataset::AdditionalPatientHistory::get()
{
  return gcnew String((*m_pDcmImage)->GetAdditionalPatientHistory().c_str());
} // AdditionalPatientHistory::get()

/*
 * Gets the operator name
 *
 * @return  The operator name
 */
String ^ Dataset::OperatorName::get()
{
  return gcnew String((*m_pDcmImage)->GetOperatorName().c_str());
} // OperatorName::get()

/*
 * Gets the operator name
 *
 * @return  The operator name
 */
String ^ Dataset::ReferringPhysician::get()
{
  return gcnew String((*m_pDcmImage)->GetReferringPhysician().ToString().c_str());
} // ReferringPhysician::get()

/*
 * Gets the reconstruction diameter
 *
 * @return  The reconstruction diameter
 */
float Dataset::ReconstructionDiameter::get()
{
  return (*m_pDcmImage)->GetReconstructionDiameter();
 } // ReconstructionDiameter::get()



/*
 * Gets the kvp
 *
 * @return  The kvp
 */
float Dataset::Kvp::get()
{
  return (*m_pDcmImage)->GetKvp();
 } // Kvp::get()


/*
 * Gets the xray tube current
 *
 * @return  The xray tube current
 */
float Dataset::XrayTubeCurrent::get()
{
  return (*m_pDcmImage)->GetXrayTubeCurrent();
} // XrayTubeCurrent::get()



/*
 * Gets the patient position
 *
 * @return  The patient position
 */
String ^ Dataset::PatientPosition::get()
{
  return gcnew String((*m_pDcmImage)->GetPatientPosition().c_str());
} // PatientPosition::get()


/*
 * Gets the accession number 
 *
 * @return  The accession number
 */
String ^ Dataset::AccessionNumber::get()
{
  return gcnew String((*m_pDcmImage)->GetAccessionNumber().c_str());
} // AccessionNumber::get()

/*
 * Gets the institution name 
 *
 * @return  The institution name
 */
String ^ Dataset::InstitutionName::get()
{
  return gcnew String((*m_pDcmImage)->GetInstitutionName().c_str());
} // InstitutionName::get()

/*
 * Gets the patient id
 *
 * @return  The patient id
 */
String ^ Dataset::PatientId::get()
{
  return gcnew String((*m_pDcmImage)->GetMedicalId().c_str());
} // PatientId::get()



/*
 * Gets the modality
 *
 * @return  The modality
 */
String ^ Dataset::Modality::get()
{
  return gcnew String((*m_pDcmImage)->GetModality().c_str());
} // Modality::get()

/*
 * Sets the modality
 *
 * @param msValue  New modality
 */
void Dataset::Modality::set(String ^ msValue)
{
  (*m_pDcmImage)->SetModality(ss(msValue));
} // Modality::get()



/*
 * Gets the pixel data
 *
 * @return Buffer containing the pixel data
 */
Viatronix::Dicom::Buffer ^ Dataset::PixelData::get()
{

  std::shared_ptr<dcm::DcmBuffer> pDcmBuffer((*m_pDcmImage)->GetPixelData());

 
  return Buffer::Create(IntPtr((void *)&pDcmBuffer));

} // PixelDtata::get()


/*
 * Gets the pixel representation
 *
 * @return  pixel representation
 */
unsigned short Dataset::PixelRepresentation::get()
{
  return (*m_pDcmImage)->GetPixelRepresentation();
} // PixelRepresentation::get()

/*
 * Sets the pixel representation
 *
 * @param iPixelRepresentation  pixel representation
 * 
 */
void Dataset::PixelRepresentation::set(unsigned short iPixelRepresentation)
{
  (*m_pDcmImage)->SetPixelRepresentation(iPixelRepresentation);
} // PixelRepresentation::set()


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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, int %   value)
{
  int iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned int %   value)
{
  unsigned int iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<unsigned int>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, short %   value)
{
  short iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned short %   value)
{
  unsigned short iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<unsigned short>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, float %  value)
{
  float fValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<float>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, double %   value)
{
  double fValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<double>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), fValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, __int64 %   value)
{
  __int64 iValue;
  bool bFound =  (*m_pDcmImage)->TryGetValue<__int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, unsigned __int64 %  value)
{
  unsigned __int64 iValue;
  bool  bFound =  (*m_pDcmImage)->TryGetValue<unsigned __int64>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), iValue);
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

bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, Viatronix::Dicom::Buffer ^% mpBuffer)
{

  std::shared_ptr<dcm::DcmBuffer> buffer = std::shared_ptr<dcm::DcmBuffer>(new dcm::DcmBuffer());
  bool bFound =  (*m_pDcmImage)->TryGetValue<dcm::DcmBuffer>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), buffer);
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
bool Dataset::TryGetValue(unsigned short iGroup, unsigned short iElement, String ^% msValue)
{

  std::string sValue;

  bool bReturn((*m_pDcmImage)->TryGetValue<std::string>(dcm::DcmUtility::CreateDcmTag(iGroup, iElement), sValue));

  msValue = gcnew String(sValue.c_str());

  return bReturn;

} // TryGetValue(unsigned short iGroup, unsigned short iElement, String ^ value)


#pragma endregion
