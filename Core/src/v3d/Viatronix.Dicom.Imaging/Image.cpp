// This is the main DLL file.

#include "stdafx.h"
#include "Image.h"
#include "WindowLevel.h"

#include <limits.h>

using namespace System;
using namespace System::Drawing::Drawing2D;

USING_DICOM_IMAGING_NS


/** 
 * Renders the pixel data into a bitmap
 *
 */
Drawing::Bitmap ^ Image::RenderPixelData(Dataset ^ mpDataset )
{
  return RenderPixelData(mpDataset, gcnew WindowLevel((int)mpDataset->WindowCenter, (int)mpDataset->WindowWidth), 1.0);
} // RenderPixelData(Dataset ^ mpDataset)

/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpDataset       Element list to render pixel data from
 * @param mpWindow        Window level
 *
 * @return Rendered bitmap image
 */
Drawing::Bitmap ^ Image::RenderPixelData(Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel )
{
  return RenderPixelData(mpDataset, mpWindowLevel, 1.0);
} // RenderPixelData(Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel )

/** 
 * Renders the pixel data into a bitmap
 *
 * @param mpDataset       Element list to render pixel data from
 * @param mpWindow        Window level
 * @param dZoomFactor     Zoom factor
 *
 * @return Rendered bitmap image
 */
Drawing::Bitmap ^ Image::RenderPixelData(Dataset ^ mpDataset, WindowLevel ^ mpWindowLevel, double dZoomFactor )
{

  Viatronix::Dicom::Buffer ^ mpBuffer(mpDataset->PixelData);

  // Get the Rows/Columns of the image
  const unsigned int iHeight(512);
  const unsigned int iWidth(512);

  // =============================================================================================================
  // If no pixel data exists then return an image indicating no image exists. The zoom factor will be ignored.
  // =============================================================================================================
  if ( mpBuffer->Size == 0 )
  {

    if ( mpDataset->IsEncapsulatedPdf )
    {
      Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(System::Reflection::Assembly::GetExecutingAssembly()->GetManifestResourceStream( "pdf.png" ));

      return mpBitmap;
    }
    else
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


    }


  } // END ... If there is no pixel data

  // Get the bits allocated to determine the render method to call
  const unsigned short iBitsAllocated(mpDataset->BitsAllocated);

  // =====================================
  // Call the appropriate rendering method
  // ======================================
  if ( iBitsAllocated == 8 )
    return Zoom(RenderPixelData<unsigned char>(mpDataset, mpWindowLevel, reinterpret_cast<unsigned char *>(mpBuffer->GetBufferPtr().ToPointer())), dZoomFactor);
  else
    if ( mpDataset->PixelRepresentation == 0 && mpDataset->Modality != "CT" )
      return Zoom(RenderPixelData<unsigned short>(mpDataset, mpWindowLevel, reinterpret_cast<unsigned short *>(mpBuffer->GetBufferPtr().ToPointer())), dZoomFactor);
    else
      return Zoom(RenderPixelData<short>(mpDataset, mpWindowLevel, reinterpret_cast<short *>(mpBuffer->GetBufferPtr().ToPointer())), dZoomFactor);
 


} //  RenderPixelData(Dataset ^ mpDataset, Size ^ mpSize, WindowLevel ^ mpWindow )



/**
 * Renders a thumbnail image for the specified dataset for the specified size
 *
 * @param mpDataset     Dicom data
 * @param iWidth        Thumbnail Width
 * @param iHeight       Thumbnail Height
 *
 * @return Thumbnail
 */
Drawing::Image ^ Image::RenderThumbnail(Dataset ^ mpDataset, unsigned int iWidth, unsigned int iHeight)
{
  Drawing::Bitmap ^ mpBitmap = RenderPixelData(mpDataset);

  System::Drawing::Image::GetThumbnailImageAbort ^ mpCallback = gcnew System::Drawing::Image::GetThumbnailImageAbort( ThumbnailCallback );


  return mpBitmap->GetThumbnailImage(iWidth, iHeight, mpCallback, System::IntPtr::Zero);

} // RenderThumbnail(Dataset ^ mpDataset, unsigned int iWdith, unsigned int iHeight)

/**
 * Performs a zoom on the image supplied,
 *
 * @param mpBitmap      Original image
 * @param dZoomFactor   Zoom Factor
 *
 * @return  Zoomed image
 */
Drawing::Bitmap ^ Image::Zoom(Drawing::Bitmap ^ mpBitmap, double dZoomFactor)
{

  // Check for a null bitmap
  if ( mpBitmap == nullptr )
    throw gcnew ArgumentException("No bitmap supplied");

  // Check for a negative zoom factor
  if ( dZoomFactor <= 0.0 )
    throw gcnew ArgumentException("Zoom factor must be greater then 0");

  // Default return will be the actual bitmap
  Drawing::Bitmap ^ mpZoomBitmap = mpBitmap;


  try
  {

    if ( dZoomFactor != 1.0 )
    {

      // Calculate the new height and width for the zoomed image
      int iHeight  = (int)((double)mpBitmap->Height * dZoomFactor);
      int iWidth   = (int)((double)mpBitmap->Width  * dZoomFactor);

      // Create a new bitmap with the new size
      mpZoomBitmap = gcnew Drawing::Bitmap(iWidth, iHeight, Drawing::Imaging::PixelFormat::Format24bppRgb);

      // Keep original resolution
      mpZoomBitmap->SetResolution(mpBitmap->HorizontalResolution, mpBitmap->VerticalResolution);

      // Create graphics to load new image
      Drawing::Graphics ^ mpGraphics = Drawing::Graphics::FromImage(mpZoomBitmap);


      // Set interpolation
      mpGraphics->InterpolationMode = Drawing::Drawing2D::InterpolationMode::HighQualityBicubic;

      // Draw original image into the zomBitmap
      mpGraphics->DrawImage(  mpBitmap, 
                              Drawing::RectangleF(0.0F, 0.0F, (float)iWidth, (float)iHeight), 
                              Drawing::RectangleF(0.0F, 0.0F, (float)mpBitmap->Width, (float)mpBitmap->Height), 
                              Drawing::GraphicsUnit::Pixel);

      // Dispose of the graphics
      delete mpGraphics;


    } // END ... If the zoom factor is not 1.0


  }
  catch (Exception ^ )
  {
    throw;
  }

  /// Return image
  return mpZoomBitmap;

} // Image::Zoom(Drawing::Bitmap ^ mpBitmap, double dZoomFactor)



