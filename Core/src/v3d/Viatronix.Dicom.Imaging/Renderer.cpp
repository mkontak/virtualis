// This is the main DLL file.

#include "stdafx.h"
#include "Renderer.h"
#include "WindowLevel.h"
#include "Lut.h"

#include <limits.h>

using namespace System;
using namespace System::Drawing::Drawing2D;

USING_DICOM_IMAGING_NS


/**
 * Generates a NO IMAGE bitmap for display when we have DICOM information but no Pixel Data
 *
 * @param iWdith      Width of image
 * @param iHeight     Height of image 
 *
 * @return No Image Bitmap
 */
Drawing::Bitmap ^ Renderer::GenerateNoImageBitmap(int iWidth, int iHeight)
{
  Drawing::StringFormat ^ mpFormat = gcnew Drawing::StringFormat();
  mpFormat->Alignment = Drawing::StringAlignment::Center;
  mpFormat->LineAlignment = System::Drawing::StringAlignment::Center;
    
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(iWidth, iHeight, Drawing::Imaging::PixelFormat::Format24bppRgb);
  Drawing::Graphics ^ mpGraphics = Drawing::Graphics::FromImage((Drawing::Image ^)mpBitmap);
  mpGraphics->DrawString("No Image", gcnew Drawing::Font("Tahoma",12),  Drawing::Brushes::DarkRed, (iWidth / 2.0F), (iHeight / 2.0F), mpFormat );
  mpGraphics->DrawLine(Drawing::Pens::DarkRed, Drawing::PointF(0,0), Drawing::PointF((float)iWidth,(float)iHeight));
  mpGraphics->DrawLine(Drawing::Pens::DarkRed, Drawing::PointF(0,(float)iHeight), Drawing::PointF((float)iWidth,0));
  mpGraphics->DrawRectangle(Drawing::Pens::DarkRed, Drawing::Rectangle(1, 1, (iWidth - 2), (iHeight - 2)));

  return mpBitmap;

} // GenerateNoImageBitmap()

/**
 * Generates a COMPRESSED IMAGE bitmap for display when we have DICOM information and the pixel data is compressed
 *
 * @param iWdith      Width of image
 * @param iHeight     Height of image 
 *

 * @return Compressed Image Bitmap
 */
Drawing::Bitmap ^ Renderer::GenerateCompressedImageBitmap(int iWidth, int iHeight)
{
  Drawing::StringFormat ^ mpFormat = gcnew Drawing::StringFormat();
  mpFormat->Alignment = Drawing::StringAlignment::Center;
  mpFormat->LineAlignment = System::Drawing::StringAlignment::Center;
    
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(iWidth, iHeight, Drawing::Imaging::PixelFormat::Format24bppRgb);
  Drawing::Graphics ^ mpGraphics = Drawing::Graphics::FromImage((Drawing::Image ^)mpBitmap);
  mpGraphics->DrawString("Image is compressed", gcnew Drawing::Font("Tahoma",12),  Drawing::Brushes::DarkRed, (iWidth / 2.0F), (iHeight / 2.0F), mpFormat );
  mpGraphics->DrawLine(Drawing::Pens::Green, Drawing::PointF(0,0), Drawing::PointF((float)iWidth,(float)iHeight));
  mpGraphics->DrawLine(Drawing::Pens::Green, Drawing::PointF(0,(float)iHeight), Drawing::PointF((float)iWidth,0));
  mpGraphics->DrawRectangle(Drawing::Pens::Green, Drawing::Rectangle(1, 1, (iWidth - 2), (iHeight - 2)));

  return mpBitmap;
} // GenerateCompressedImageBitmap()


/**
 * Generates a PDF IMAGE bitmap for display when we have DICOM information and the pixel data is compressed
 *
 * @param iWdith      Width of image
 * @param iHeight     Height of image 
 *

 * @return Compressed Image Bitmap
 */
Drawing::Bitmap ^ Renderer::GeneratePdfBitmap(int iWidth, int iHeight)
{
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(System::Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream( "pdf.png" ));

  return mpBitmap;

} // GeneratePdfBitmap;


/** 
 * Renders the pixel data into a bitmap
 *
 */
Drawing::Bitmap ^ Renderer::RenderPixelData(IImage ^ mpImage )
{
  return RenderPixelData(mpImage, gcnew WindowLevel((int)mpImage->WindowCenter, (int)mpImage->WindowWidth), 1.0);
} // RenderPixelData(IImage ^ mpImage)

/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpImage       Element list to render pixel data from
 * @param mpWindow        Window level
 *
 * @return Rendered bitmap image
 */
