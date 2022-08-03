// Viatronix.Enterprise.Dicom.Imaging.h

#pragma once



using namespace System;
using namespace System::Collections::Generic;

USING_DICOM_NS

OPEN_DICOM_IMAGING_NS

ref class Lut;
ref class WindowLevel;
ref class RenderingParameters;
  
/**
 * Class that implements the rendering of bitmap images
 */
public ref class DicomImage
{

private:

  /// Abrt handler for the thumbnail creation
  static System::Drawing::Image::GetThumbnailImageAbort ^ m_mpImageAbortHandler = nullptr;



public:

  static DicomImage ^ Create(IImage ^ mpImage);



  /// Renders the pixel data to a bitmap using the default window level
  inline System::Drawing::Bitmap ^ Render()
  { return Render(m_mpWindowLevel); }

  /// Renders the pixel data to a bitmap
  inline System::Drawing::Bitmap ^ Render( WindowLevel ^ mpWindowLevel )
  { return Render(( mpWindowLevel == nullptr ? m_mpWindowLevel : mpWindowLevel), 1.0F); }

  /// Renders the pixel data to a bitmap
  System::Drawing::Bitmap ^ Render( WindowLevel ^ mpWindowLevel, float dZoomFactor );

  /// Renders a thumbnail image of the data using the specified size
  System::Drawing::Image ^ RenderThumbnail( unsigned int iWidth, unsigned int iHeight )
  { return RenderThumbnail(iWidth, iHeight, m_mpWindowLevel); }

  /// Renders a thumbnail image of the data using the specified size
  System::Drawing::Image ^ RenderThumbnail( unsigned int iWidth, unsigned int iHeight, WindowLevel ^ mpWindowLevel );

  System::Windows::Media::Imaging::BitmapSource ^ RenderImage( WindowLevel ^ mpWindowLevel);

private:\

  /// Pdf Image bitmap
  static System::Drawing::Bitmap ^ GeneratePdfBitmap(int iWidth, int iHeight);


  /// No Image bitmap
  static System::Drawing::Bitmap ^ GenerateNoImageBitmap(int iWidth, int iHeight);


  /// Compressed image bitmap
  static System::Drawing::Bitmap ^ GenerateCompressedImageBitmap(int iWidth, int iHeight);


  /// Handler for the thumbnail image abort
  static bool ThumbnailCallback()
  {  return false; } // 


  template<typename T> 
  System::Drawing::Bitmap ^ RenderMonochrome( array<T> ^ pPixelData, WindowLevel ^ mpWindowLevel );

  unsigned char RenderPixelByte(short pixel, WindowLevel ^ mpWindowLevel);

  short RenderPixelShort(short pixel, WindowLevel ^ mpWindowLevel);

  System::Drawing::Bitmap ^ RenderRgb(  array<unsigned char> ^ pPixelData, WindowLevel ^ mpWindowLevel );
  

  /// Computes the lookup table
  generic<typename T> 
  array<unsigned short> ^ CreateLookUpTable2(double dWidth, double dCenter );


  /// Computes the lookup table
  generic<typename T> 
  array<unsigned char> ^ CreateLookUpTable(double dWidth, double dCenter );

  /// Try to get the LUT
  bool TryGetLut(IImage ^ mpImage, unsigned short iGroup, unsigned short iElement, Lut ^% mpLut);

private:

  IImage ^ m_mpImage;

  UInt32 m_iRows;

  UInt32 m_iColumns;

  Single m_fRescaleSlope;

  Single m_fRescaleIntercept;

  WindowLevel ^ m_mpWindowLevel;

  Int32 m_iPixelPaddingValue;

  bool m_bIsEncapsulatedPdf;


  bool m_bIsMonochrome;

  UInt16 m_iBitsAllocated;

  Viatronix::Dicom::Buffer ^ m_mpPixelData;
  
};  // class Image

CLOSE_DICOM_IMAGING_NS