/** 
 * Renders the pixel data into a bitmap image used a scaling factor calculated by using 
 * the min/max pixel value in the image.
 *
 * @param mpDataset       Element list to render
 *
 * @return  Bitmap representation of the dicom image
 */
System::Drawing::Bitmap ^ Image::RenderPixelDataScaled(Dataset ^ mpDataset)
{
  return RenderPixelDataScaled(mpDataset, 1.0);
} // RenderPixelDataScaled(Dataset ^ mpDataset)

/** 
 * Renders the pixel data into a bitmap image used a scaling factor calculated by using 
 * the min/max pixel value in the image.
 *
 * @param mpDataset       Element list to render
 *
 * @return  Bitmap representation of the dicom image
 */
System::Drawing::Bitmap ^ Image::RenderPixelDataScaled(Dataset ^ mpDataset, double dZoomFactor)
{

  Viatronix::Dicom::Buffer ^ mpBuffer(mpDataset->PixelData);
  short  * pPixelData(nullptr);

  if (  mpBuffer->Size > 0 )
  {
    pPixelData = reinterpret_cast<short  *>(mpBuffer->GetBufferPtr().ToPointer());
  }
  else
  {
    Drawing::StringFormat ^ mpFormat = gcnew Drawing::StringFormat();
    mpFormat->Alignment = Drawing::StringAlignment::Center;
    mpFormat->LineAlignment = System::Drawing::StringAlignment::Center;


    Drawing::Bitmap ^ mpBitmap = gcnew Drawing::Bitmap(512, 512, Drawing::Imaging::PixelFormat::Format24bppRgb);
    Drawing::Graphics ^ mpGraphics = Drawing::Graphics::FromImage((Drawing::Image ^)mpBitmap);
    mpGraphics->DrawString("No Image", gcnew Drawing::Font("Tahoma",12),  Drawing::Brushes::DarkRed, 256.0F, 256.0F, mpFormat );
    mpGraphics->DrawLine(Drawing::Pens::DarkRed, Drawing::PointF(0,0), Drawing::PointF(512,512));
    mpGraphics->DrawLine(Drawing::Pens::DarkRed, Drawing::PointF(0,512), Drawing::PointF(512,0));
    mpGraphics->DrawRectangle(Drawing::Pens::DarkRed, Drawing::Rectangle(1, 1, 510, 510));


    return Zoom(mpBitmap, dZoomFactor);
  }

  const int iTypeSize = sizeof(unsigned short);
  const int iMaxValue = (int)Math::Pow(2.0,(double)iTypeSize * 8.0);

  const unsigned short iRows(mpDataset->Rows);
  const unsigned short iCols(mpDataset->Columns);
  const unsigned short iSamplesPerPixel(mpDataset->SamplesPerPixel);
  int iPixelPaddingValue(mpDataset->PixelPaddingValue);
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

  return Zoom(mpBitmap, dZoomFactor);

} // RenderPixelDataScaled(Dataset ^ mpDataset)



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
array<unsigned char> ^ Image::CreateLookUpTable(int iWindowMinimum, int iWindowMaximum )
{

  const int typeSize = sizeof(T);

  
  const int iTableSize = (int)Math::Pow(2.0,(double)typeSize * 8.0);

  System::Type ^ mpType = T::typeid;

  bool bUnsigned = (mpType->FullName->StartsWith("System.U") || mpType->FullName == "System.Byte" );
 

  const int iMin( bUnsigned ? 0 : -(iTableSize/2) );
  const int iMax( bUnsigned ? iTableSize - 1 : (int)Math::Abs(iMin) - 1); 


  /// Allocate look table
  array<unsigned char> ^ lookupTable = gcnew array<unsigned char>(iTableSize);


  int range = iWindowMaximum - iWindowMinimum;

  if (range < 1) range = 1;
  
  const double factor = (255.0 / (double)range);

  for ( int i(iMin); i <= iMax; i++ )
  {
    unsigned short index((unsigned short )i);
    if (i <= iWindowMinimum)
    {
      lookupTable[index] = 0;
    }
    else if (i >= iWindowMaximum)
    {
      lookupTable[index] = 255;
    }
    else
    {
      lookupTable[index] = (unsigned char)((i - iWindowMinimum) * factor);
    }
  }

  return lookupTable;

} // CreateLookUpTable(int iWindowMinimum, int iWindowMaximum, unsigned short iPixelPaddingValue )