Drawing::Bitmap ^ Renderer::RenderPixelData(IImage ^ mpImage, WindowLevel ^ mpWindowLevel )
{
  if ( mpWindowLevel == nullptr )
    return RenderPixelData(mpImage);
  else
   return RenderPixelData(mpImage, mpWindowLevel, 1.0);
} // RenderPixelData(IImage ^ mpImage, WindowLevel ^ mpWindowLevel )

/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpImage       Element list to render pixel data from
 * @param mpWindow        Window level
 * @param dZoomFactor     Zoom factor
 *
 * @return Rendered bitmap image
 */
Drawing::Bitmap ^ Renderer::RenderPixelData(IImage ^ mpImage, WindowLevel ^ mpWindowLevel, double dZoomFactor )
{

  Viatronix::Dicom::Buffer ^ mpBuffer(mpImage->PixelData);

  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);

  // =============================================================================================================
  // If no pixel data exists then return an image indicating no image exists. The zoom factor will be ignored.
  // =============================================================================================================
  if ( mpBuffer->Size == 0 )
  {

    if ( mpImage->IsEncapsulatedPdf )
      return GeneratePdfBitmap(iWidth, iHeight);
    else
      return GenerateNoImageBitmap(iWidth, iHeight);

  } // END ... If there is no pixel data

  // ==========================================================================================================================
  // Else if thers is pixel data and the  the buffer is less then the image size is suppose to be then the image is compressed
  // ==========================================================================================================================
  else if ( mpBuffer->Size < mpImage->GetImageSize() )
     return GenerateCompressedImageBitmap(iWidth, iHeight);


  // Get the bits allocated to determine the render method to call
  const unsigned short iBitsAllocated(mpImage->BitsAllocated);

  // =====================================
  // Call the appropriate rendering method
  // ======================================
  if ( mpImage->IsMonochrome )
  {
    if ( iBitsAllocated == 8 )
      return RenderMonochrome<unsigned char>(mpImage, mpWindowLevel, reinterpret_cast<unsigned char *>(mpBuffer->GetBufferPtr().ToPointer()));
    else
      return RenderMonochrome<short>(mpImage, mpWindowLevel, reinterpret_cast<short *>(mpBuffer->GetBufferPtr().ToPointer()));
  }
  else
    return RenderRgb(mpImage, mpWindowLevel, reinterpret_cast<unsigned char *>(mpBuffer->GetBufferPtr().ToPointer()));
 


} //  RenderPixelData(IImage ^ mpImage, Size ^ mpSize, WindowLevel ^ mpWindow )



/**
 * Renders a thumbnail image for the specified dataset for the specified size
 *
 * @param mpImage     Dicom data
 * @param iWidth        Thumbnail Width
 * @param iHeight       Thumbnail Height
 *
 * @return Thumbnail
 */
Drawing::Image ^ Renderer::RenderThumbnail(IImage ^ mpImage, unsigned int iWidth, unsigned int iHeight)
{
  Drawing::Bitmap ^ mpBitmap = RenderPixelData(mpImage);

  System::Drawing::Image::GetThumbnailImageAbort ^ mpCallback = gcnew System::Drawing::Image::GetThumbnailImageAbort( ThumbnailCallback );


  return mpBitmap->GetThumbnailImage((int)iWidth, (int)iHeight, mpCallback, System::IntPtr::Zero);

} // RenderThumbnail(IImage ^ mpImage, unsigned int iWdith, unsigned int iHeight)


/**
 * Renders a thumbnail image for the specified dataset for the specified size
 *
 * @param mpImage     Dicom data
 * @param iWidth        Thumbnail Width
 * @param iHeight       Thumbnail Height
 *
 * @return Thumbnail
 */
Drawing::Image ^ Renderer::RenderThumbnail(IImage ^ mpImage, unsigned int iWidth, unsigned int iHeight, WindowLevel ^ mpWindowLevel)
{

  
  Drawing::Bitmap ^ mpBitmap = ( mpWindowLevel == nullptr ? RenderPixelData(mpImage) : RenderPixelData(mpImage, mpWindowLevel) );

  System::Drawing::Image::GetThumbnailImageAbort ^ mpCallback = gcnew System::Drawing::Image::GetThumbnailImageAbort( ThumbnailCallback );


  return mpBitmap->GetThumbnailImage((int)iWidth, (int)iHeight, mpCallback, System::IntPtr::Zero);

} // RenderThumbnail(IImage ^ mpImage, unsigned int iWdith, unsigned int iHeight)



