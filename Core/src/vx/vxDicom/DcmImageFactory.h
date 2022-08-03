// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImageFactory.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmImage.h"


namespace dcm
{


class DCM_DLL DcmImageFactory
{

#pragma region Create Static Methods

public:

  /// Creates the DcmImage from the string ( Xml Header, Serialize Header or Dicom File)
  static std::shared_ptr<DcmImage> Create(const std::string & sParameter, const DcmParameterType & type);
  
  /// Creates the DCMImage from the xmlNodePtr
  static std::shared_ptr<DcmImage> Create(const xml::XmlNode & node);
  
  /// Creates the DcmImage from the DcmDataset *
  static std::shared_ptr<DcmImage> Create(DcmDataset * pDcmDataset);

  /// Creates the DcmImage ( the empty=true indicates that no elements are initialize. Otherwise the instance UID and date are set)
  static std::shared_ptr<DcmImage> Create(bool bEmpty = false);

  /// Creates the DcmImage using an image file ( Pdf, Bitmap, Png. Jpeg, ... )
  static std::shared_ptr<DcmImage> Create(const std::string & sHeader, const std::string & sFilename,  const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr) );

  /// Creates the DcmImage using the buffer
  static std::shared_ptr<DcmImage> Create(const std::string & sHeader, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr)  );

  /// Creates the DcmImage using the buffer
  static std::shared_ptr<DcmImage> Create(const std::string & sHeader, const std::shared_ptr<Gdiplus::Bitmap> & pImage, const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr)  );

  /// Creates the DcmImage using the buffer and the element list
  static std::shared_ptr<DcmImage> Create(const std::shared_ptr<DcmElementCollection> & pDcmElementCollection, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr) );


#pragma endregion

#pragma region Initialization Methods

private:

  /// Initializes the image for PDF
  static void InitializePdf(const std::shared_ptr<DcmImage> & pDcmImage);

  /// Initializes the image for a generic image (Used for Bitmap, Jpeg, Png, .. )
  static void InitializeImage(const std::shared_ptr<DcmImage> & pDcmImage);



#pragma endregion

#pragma region Helper Methods

private:


  /// Gets the PDF version from the image 
  static std::string GetPdfVersion(const std::shared_ptr<DcmBuffer> & pDcmBuffer);



#pragma endregion

#pragma region Loading methods


private:

 
  /// Loads and initializes the DcmImage using the Pdf file
  static void LoadPdf(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr) );

  /// Loads and initializes the DcmImage using the Bitmap file
  static void LoadBitmap(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage, const std::shared_ptr<DcmBuffer> & pPayload = std::shared_ptr<DcmBuffer>(__nullptr) );
 
  /// Loads the DcmBuffer's image into the DcmImage
  static void LoadImage(const std::shared_ptr<DcmImage> & pDcmImage, const std::shared_ptr<DcmBuffer> & pImage,  const std::shared_ptr<DcmBuffer> & pPayload  );


#pragma endregion


};  // class DcmImageFactory

};  // namespace dcm