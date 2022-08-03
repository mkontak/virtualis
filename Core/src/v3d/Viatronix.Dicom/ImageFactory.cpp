#include "Stdafx.h"
#include "ImageFactory.h"
#include "IImage.h"
#include "ImageBase.h"
#include "Image.h"
#include "Frame.h"





using namespace System;
using namespace System::IO;
using namespace System::Xml;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Drawing::Imaging;
using namespace System::Collections;

USING_DICOM_NS

/** 
 * Creates the frame from the image pointer
 *
 * @param dcmImagePtr     Unmanaged DcmImage pointer
 *
 * @return Image
 */
IImage ^ ImageFactory::Create(IntPtr dcmImagePtr)
{

  

  std::shared_ptr<dcm::IDcmImage> *ptr = (std::shared_ptr<dcm::IDcmImage> *)(dcmImagePtr.ToPointer());


  IImage ^ mpImage( (*ptr)->IsFrame() ?  (IImage ^)(gcnew Frame()) : (IImage ^)(gcnew Image()) );


  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image object");

  mpImage->SetIDcmImagePtr(dcmImagePtr);

  return mpImage;


} // Create(IntPtr dcmImagePtr)

/** 
 * Creates the element list from the xml node
 *
 * @param msString    String
 *
 * @return Image
 */
IImage ^ ImageFactory::Create(String ^ msParameter, ParameterType type)
{

  /// Create empty object
  Image ^ mpImage(gcnew Image());

  // Make sure memory was allocated
  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image");

  mpImage->SetIDcmImagePtr(System::IntPtr(new std::shared_ptr<dcm::IDcmImage>(std::dynamic_pointer_cast<dcm::IDcmImage>(dcm::DcmImageFactory::Create(ss(msParameter), (dcm::DcmParameterType)type)))));


  return (IImage ^)mpImage;

} // Create(XmlNode ^ mpNode)

/** 
 * Creates the element list from the xml node
 *
 * @param msFilename      File to use for initialization
 * @param msHeader        Header to initialize with
 *
 * @return Image
 */
IImage ^ ImageFactory::Create(String ^ msFilename, String ^ msHeader)
{

  /// Create empty object
  Image ^ mpImage(gcnew Image());

  // Make sure memory was allocated
  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image");

  mpImage->SetIDcmImagePtr(System::IntPtr(new std::shared_ptr<dcm::DcmImage>(dcm::DcmImageFactory::Create(ss(msFilename),ss(msHeader)))));


  return (IImage ^)mpImage;

} // Create(XmlNode ^ mpNode)

/** 
 * Creates the element list from the xml node
 *
 * @param mpNode    Xml representation of the dicom header
 *
 * @return Image
 */
IImage ^ ImageFactory::Create(XmlNode ^ mpNode)
{
  return Create(mpNode->OuterXml, ParameterType::HEADER);
} // Create(XmlNode ^ mpNode)


/** 
 * Creates the element list from the xml node
 *
 * @param msHeader      Header
 * @param mpBitmap      Bitmap
 *
 * @return Image
 */
IImage ^ ImageFactory::Create(String ^ msHeader, System::Drawing::Bitmap ^ mpBitmap)
{

  /// Create empty object
  Image ^ mpImage(gcnew Image());

  // Make sure memory was allocated
  if ( mpImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Image");


   dcm::DcmGdiplusEngine engine;



  unsigned int iWidth(mpBitmap->Width);
  unsigned int iHeight(mpBitmap->Height);

  HBITMAP hBitmap(NULL);


  try
  {




    IntPtr bitmapPtr = mpBitmap->GetHbitmap();
    hBitmap = (HBITMAP)bitmapPtr.ToPointer();
    std::shared_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL)); 
 
    //CLSID  encoderClsid;
    //dcm::DcmGdiplusEngine::GetEncoderClsid(L"image/bmp", &encoderClsid);
    //pBitmap->Save(L"D:\\Temp\\Test2.bmp", &encoderClsid, NULL);

 
    mpImage->SetIDcmImagePtr(System::IntPtr(new std::shared_ptr<dcm::DcmImage>(dcm::DcmImageFactory::Create(ss(msHeader), pBitmap ))));


    pBitmap.reset();

   
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
    if ( hBitmap != NULL )
      DeleteObject(hBitmap);
  }

  return (IImage ^)mpImage;

} // Create(String ^ msHeader, Bitmap ^ mpBitmap)

