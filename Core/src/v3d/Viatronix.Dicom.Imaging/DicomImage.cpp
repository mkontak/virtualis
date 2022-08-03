// This is the main DLL file.

#include "stdafx.h"
#include "DicomImage.h"
#include "WindowLevel.h"
#include "Lut.h"

#include <limits.h>

using namespace System;
using namespace System::Drawing::Drawing2D;

USING_DICOM_IMAGING_NS



DicomImage ^ DicomImage::Create(IImage ^ mpImage)
{

  DicomImage ^ mpDicomImage = gcnew DicomImage();

  if ( mpDicomImage == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate DicomImage");

  mpDicomImage->m_mpImage = mpImage;

  mpDicomImage->m_iRows = mpImage->Rows;
  mpDicomImage->m_iColumns = mpImage->Columns;
  mpDicomImage->m_fRescaleIntercept = mpImage->RescaleIntercept;
  mpDicomImage->m_fRescaleSlope = mpImage->RescaleSlope;
  mpDicomImage->m_iPixelPaddingValue = mpImage->PixelPaddingValue;
  mpDicomImage->m_mpPixelData = mpImage->PixelData;
  mpDicomImage->m_bIsEncapsulatedPdf =  mpImage->IsEncapsulatedPdf;
  mpDicomImage->m_iBitsAllocated = mpImage->BitsAllocated;
  mpDicomImage->m_bIsMonochrome = mpImage->IsMonochrome;

  Double dWindowWidth(0.0F);
  Double dWindowCenter(0.0F);

  if ( ! mpImage->TryGetValue(0x0028, 0x1050, dWindowWidth) )
    dWindowWidth = ( mpDicomImage->m_bIsMonochrome ? 2000.0F : 128.0F );

      

  if ( ! mpImage->TryGetValue(0x0028, 0x1051, dWindowCenter) )
    dWindowCenter = ( mpDicomImage->m_bIsMonochrome ? 400.0F : 255.0F );

  mpDicomImage->m_mpWindowLevel = gcnew WindowLevel( dWindowCenter, dWindowWidth);

  return mpDicomImage;
}



/**
 * Generates a NO IMAGE bitmap for display when we have DICOM information but no Pixel Data
 *
 * @param iWdith      Width of image
 * @param iHeight     Height of image 
 *
 * @return No Image Bitmap
 */
Drawing::Bitmap ^ DicomImage::GenerateNoImageBitmap(int iWidth, int iHeight)
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
Drawing::Bitmap ^ DicomImage::GenerateCompressedImageBitmap(int iWidth, int iHeight)
{
  Drawing::StringFormat ^ mpFormat = gcnew Drawing::StringFormat();
  mpFormat->Alignment = Drawing::StringAlignment::Center;
  mpFormat->LineAlignment = System::Drawing::StringAlignment::Center;
    
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(iWidth, iHeight, Drawing::Imaging::PixelFormat::Format24bppRgb);
  Drawing::Graphics ^ mpGraphics = Drawing::Graphics::FromImage((Drawing::Image ^)mpBitmap);
  mpGraphics->DrawString("Image is compressed", gcnew Drawing::Font("Tahoma",12),  Drawing::Brushes::Green, (iWidth / 2.0F), (iHeight / 2.0F), mpFormat );
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
Drawing::Bitmap ^ DicomImage::GeneratePdfBitmap(int iWidth, int iHeight)
{
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(System::Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream( "pdf.png" ));

  return mpBitmap;

} // GeneratePdfBitmap;



/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpImage       Element list to render pixel data from
 * @param mpWindow        Window level
 * @param dZoomFactor     Zoom factor
 *
 * @return Rendered bitmap image
 */
Drawing::Bitmap ^ DicomImage::Render( WindowLevel ^ mpWindowLevel, float dZoomFactor )
{

  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);

   // =============================================================================================================
  // If no pixel data exists then return an image indicating no image exists. The zoom factor will be ignored.
  // =============================================================================================================
  if ( m_mpPixelData->Size == 0 )
  {

    if ( m_mpImage->IsEncapsulatedPdf )
      return GeneratePdfBitmap(iWidth, iHeight);
    else
      return GenerateNoImageBitmap(iWidth, iHeight);

  } // END ... If there is no pixel data

  // ==========================================================================================================================
  // Else if thers is pixel data and the  the buffer is less then the image size is suppose to be then the image is compressed
  // ==========================================================================================================================
  else if ( m_mpPixelData->Size < m_mpImage->GetImageSize() )
     return GenerateCompressedImageBitmap(iWidth, iHeight);


  // =====================================
  // Call the appropriate rendering method
  // ======================================
  if ( m_bIsMonochrome )
  {
    if ( m_iBitsAllocated == 8 )
      return RenderMonochrome<unsigned char>(m_mpPixelData->GetByteData(), mpWindowLevel );
    else
      return RenderMonochrome<short>(m_mpPixelData->GetShortData(), mpWindowLevel);
  }
  else
    return RenderRgb(m_mpPixelData->GetByteData(),  mpWindowLevel);
 


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
Drawing::Image ^ DicomImage::RenderThumbnail(unsigned int iWidth, unsigned int iHeight, WindowLevel ^ mpWindowLevel)
{

  
  Drawing::Bitmap ^ mpBitmap = ( mpWindowLevel == nullptr ? Render() : Render(mpWindowLevel) );

  System::Drawing::Image::GetThumbnailImageAbort ^ mpCallback = gcnew System::Drawing::Image::GetThumbnailImageAbort( ThumbnailCallback );


  return mpBitmap->GetThumbnailImage((int)iWidth, (int)iHeight, mpCallback, System::IntPtr::Zero);

} // RenderThumbnail(IImage ^ mpImage, unsigned int iWdith, unsigned int iHeight)


/// Renders the pixel data as an unsigned char
template<typename T>
System::Drawing::Bitmap ^ DicomImage::RenderMonochrome( array<T> ^ pPixelData, WindowLevel ^ mpWindowLevel)
{

    

  // =================================================================================================================
  // Get the pixel padding value from the header and adjust it to the first pixel in the image. This is done 
  // because many time the padding value is not set and the value returned is UInt16::MaxValue which is not correct.
  // The first pixel is in the usually outside the fov so I will use it for  PixelPaddingValue.
  // =================================================================================================================
  T iPixelPaddingValue((T)m_iPixelPaddingValue);
  iPixelPaddingValue =  ( (int)(pPixelData[0]) != iPixelPaddingValue ? (int)(pPixelData[0]) : iPixelPaddingValue );


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
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(m_iColumns, m_iRows, Drawing::Imaging::PixelFormat::Format24bppRgb);

  // Lock to internal bitmap buffer so we can load the data
  Drawing::Imaging::BitmapData ^ mpBitmapData = 
    mpBitmap->LockBits(System::Drawing::Rectangle(0,0,m_iColumns,m_iRows), 
                                                  Drawing::Imaging::ImageLockMode::ReadOnly, 
                                                  Drawing::Imaging::PixelFormat::Format24bppRgb);

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
  
      if ( pixel != (T)iPixelPaddingValue )
      {     
        //if ( sModality == "CT"  )
        pixel = (T)(((float)pixel * m_fRescaleSlope) + m_fRescaleIntercept);

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
array<unsigned char> ^ DicomImage::CreateLookUpTable(double dWidth, double dCenter )
{


  
  const int lutSize = (int)Math::Pow(2.0,(double)sizeof(T) * 8.0);
  
  /// Allocate look table
  array<unsigned char> ^ lut = gcnew array<unsigned char>(lutSize);

  const double dAdjustedWidth( dWidth - 1.0F );
  const double dHalfWidth( dAdjustedWidth/2.0F );
  const double dAdjustedCenter( dCenter - 0.5F );
  const int iWindowMinimum( (int)(dAdjustedCenter - dHalfWidth) );
  const int iWindowsMaximum( (int)(dAdjustedCenter + dHalfWidth) );

 

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
    else lut[index] = (unsigned char)(((i - dAdjustedCenter)/dAdjustedWidth + 0.5F) * 255);

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
array<unsigned short> ^ DicomImage::CreateLookUpTable2(double dWidth, double dCenter )
{

  
  const int lutSize = (int)Math::Pow(2.0,(double)sizeof(T) * 8.0);


  
  /// Allocate look table
  array<unsigned short> ^ lut = gcnew array<unsigned short>(lutSize);

  const double dAdjustedWidth( dWidth - 1.0 );
  const double dHalfWidth( dAdjustedWidth/2.0F );
  const double dAdjustedCenter( dCenter - 0.5F );
  const int iWindowMinimum( (int)(dAdjustedCenter - dHalfWidth) );
  const int iWindowsMaximum( (int)(dAdjustedCenter + dHalfWidth) );

 

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
    else lut[index] = (unsigned short)(((i - dAdjustedCenter)/dAdjustedWidth + 0.5F) * 65535 );

  }

  return lut;

} // CreateLookUpTable(int iWidth, int iCenter )




System::Drawing::Bitmap ^ DicomImage::RenderRgb( array<unsigned char> ^ pPixelData, WindowLevel ^ mpWindowLevel )
{

  unsigned char iPixelPaddingValue((unsigned char)m_iPixelPaddingValue);


  /// Default the width to 255 if 0
  //const double dWindowWidth( mpWindowLevel->Width <= 0.0F ? 255.0F : mpWindowLevel->Width );
 
   // Create the RGB value lookup table
  array<unsigned char> ^ lookupTable = CreateLookUpTable<unsigned char>(mpWindowLevel->Width, mpWindowLevel->Center);


  // Create the bitmap to hold the new render image
  Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(m_iColumns, m_iRows, Drawing::Imaging::PixelFormat::Format24bppRgb);

  // Lock to internal bitmap buffer so we can load the data
  Drawing::Imaging::BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0,0,m_iColumns,m_iRows), 
                                                                    Drawing::Imaging::ImageLockMode::ReadOnly,
                                                                    Drawing::Imaging::PixelFormat::Format24bppRgb);

  // Destination pointer
  unsigned char * pPixelBuffer = reinterpret_cast<unsigned char *>(mpBitmapData->Scan0.ToPointer());

  // Destination pixel size
  const unsigned short pixelSize = 3;

  // ====================================================================================
  // Get the first pixel value to set an alternator value just in case they do not match
  // ====================================================================================
  int iPixelPosition(0);

  //===============================================================
  // Loop through all the pixels and convert them to RGB value.
  //==============================================================
  for (register int i(0); i < mpBitmapData->Height; ++i)
  {
    unsigned char * row = pPixelBuffer + (i * mpBitmapData->Stride);

    for (register int j(0); j < mpBitmapData->Width; j++ )
    {
      const int offset(j * pixelSize);

      row[offset + 2]    = lookupTable[pPixelData[iPixelPosition++]];
      row[offset + 1]    = lookupTable[pPixelData[iPixelPosition++]];
      row[offset]        = lookupTable[pPixelData[iPixelPosition++]];


    } // END ... For each column

  } // END ... For each row


  // Unlock the bitmap buffer
  mpBitmap->UnlockBits(mpBitmapData);


  // Returned the rendered bitmap
  return mpBitmap;

}




bool DicomImage::TryGetLut(IImage ^ mpImage, unsigned short iGroup, unsigned short iElement, Lut ^% mpLut)
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




/**
 * Performs the pixel transformation for a short 
 *
 * @param pixel           Pixel to perform transform on
 * @param mpWindowLevel   Window level
 *
 * @return New pixel value
 */
short DicomImage::RenderPixelShort(short pixel, WindowLevel ^ mpWindowLevel)
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

/**
 * Performs the pixel transformation for a unsigned char 
 *
 * @param pixel           Pixel to perform transform on
 * @param mpWindowLevel   Window level
 *
 * @return New pixel value
 */
unsigned char DicomImage::RenderPixelByte(short pixel, WindowLevel ^ mpWindowLevel)
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
System::Windows::Media::Imaging::BitmapSource ^ DicomImage::RenderImage(WindowLevel ^ mpWindowLevel)
{

  Viatronix::Dicom::Buffer ^ mpBuffer(m_mpPixelData);

  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);




  System::Windows::Media::Imaging::BitmapSource ^ mpSource;


  const int iBytesPerPixel( m_iBitsAllocated / 8 );
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

