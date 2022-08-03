#pragma once

#include "IImage.h"


// namespaces
OPEN_DICOM_NS


public ref class ImageFactory
{

public:

  /// Create the element
  static IImage ^ Create(System::IntPtr dcmImagePtr);

    /// Creates the element list from the file specified
  static IImage ^ Create(String ^ msParameter, ParameterType type);

  /// Creates the element list from the file specified
  static IImage ^ Create(String ^ msFilename, String ^ msHeader);

  /// Creates the element list from the xml node 
  static IImage ^ Create(XmlNode ^ mpNode);
 
  /// Creates the dataset using the seeding header and the bitmap
  static IImage ^ Create(String ^ msHeader, System::Drawing::Bitmap ^ mpBitmap);

};


CLOSE_DICOM_NS