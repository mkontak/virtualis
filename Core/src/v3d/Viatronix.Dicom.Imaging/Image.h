// Viatronix.Enterprise.Dicom.Imaging.h

#pragma once



using namespace System;
using namespace System::Collections::Generic;

USING_DICOM_NS

OPEN_DICOM_IMAGING_NS

ref class WindowLevel;
ref class Size;
ref class RenderingParameters;
  
/**
 * Class that implements the rendering of bitmap images
 */
public ref class Image
{

private:

  /// Abrt handler for the thumbnail creation
  static System::Drawing::Image::GetThumbnailImageAbort ^ m_mpImageAbortHandler = nullptr;



public:

  /// Renders the PixelData from the element list into a bitmap ( The default does not use the window leveling, but a scaling factor )
  static System::Drawing::Bitmap ^ RenderPixelDataScaled( Dataset ^ mpDataset, double dZoomFactor );

  /// Renders the PixelData from the element list into a bitmap ( The default does not use the window leveling, but a scaling factor )
  static System::Drawing::Bitmap ^ RenderPixelDataScaled( Dataset ^ mpDataset );

  /// Renders the pixel data to a bitmap using the default window level
  static System::Drawing::Bitmap ^ RenderPixelData( Dataset ^ mpDataset );

  /// Renders the pixel data to a bitmap
  static System::Drawing::Bitmap ^ RenderPixelData( Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel );

  /// Renders the pixel data to a bitmap
  static System::Drawing::Bitmap ^ RenderPixelData( Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel, double dZoomFactor );

  /// Zooms the image supplied
  static System::Drawing::Bitmap ^ Zoom(System::Drawing::Bitmap ^ mpBitmap, double dZoomFactor );

   /// Renders a thumbnail image of the data using the specified size
  static System::Drawing::Image ^ RenderThumbnail( Dataset ^ mpDataset, unsigned int iWdith, unsigned int iHeight );

private:

  /// Handler for the thumbnail image abort
  static bool ThumbnailCallback()
  {  return false; } // 


  /// Renders the pixel data as an unsigned char
  template<class T>
  static System::Drawing::Bitmap ^ RenderPixelData( Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel, T * pPixelData )
  {


    // Get the Rows/Columns of the image
    const unsigned short iRows(mpDataset->Rows);
    const unsigned short iCols(mpDataset->Columns);
    const float fRescaleSlope(mpDataset->RescaleSlope);
    const float fRescaleIntercept(mpDataset->RescaleIntercept);

    // =================================================================================================================
    // Get the pixel padding value from the header and adjust it to the first pixel in the image. This is done 
    // because many time the padding value is not set and the value returned is UInt16::MaxValue which is not correct.
    // The first pixel is in the usually outside the fov so I will use it for  PixelPaddingValue.
    // =================================================================================================================
    int iPixelPaddingValue(mpDataset->PixelPaddingValue);
    iPixelPaddingValue =  (pPixelData[0] != iPixelPaddingValue ? pPixelData[0] : iPixelPaddingValue );

    String ^ sModality(mpDataset->Modality);

    int iWindowMaximum = mpWindowLevel->Center + (mpWindowLevel->Width / 2);
    int iWindowMinimum = iWindowMaximum - mpWindowLevel->Width;

    // Create the RGB value lookup table
    array<unsigned char> ^ lookupTable = CreateLookUpTable<T>(iWindowMinimum, iWindowMaximum);

    // Create the bitmap to hold the new render image
    Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(iCols, iRows, Drawing::Imaging::PixelFormat::Format24bppRgb);

    // Lock to internal bitmap buffer so we can load the data
    Drawing::Imaging::BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,iCols,iRows), Drawing::Imaging::ImageLockMode::ReadOnly, Drawing::Imaging::PixelFormat::Format24bppRgb);

    // Destination pointer
    unsigned char * pPixelBuffer = reinterpret_cast<unsigned char *>(mpBitmapData->Scan0.ToPointer());

 
    /// Pixel size is 3 for RGB
    const int pixelSize = 3;

    //===============================================================
    // Loop through all the pixels and convert them to RGB value.
    //==============================================================
    for (int i(0); i < mpBitmapData->Height; ++i)
    {
      unsigned char * row = pPixelBuffer + (i * mpBitmapData->Stride);

      for (int j(0); j < mpBitmapData->Width; ++j)
      {

        const int colorPos( j * pixelSize );

        unsigned char byte(0);

        T pixel = pPixelData[i * mpBitmapData->Width + j];
      
  
        if ( pixel != iPixelPaddingValue )
        {     
          if ( sModality == "CT"  )
            pixel = (T)(((float)pixel * fRescaleSlope) + fRescaleIntercept);

          byte = lookupTable[ (unsigned short)pixel ];
        }


        row[colorPos]     = byte;          // Red
        row[colorPos + 1] = byte;          // Green
        row[colorPos + 2] = byte;          // Blue

      } // END ... For each column

    } // END ... For each row


    // Unlock the bitmap buffer
    mpBitmap->UnlockBits(mpBitmapData);


    // Returned the rendered bitmap
    return mpBitmap;




  }

  template<class T>
  static System::Drawing::Bitmap ^ RenderPixelData( Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel, unsigned char * pPixelData )
  {
      // Get the Rows/Columns of the image
  const unsigned short iRows(mpDataset->Rows);
  const unsigned short iCols(mpDataset->Columns);
  const unsigned short iSamplesPerPixel(mpDataset->SamplesPerPixel);
  const unsigned short iBitsAllocated(mpDataset->BitsAllocated);

  unsigned char iPixelPaddingValue((unsigned char)mpDataset->PixelPaddingValue);

   // Create the RGB value lookup table
  array<unsigned char> ^ lookupTable = CreateLookUpTable<unsigned char>(0, 255);

  // Create the bitmap to hold the new render image
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(iCols, iRows, Drawing::Imaging::PixelFormat::Format24bppRgb);

  // Lock to internal bitmap buffer so we can load the data
  Drawing::Imaging::BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,iCols,iRows), Drawing::Imaging::ImageLockMode::ReadOnly, Drawing::Imaging::PixelFormat::Format24bppRgb);

  // Destination pointer
  unsigned char * pPixelBuffer = reinterpret_cast<unsigned char *>(mpBitmapData->Scan0.ToPointer());

  // Destination pixel size
  const unsigned short pixelSize = 3;

  // ====================================================================================
  // Get the first pixel value to set an alternator value just in case they do not match
  // ====================================================================================
  register int iPixelPosition(0);

  //===============================================================
  // Loop through all the pixels and convert them to RGB value.
  //==============================================================
  for (int i(0); i < mpBitmapData->Height; ++i)
  {
    unsigned char * row = pPixelBuffer + (i * mpBitmapData->Stride);

    for (int j(0); j < mpBitmapData->Width; ++j)
    {
      const int offset(j * pixelSize);

      for ( int p(pixelSize - 1); p >= 0; --p )
        row[offset + p] = lookupTable[pPixelData[iPixelPosition++]];

    } // END ... For each column

  } // END ... For each row


  // Unlock the bitmap buffer
  mpBitmap->UnlockBits(mpBitmapData);


  // Returned the rendered bitmap
  return mpBitmap;

  }
  



  /// Computes the lookup table
  generic<typename T> 
  static array<unsigned char> ^ CreateLookUpTable(int iWindowMinimum, int iWindowMaximum);

		
};  // class Image

CLOSE_DICOM_IMAGING_NS