/** 
 * Renders the pixel data into a bitmap image used a scaling factor calculated by using 
 * the min/max pixel value in the image.
 *
 * @param mpImage       Element list to render
 *
 * @return  Bitmap representation of the dicom image
 */
System::Drawing::Bitmap ^ Renderer::RenderPixelDataScaled(IImage ^ mpImage)
{
  return RenderPixelDataScaled(mpImage, 1.0);
} // RenderPixelDataScaled(IImage ^ mpImage)

/** 
 * Renders the pixel data into a bitmap image used a scaling factor calculated by using 
 * the min/max pixel value in the image.
 *
 * @param mpImage       Element list to render
 *
 * @return  Bitmap representation of the dicom image
 */
System::Drawing::Bitmap ^ Renderer::RenderPixelDataScaled(IImage ^ mpImage, double dZoomFactor)
{

  Viatronix::Dicom::Buffer ^ mpBuffer(mpImage->PixelData);
  
  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);

 // =============================================================================================================
  // If no pixel data exists then return an image indicating no image exists. The zoom factor will be ignored.
  // =============================================================================================================
  if ( mpBuffer->Size == 0 )
  {

    if ( mpImage->IsEncapsulatedPdf )
      return GeneratePdfBitmap(iWidth, iHeight);
    else
      return GenerateNoImageBitmap(iWidth, iHeight);

  } // END ... If there is no pixel data

  // ==========================================================================================================================
  // Else if thers is pixel data and the  the buffer is less then the image size is suppose to be then the image is compressed
  // ==========================================================================================================================
  else if ( mpBuffer->Size < mpImage->GetImageSize() )
     return GenerateCompressedImageBitmap(iWidth, iHeight);


  short  * pPixelData(reinterpret_cast<short  *>(mpBuffer->GetBufferPtr().ToPointer()));

  const int iTypeSize = sizeof(unsigned short);
  const int iMaxValue = (int)Math::Pow(2.0,(double)iTypeSize * 8.0);

  const unsigned short iRows(mpImage->Rows);
  const unsigned short iCols(mpImage->Columns);
  const unsigned short iSamplesPerPixel(mpImage->SamplesPerPixel);
  int iPixelPaddingValue(mpImage->PixelPaddingValue);
  iPixelPaddingValue =  (pPixelData[0] != iPixelPaddingValue ? pPixelData[0] : iPixelPaddingValue );
  const int iPixelPaddingMaximum = ( iPixelPaddingValue == 0 ? iMaxValue : (iPixelPaddingValue - iTypeSize) );

  short * pImage = reinterpret_cast<short *>(pPixelData);

  int iMinPixelValue = Int16::MaxValue; 
  int iMaxPixelValue = 0; 

  short iMin = pImage[0];
  short iMax = pImage[0];


  const unsigned long iImageSize = ( iRows * iCols ) * iSamplesPerPixel;


  for ( register unsigned long i(1); i < iImageSize; ++i )
  {
  
    if ( iMin > pImage[i] )
    {
      iMin = pImage[i];
    }
    else if ( iMax < pImage[i] )
    {
      iMax = pImage[i];
    }

    if ( pPixelData[i] < iPixelPaddingMaximum )
    {
      if ( iMinPixelValue > pPixelData[i] )
      {
        iMinPixelValue = pPixelData[i];
      }
      else if ( iMaxPixelValue < pPixelData[i] )
      {
        iMaxPixelValue = pPixelData[i];
      }
    }
  } // END ... For each pixel value

  short iBias(iMinPixelValue * -1);
  iMaxPixelValue = ( iMaxPixelValue + iBias );

  float fScale( iMin < (short)Byte::MaxValue ? 1.0F : ((float)Byte::MaxValue / (float)iMax));


  System::Drawing::Bitmap ^ mpBitmap = gcnew System::Drawing::Bitmap(iCols, iRows, System::Drawing::Imaging::PixelFormat::Format32bppArgb);

  System::Drawing::Imaging::BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,iCols,iRows), System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format32bppArgb);


  int * pData = reinterpret_cast<int *>(mpBitmapData->Scan0.ToPointer());

  for ( register unsigned int  i(0);  i < iImageSize; ++i )
  {

    if ( pPixelData[i] >= iPixelPaddingMaximum )
    {
      pData[i] = System::Drawing::Color::Black.ToArgb();
    }
    else
    {
      const short scaledPixelValue( ( short)((float)pPixelData[i] * fScale ) );
      const short color(BOUND(( short )System::Byte::MinValue, scaledPixelValue, ( short )System::Byte::MaxValue));
      pData[i] = System::Drawing::Color::FromArgb(color, color, color).ToArgb();
    }

  }

  mpBitmap->UnlockBits(mpBitmapData);

  return mpBitmap;

} // RenderPixelDataScaled(IImage ^ mpImage)



