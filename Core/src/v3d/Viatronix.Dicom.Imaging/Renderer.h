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
public ref class Renderer
{

private:

  /// Abrt handler for the thumbnail creation
  static System::Drawing::Image::GetThumbnailImageAbort ^ m_mpImageAbortHandler = nullptr;



public:

  /// Renders the PixelData from the element list into a bitmap ( The default does not use the window leveling, but a scaling factor )
  static System::Drawing::Bitmap ^ RenderPixelDataScaled( IImage ^ mpImage, double dZoomFactor );

  /// Renders the PixelData from the element list into a bitmap ( The default does not use the window leveling, but a scaling factor )
  static System::Drawing::Bitmap ^ RenderPixelDataScaled( IImage ^ mpImage );

  /// Renders the pixel data to a bitmap using the default window level
  static System::Drawing::Bitmap ^ RenderPixelData( IImage ^ mpImage );

  /// Renders the pixel data to a bitmap
  static System::Drawing::Bitmap ^ RenderPixelData( IImage ^ mpImage, WindowLevel ^ mpWindowLevel );

  /// Renders the pixel data to a bitmap
  static System::Drawing::Bitmap ^ RenderPixelData( IImage ^ mpImage, WindowLevel ^ mpWindowLevel, double dZoomFactor );

  /// Zooms the image supplied
  //static System::Drawing::Bitmap ^ Zoom(System::Drawing::Bitmap ^ mpBitmap, double dZoomFactor );

   /// Renders a thumbnail image of the data using the specified size
  static System::Drawing::Image ^ RenderThumbnail( IImage ^ mpImage, unsigned int iWdith, unsigned int iHeight, WindowLevel ^ mpWindowLevel );
  
  /// Renders a thumbnail image of the data using the specified size
  static System::Drawing::Image ^ RenderThumbnail( IImage ^ mpImage, unsigned int iWdith, unsigned int iHeight);

  static System::Windows::Media::Imaging::BitmapSource ^ Renderer::RenderImage(IImage ^ mpImage, WindowLevel ^ mpWindowLevel);

private:

  /// Pdf Image bitmap
  static System::Drawing::Bitmap ^ GeneratePdfBitmap(int iWidth, int iHeight);


  /// No Image bitmap
  static System::Drawing::Bitmap ^ GenerateNoImageBitmap(int iWidth, int iHeight);


  /// Compressed image bitmap
  static System::Drawing::Bitmap ^ GenerateCompressedImageBitmap(int iWidth, int iHeight);


  /// Handler for the thumbnail image abort
  static bool ThumbnailCallback()
  {  return false; } // 


  /// Renders the pixel data as an unsigned char
  template<class T>
  static System::Drawing::Bitmap ^ RenderMonochrome( IImage ^ mpImage, WindowLevel ^ mpWindowLevel, T * pPixelData )
  {


    // Get the Rows/Columns of the image
    const unsigned short iRows(mpImage->Rows);
    const unsigned short iCols(mpImage->Columns);
    const float fRescaleSlope(mpImage->RescaleSlope);
    const float fRescaleIntercept(mpImage->RescaleIntercept);
    

    // =================================================================================================================
    // Get the pixel padding value from the header and adjust it to the first pixel in the image. This is done 
    // because many time the padding value is not set and the value returned is UInt16::MaxValue which is not correct.
    // The first pixel is in the usually outside the fov so I will use it for  PixelPaddingValue.
    // =================================================================================================================
    T iPixelPaddingValue(mpImage->PixelPaddingValue);
    iPixelPaddingValue =  (pPixelData[0] != iPixelPaddingValue ? pPixelData[0] : iPixelPaddingValue );

    String ^ sModality(mpImage->Modality);

    // Create the RGB value lookup table
    //array<unsigned char> ^ lookupTable = CreateLookUpTable<T>(mpWindowLevel->Width, mpWindowLevel->Center );

    //Lut ^ redLut = Lut::Create();
    //TryGetLut(mpImage, 0x0028, 0x1101, redLut);

    //Lut ^ greenLut = Lut::Create();
    //TryGetLut(mpImage, 0x0028, 0x1102, greenLut);

    //Lut ^ blueLut = Lut::Create();
    //TryGetLut(mpImage, 0x0028, 0x1103, blueLut);

    //Lut ^ alphaLut = Lut::Create();
    //TryGetLut(mpImage, 0x0028, 0x1104, alphaLut);



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
        T voxel = pixel;
  
        if ( pixel != iPixelPaddingValue )
        {     
          //if ( sModality == "CT"  )
          pixel = (T)(((float)pixel * fRescaleSlope) + fRescaleIntercept);

          byte =  RenderPixelByte( pixel, mpWindowLevel );
        }


        row[colorPos]     = byte;          // Red
        row[colorPos + 1] = byte;        // Green
        row[colorPos + 2] = byte;         // Blue

        //row[colorPos]     = redLut->GetLutValue(voxel, byte);          // Red
        //row[colorPos + 1] = greenLut->GetLutValue(voxel, byte);        // Green
        //row[colorPos + 2] = blueLut->GetLutValue(voxel, byte);         // Blue

      } // END ... For each column

    } // END ... For each row


    // Unlock the bitmap buffer
    mpBitmap->UnlockBits(mpBitmapData);


    return mpBitmap;




  }



  //static unsigned char RenderPixel(unsigned char pixel, WindowLevel ^ mpWindowLevel);
  //static unsigned short RenderPixel(unsigned short pixel, WindowLevel ^ mpWindowLevel);


  static unsigned char RenderPixelByte(short pixel, WindowLevel ^ mpWindowLevel);


  static short RenderPixelShort(short pixel, WindowLevel ^ mpWindowLevel);

  static System::Drawing::Bitmap ^ RenderRgb( IImage ^ mpImage, WindowLevel ^ mpWindowLevel, unsigned char * pPixelData );
  

  /// Computes the lookup table
  generic<typename T> 
  static array<unsigned short> ^ CreateLookUpTable2(int iWidth, int iCenter );


  /// Computes the lookup table
  generic<typename T> 
  static array<unsigned char> ^ CreateLookUpTable(int iWidth, int iCenter );

  /// Try to get the LUT
  static bool TryGetLut(IImage ^ mpImage, unsigned short iGroup, unsigned short iElement, Lut ^% mpLut);

		
};  // class Image

CLOSE_DICOM_IMAGING_NS
