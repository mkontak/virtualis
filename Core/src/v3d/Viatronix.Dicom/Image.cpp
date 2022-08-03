#include "Stdafx.h"
#include "Utility.h"
#include "Image.h"
#include "Element.h"
#include "Buffer.h"
#include "Sequence.h"
#include "ElementCollection.h"
#include "Element.h"
#include "ImageCollection.h"


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
Image::Image() : 
ImageBase()
{
  m_pDcmImage.reset(new std::shared_ptr<dcm::IDcmImage>(dcm::DcmImageFactory::Create()));
} // Image()


/**
 * Destructor
 */
Image::~Image()
{
  this->!Image();
} // ~Image()
  
/**
 * Finalizer
 */
Image::!Image()
{
  
} // !Image


/**
 * Clones the dataset
 */
IImage ^ Image::Clone()
{

  Image ^ mpImage(gcnew Image());

  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image for clone");

  mpImage->SetIDcmImagePtr(System::IntPtr(new std::shared_ptr<dcm::IDcmImage>((*m_pDcmImage)->Clone())));


  return mpImage;

} // Clone()




/**
 * Clears the dicom header
 */
void Image::Clear()
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->Clear();
} // Clear()



/**
 * Saves the dicom to the file/path specified
 *
 * @param sFilePath
 */
void Image::Save(String ^ msFilePath)
{

   String ^ msFile(msFilePath);

  // =============================================================================
  // If the msFilePath is a directory then name the file using the instance uid
  // =============================================================================
  if ( System::IO::Directory::Exists(msFilePath) )
    msFile = System::IO::Path::Combine(msFilePath, InstanceUid->Replace(".","_") + ".dcm");

  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->Save(ss(msFile));

} // Save(String ^ sFilePath)


/**
 * Sets teh pixel data using the bitmap supplied
 *
 * @param mpBitmap
 */
void Image::SetPixelData(System::Drawing::Bitmap ^ mpBitmap)
{

 unsigned int iWidth(mpBitmap->Width);
  unsigned int iHeight(mpBitmap->Height);

  HBITMAP hBitmap(NULL);



  try
  {
    IntPtr bitmapPtr = mpBitmap->GetHbitmap();
    hBitmap = (HBITMAP)bitmapPtr.ToPointer();
    std::shared_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL));

    std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetPixelData(pBitmap);

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



} // SetPixelData(SetPixelData(System::Drawing::Bitmap ^ mpBitmap)


/**
 * Adds or updates the specified element
 *
 * @param mpElement     Element to be added or updated
 */
void Image::Insert(IElement ^ mpElement)
{

  std::shared_ptr<dcm::DcmImage> image(std::dynamic_pointer_cast<dcm::DcmImage>(*m_pDcmImage));
  if ( mpElement->IsSequence )
  {
    image->Insert(*((std::shared_ptr<dcm::DcmElement> *)(((Sequence ^)mpElement)->GetDcmElementPtr().ToPointer())) );
  }
  else
  {
    image->Insert(*((std::shared_ptr<dcm::DcmElement> *)(((Element ^)mpElement)->GetDcmElementPtr().ToPointer())) );
  }

} // Insert(IElement ^ mpElement))


#pragma region properties


/**
 * Gets the query Retrieval level
 */
String ^ Image::QueryRetrievalLevel::get()
{
  return gcnew String(std::dynamic_pointer_cast<dcm::DcmImage>(std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage)))->GetQueryRetrievalLevel().c_str());
} // QueryRetrievalLevel::get()

/**
 * Sets the query Retrieval level
 */
void Image::QueryRetrievalLevel::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>(std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage)))->SetQueryRetrievalLevel(ss(msValue));
} // QueryRetrievalLevel::set()


/**
 * Sets the instance number
 */
void Image::InstanceNumber::set(unsigned int iValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetInstanceNumber(iValue);
} // InstanceNumber::set()



/**
 * Sets the acquisition number
 */
void Image::AcquisitionNumber::set(__int64 iValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetAcquisitionNumber(iValue);
} // AcquisitionNumber::set()


/**
 * Sets the patient name
 */
void Image::PatientName::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetPatientName(ss(msValue));
} // PatientName::set()

/**
 * Sets the study uid
 */
void Image::StudyInstanceUid::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetStudyInstanceUid(ss(msValue));
} // StudyInstanceUid::set()

/**
 * Sets the series uid
 */
void Image::SeriesInstanceUid::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetSeriesInstanceUid(ss(msValue));
} // SeriesInstanceUid::set()

/**
 * Sets the series number
 */
void Image::SeriesNumber::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetSeriesNumber(ss(msValue));
} // SeriesNumber

/**
 * Sets the instance uid
 */
void Image::InstanceUid::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetInstanceUid(ss(msValue));
} // InstanceUid::set()





/*
 * Sets the modality
 *
 * @param msValue  New modality
 */
void Image::Modality::set(String ^ msValue)
{
  std::dynamic_pointer_cast<dcm::DcmImage>((*m_pDcmImage))->SetModality(ss(msValue));
} // Modality::get()




#pragma endregion