/** 
 * Computes the lookup table
 *
 * @param iWindowMaximum      Window maximum
 * @param iWindowMinimum      Window minimum
 * @param iPixelPaddingValue  Pixel padding value
 *
 * @return Lookup table
 */
generic<typename T>
array<unsigned char> ^ Renderer::CreateLookUpTable(int iWidth, int iCenter )
{

  const int typeSize = sizeof(T);

  
  const int lutSize = (int)Math::Pow(2.0,(double)typeSize * 8.0);
  const int maxIntensity = lutSize - 1;

  
  /// Allocate look table
  array<unsigned char> ^ lut = gcnew array<unsigned char>(lutSize);

  const int iAdjustedWidth(iWidth -1 );
  const float fHalfWidth( (float)iAdjustedWidth/2.0F );
  const float fAdjustedCenter( (float)iCenter - 0.5F );
  const int iWindowMinimum( (int)(fAdjustedCenter - fHalfWidth) );
  const int iWindowsMaximum( (int)(fAdjustedCenter + fHalfWidth) );

 

  System::Type ^ mpType = T::typeid;

  const bool bUnsigned = (mpType->FullName->StartsWith("System.U") || mpType->FullName == "System.Byte" );
  const int iMinValue( bUnsigned ? 0 : -(lutSize/2) );
  const int iMaxValue( bUnsigned ? lutSize - 1 : (int)Math::Abs(iMinValue) - 1); 

  // ================================================
  // Loop thourgh all possible pixel values
  // ================================================
  for ( register int i(iMinValue); i <= iMaxValue; i++ )
  {

    unsigned short index((unsigned short)i);
    if ( i < iWindowMinimum ) lut[index] = 0;
    else if ( i > iWindowsMaximum ) lut[index] = 255;
    else lut[index] = (unsigned char)(((i - fAdjustedCenter)/iAdjustedWidth + 0.5F) * 255);

  }

  return lut;

} // CreateLookUpTable(int iWidth, int iCenter )

/** 
 * Computes the lookup table
 *
 * @param iWindowMaximum      Window maximum
 * @param iWindowMinimum      Window minimum
 * @param iPixelPaddingValue  Pixel padding value
 *
 * @return Lookup table
 */
generic<typename T>
array<unsigned short> ^ Renderer::CreateLookUpTable2(int iWidth, int iCenter )
{

  const int typeSize = sizeof(T);

  
  const int lutSize = (int)Math::Pow(2.0,(double)typeSize * 8.0);


  
  /// Allocate look table
  array<unsigned short> ^ lut = gcnew array<unsigned short>(lutSize);

  const int iAdjustedWidth(iWidth -1 );
  const float fHalfWidth( (float)iAdjustedWidth/2.0F );
  const float fAdjustedCenter( (float)iCenter - 0.5F );
  const int iWindowMinimum( (int)(fAdjustedCenter - fHalfWidth) );
  const int iWindowsMaximum( (int)(fAdjustedCenter + fHalfWidth) );

 

  System::Type ^ mpType = T::typeid;

  const bool bUnsigned = (mpType->FullName->StartsWith("System.U") || mpType->FullName == "System.Byte" );
  const int iMinValue( bUnsigned ? 0 : -(lutSize/2) );
  const int iMaxValue( bUnsigned ? lutSize - 1 : (int)Math::Abs(iMinValue) - 1); 

  // ================================================
  // Loop thourgh all possible pixel values
  // ================================================
  for ( register int i(iMinValue); i <= iMaxValue; i++ )
  {

    unsigned short index((unsigned short)i);
    if ( i < iWindowMinimum ) lut[index] = 0;
    else if ( i > iWindowsMaximum ) lut[index] = 65535 ;
    else lut[index] = (unsigned short)(((i - fAdjustedCenter)/iAdjustedWidth + 0.5F) * 65535 );

  }

  return lut;

} // CreateLookUpTable(int iWidth, int iCenter )




System::Drawing::Bitmap ^ Renderer::RenderRgb( IImage ^ mpImage, WindowLevel ^ mpWindowLevel, unsigned char * pPixelData )
{
      // Get the Rows/Columns of the image
  const unsigned short iRows(mpImage->Rows);
  const unsigned short iCols(mpImage->Columns);
  const unsigned short iSamplesPerPixel(mpImage->SamplesPerPixel);
  const unsigned short iBitsAllocated(mpImage->BitsAllocated);

  unsigned char iPixelPaddingValue((unsigned char)mpImage->PixelPaddingValue);

   // Create the RGB value lookup table
  array<unsigned char> ^ lookupTable = CreateLookUpTable<unsigned char>((int)mpWindowLevel->Width, (int)mpWindowLevel->Center);


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




bool Renderer::TryGetLut(IImage ^ mpImage, unsigned short iGroup, unsigned short iElement, Lut ^% mpLut)
{

  bool bFound = false;

  String ^ mpDescriptor = String::Empty;

  if ( mpImage->TryGetValue(iGroup,iElement, mpDescriptor, true) )
  {
    Buffer ^ mpBuffer = gcnew Buffer();
    if ( mpImage->TryGetValue(iGroup,(iElement + 0x0100), mpBuffer, true) )
    {
      mpLut = Lut::Create(mpDescriptor, mpBuffer);
      bFound = true;
    }
  }

  return bFound;

}     





short Renderer::RenderPixelShort(short pixel, WindowLevel ^ mpWindowLevel)
{
  const double dAdjustedWidth(mpWindowLevel->Width - 1 );
  const double dHalfWidth( dAdjustedWidth/2.0F );
  const double dAdjustedCenter(mpWindowLevel->Center - 0.5F );
  const int iWindowMinimum( (int)(dAdjustedCenter - dHalfWidth) );
  const int iWindowsMaximum( (int)(dAdjustedCenter + dHalfWidth) );


  const short max(Int16::MaxValue);
  const short min(Int16::MinValue);
  

  if ( pixel <= iWindowMinimum )
    return min;
  else if ( pixel > iWindowsMaximum )
    return max;
  else
    return (short)(((pixel - dAdjustedCenter)/dAdjustedWidth + 0.5F) * (max - min) + min);
}


unsigned char Renderer::RenderPixelByte(short pixel, WindowLevel ^ mpWindowLevel)
{
  const double dAdjustedWidth(mpWindowLevel->Width - 1 );
  const double dHalfWidth( dAdjustedWidth/2.0F );
  const double dAdjustedCenter(mpWindowLevel->Center - 0.5F );
  const int iWindowMinimum( (int)(dAdjustedCenter - dHalfWidth) );
  const int iWindowsMaximum( (int)(dAdjustedCenter + dHalfWidth) );


  const unsigned char max(Byte::MaxValue);
  const unsigned char min(Byte::MinValue);
  

  if ( pixel <= iWindowMinimum )
    return min;
  else if ( pixel > iWindowsMaximum )
    return max;
  else
    return (unsigned char)(((pixel - dAdjustedCenter)/dAdjustedWidth + 0.5F) * (max - min) + min);
}



/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpImage       Element list to render pixel data from
 * @param mpWindow        Window level
 * @param dZoomFactor     Zoom factor
 *
 * @return Rendered bitmap image
 */
System::Windows::Media::Imaging::BitmapSource ^ Renderer::RenderImage(IImage ^ mpImage, WindowLevel ^ mpWindowLevel)
{

  Viatronix::Dicom::Buffer ^ mpBuffer(mpImage->PixelData);

  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);


  // Get the bits allocated to determine the render method to call
  const unsigned short iBitsAllocated(mpImage->BitsAllocated);


  System::Windows::Media::Imaging::BitmapSource ^ mpSource;


  const int iBytesPerPixel( iBitsAllocated / 8 );
  const int iStride(iWidth * iBytesPerPixel);

  //array<short> ^ lut = CreateLookUpTable<short>(40,400);

  array<System::Byte> ^ mpArry = mpBuffer->GetByteData();

  try
  {

    array<System::Byte> ^ buffer = gcnew array<System::Byte>(mpBuffer->Size);


    void * ptr = reinterpret_cast<void *>(mpBuffer->GetBufferPtr().ToPointer());

    int size(mpBuffer->Size/2);


    short * psource = (short *)ptr;
    unsigned char * mapped = new unsigned char [mpBuffer->Size];
    short * pdest = (short *)mapped;


 
    for ( int i(0); i < size; i++ )
      pdest[i] = psource[i]; //RenderPixelShort(psource[i], mpWindowLevel);

    mpSource = System::Windows::Media::Imaging::BitmapSource::Create(iWidth, iHeight, 96, 96, System::Windows::Media::PixelFormats::Gray16, nullptr, buffer, iStride);

    return mpSource;

  }
  catch (Exception ^ )
  {
    return nullptr;
  }

  

  return nullptr;

} //  RenderPixelData(Image ^ mpImage, Size ^ mpSize, WindowLevel ^ mpWindow )

