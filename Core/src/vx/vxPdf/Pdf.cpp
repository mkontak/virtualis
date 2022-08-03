// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: Pdf.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "stdafx.h"
#include "Pdf.h"
#include "PdfExceptions.h"
#include "PdfBuffer.h"
#include "PdfUtilities.h"


using namespace pdf;


const wchar_t Pdf::m_licenseKey[] =  L"jc7am37k8756te9qi5xp9wt8y"; 



/**
 * Creates the PDF object encapsulating whatever external library we are using
 */
Pdf::Pdf() :
#if defined(WIN64) || defined(_WIN64)
m_qp(L"quick_pdf64")
#else
m_qp(L"quick_pdf32")
#endif
{
  int rc;

  // Unlock
  if ( ( rc = m_qp.UnlockKey((wchar_t *)m_licenseKey)) != 1) 
    throw PdfException(LogRec(util::Strings::Format("Invalid license key for pdf library (%d)",rc), "Pdf","Pdf"));
  

} // Pdf()


/**
 * Destructor
 */
Pdf::~Pdf()
{
 
 
} // ~Pdf()


#pragma region create methods


/**
 * Creates the Pdf object using the buffer supplied. The buffer would be the memory/binary of an actual PDF
 *
 * @return pPdfBuffer      Buffer contains the PDF
 */
std::shared_ptr<Pdf> Pdf::Create()
{

  std::shared_ptr<Pdf> pPdf = std::shared_ptr<Pdf>(new Pdf());

  if ( pPdf == __nullptr )
    throw PdfOutOfMemoryException(LogRec("Failed to allocate memory for Pdf object","Pdf","Create"));

  return pPdf;

} // Create(std::shared_ptr<PdfBuffer> pPdfBuffer)


/**
 * Creates the Pdf object using the buffer supplied. The buffer would be the memory/binary of an actual PDF
 *
 * @param pPdfBuffer      Buffer contains the PDF
 * @return the pointer of the created Pdf
 */
std::shared_ptr<Pdf> Pdf::Create(std::shared_ptr<PdfBuffer> & pPdfBuffer)
{

  std::shared_ptr<Pdf> pPdf(Create());

  std::string sSource(pPdfBuffer->GetData<char>(),pPdfBuffer->GetSize());

  int rc;
  if ( (rc = pPdf->m_qp.LoadFromString(sSource, L"")) != 1 )
    throw PdfInvalidDataException(LogRec(util::Strings::Format("Failed to load the memory into the library (%d)", rc), "Pdf","Create"));

  return pPdf;

} // Create(std::shared_ptr<PdfBuffer> pPdfBuffer)


/**
 * Creates the Pdf object using the buffer supplied. The buffer would be the memory/binary of an actual PDF
 *
 * @param sFilename Filename to open
 * @return pointer of the created pdf
 */
std::shared_ptr<Pdf> Pdf::Create(const std::string & sFilename)
{

  std::shared_ptr<Pdf> pPdf(Create());

  std::wstring wFilename(PdfUtilities::ToWString(sFilename));
  
  int rc;
  if ( (rc = pPdf->m_qp.LoadFromFile((wchar_t *)wFilename.c_str(), L"")) != 1 )
    throw PdfInvalidDataException(LogRec(util::Strings::Format("Failed to load the file into the library : %s (%d)",sFilename.c_str(), rc), "Pdf","Create"));

  return pPdf;

} // Create(std::shared_ptr<PdfBuffer> pPdfBuffer)


#pragma endregion

/**
 * Generate a list of bitmap pages from the PDF
 *
 *  @return page pointer
 */
std::vector<std::shared_ptr<Gdiplus::Bitmap>> Pdf::GetPages()
{

  std::vector<std::shared_ptr<Gdiplus::Bitmap>> pages;

  const unsigned int iPageCount(PageCount());


  //m_qp.RenderDocumentToFile(72, 1, 10, 0, L"D:\\Testing\\page.bmp");

  // ===========================
  // Loop through all the pages
  // ===========================
  for ( unsigned int iPage(1); iPage <= iPageCount; iPage++ )
  {

   
 /*   int rt(0);

    rt = m_qp.RenderPageToFile(72, iPage,  1, L"D:\\Testing\\render.jpg");
    rt = m_qp.RenderPageToFile(72, iPage,  5, L"D:\\Testing\\render.png");*/

    std::string sBitmap( m_qp.RenderPageToString(72,iPage,0) );

    char * pBitmapData((char *)sBitmap.c_str());

    /// Bitmap file header
    BITMAPFILEHEADER bitmapFileHeader;
    memcpy((void *)&bitmapFileHeader, (void *)pBitmapData, sizeof(BITMAPFILEHEADER));

    /// Bitmap info
    BITMAPINFO bitmapInfo;
    memcpy((void *)&bitmapInfo, (void *)(pBitmapData + sizeof(BITMAPFILEHEADER)), sizeof(BITMAPINFO));

    /// Pixel data
    int pixelDataSize( (int)sBitmap.size() - bitmapFileHeader.bfOffBits );
    char * pPixelData = new char[pixelDataSize];
    memcpy(pPixelData, (pBitmapData + bitmapFileHeader.bfOffBits), pixelDataSize);

    //std::shared_ptr<Gdiplus::Bitmap> pBitmap = std::shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromBITMAPINFO(&bitmapInfo, pPixelData));
    //CLSID pngClsid;
    //pdf::PdfGdiplusEngine::GetEncoderClsid(L"image/png", &pngClsid);

    //pBitmap->Save(L"D:\\TEsting\\test.png", &pngClsid);
    pages.push_back(std::shared_ptr<Gdiplus::Bitmap>(Gdiplus::Bitmap::FromBITMAPINFO(&bitmapInfo, pPixelData))); 

  }

  return pages;
  
} // GetPages


/**
 * Converts the image to a PDF buffer
 *
 *  @return the pointer to the buffer
 */
std::shared_ptr<PdfBuffer> Pdf::ToPdfBuffer()
{

  std::shared_ptr<PdfBuffer> pBuffer =  PdfBuffer::Create(m_qp.SaveToString());

  return pBuffer;

} // ToPdfBuffer()


/**
 * Gets the Page count
 *
 *  @return the page count
 */
unsigned int Pdf::PageCount()
{
  return (unsigned int)m_qp.PageCount();
    
} // PageCount()


#pragma region font methods

/**
 * Adds the specified font.
 *
 * @param sFontnam      Font name
 * @param bEmbed        Flag to indicate if the font is to be embedded.
 * @return              add fuont otions
 */
int Pdf::AddTrueTypeFont(const std::string & sFontname, bool bEmbed)
{
  int rc(0);

  std::wstring wFontname(PdfUtilities::ToWString(sFontname));
  if ( ( rc = m_qp.AddTrueTypeFont(wFontname, ( bEmbed ? 1 : 0 ) )) == 0 )
    throw PdfException(LogRec(util::Strings::Format("Could not load font %s",sFontname),"Pdf","AddTrueTypeFont"));

  return rc;
} // AddTrueTypeFont(const std::string & sFontname, bool bEmbed)


#pragma endregion

#pragma region text methods

/**
 * Sets the text underline mode.
 *
 * @param eOptions  The desired underline mode.
 */
void Pdf::SetTextUnderline(UnderlineOptions eOptions)
{
  m_qp.SetTextUnderline(eOptions);
} // SetTextUnderline( eOptions )


/**
 * Adds the specified font.
 *
 * @param iSize       New size
 */
void Pdf::SetTextSize(unsigned int iSize)
{
  m_qp.SetTextSize(iSize);
} // SetTextSize(unsigned int iSize)


/*
 * Sets the texts alignment
 */
void Pdf::SetTextAlignment(Alignment eAlign)
{
  m_qp.SetTextAlign((int)eAlign);
} // SetTextAlignment(Alignment eAlign)


/**
 * Returns the text height
 */
double Pdf::GetTextHeight()
{
  return m_qp.GetTextHeight();
} // GetTextHeight()


#ifdef UNICODE

/**
 * Draws the specified text to the specified location
 *
 * @param dX     X position
 * @param dY     Y Position
 * @param sText   Text
 */
void Pdf::DrawTextW(double dX, double dY, const std::wstring & sText)
{
  

  int rc;

  if ( (rc = m_qp.DrawText( dX, dY, sText )) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to draw text \"%s\"",sText.c_str()),"Pdf","DrawText"));
} // DrawText(double x, double y, const std::string & sText)

#else

/**
 * Draws the specified text to the specified location
 *
 * @param dX     X position
 * @param dY     Y Position
 * @param sText   Text
 */
void Pdf::DrawTextA(double dX, double dY, const std::string & sText)
{
  wchar_t * pwText(PdfUtilities::ToWChar(sText));

  int rc;

  if ( (rc = m_qp.DrawText( dX, dY, pwText )) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to draw text \"%s\"",sText.c_str()),"Pdf","DrawText"));

  delete pwText;
} // DrawText(double x, double y, const std::string & sText)

#endif

/**
 * Draws wrapped text
 *
 * @param dX      X position
 * @param dY      Y Position
 * @param dWidth  Width
 * @param sText   Text
 */
void Pdf::DrawWrappedText(double dX, double dY, double dWidth, const std::string & sText )
{
  wchar_t * pwText(PdfUtilities::ToWChar(sText));

  int rc;

  if ( (rc = m_qp.DrawWrappedText( dX, dY, dWidth, pwText )) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to draw wrapped text \"%s\"",sText.c_str()),"Pdf","DrawText"));

  delete pwText;
} // DrawWrappedText(double dX, double dY, double dWidth, const std::string & sText )


/**
 * Returns the wrapped text height
 *
 * @param dWidth      Width
 * @param sText       Text
 *
 * @return  Text height
 */
double Pdf::GetWrappedTextHeight( double dWidth,  const std::string & sText )
{
  std::wstring wText(PdfUtilities::ToWString(sText));
  return m_qp.GetWrappedTextHeight(dWidth, wText);
}   // GetWrappedTextHeight( double dWidth,  const std::string & sText )

#pragma endregion


#pragma region image methods


/**
 * Fits the image to the specified space
 *
 * @param dLeft                   Left corner
 * @param dTop                    Top corner
 * @param dWidth                  Width 
 * @param dHeight                 Height
 * @param iHorizontalAlignment    Alignment
 * @param iVerticalAlignment      Alignment
 * @param iRotate                 Rotate
 */
void Pdf::FitImage( double dLeft, double dTop, double dWidth, double dHeight, int iHorizontalAlignment, int iVerticalAlignment, int iRotate )
{
  int rc;
  if ( (rc = m_qp.FitImage( dLeft, dTop, dWidth, dHeight, iHorizontalAlignment, iVerticalAlignment, iRotate )) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to fit image (%d)", rc), "Pdf","FitImage"));

} // FitImage( double dLeft, double dTop, double dWidth, double dHeight, int iHorizontalAlignment, int iVerticalAlignment, int iRotate )

/**
 * Gets the image width
 */
int Pdf::ImageWidth()
{
  return m_qp.ImageWidth();
} // ImageWidth()

/**
 * Gets the image height
 */
int Pdf::ImageHeight()
{
  return m_qp.ImageHeight();
} // ImageHeight()

/**
 * Adds the image from the file specified
 *
 * @param sFilename       Image file 
 *
 * @return Returns the image id
 */
int Pdf::AddImageFromFile(const std::string & sFilename, int iOptions)
{

  std::wstring wFilename(PdfUtilities::ToWString(sFilename), iOptions);
  
  int rc(0);
  
  if ( ( rc = m_qp.AddImageFromFile( wFilename, 0 ) ) == 0 )
    throw PdfException(LogRec(util::Strings::Format("Failed to load image from file %s", sFilename), "Pdf","AddImageFromFile"));

  return rc;

} //  AddImageFromFile(const std::string & sFilename)


#pragma endregion

/**
 * Creates a new page
 */
void Pdf::NewPage()
{

  m_qp.NewPage();

} // NewPage()

/**
 * Sets the page size
 *
 * @param ePageSize     Page size
 */
void Pdf::SetPageSize(const std::string & sPageSize)
{


  std::wstring wPageSize(PdfUtilities::ToWString(sPageSize));

  int rc;
  if ( (rc = m_qp.SetPageSize( wPageSize ) ) != 1 )
   throw PdfException(LogRec(util::Strings::Format("Failed to set page size (%d)", rc), "Pdf","SetPageSize")); 

} // SetPageSize(PageSize ePageSize)


/**
 * Sets the line width
 * 
 * @param dWidth      New width
 */
void Pdf::SetLineWidth(double dWidth)
{
  int rc;

  if ( ( rc = m_qp.SetLineWidth(dWidth) ) != 1 )
   throw PdfException(LogRec(util::Strings::Format("Failed to set line width to %f (%d)", dWidth, rc), "Pdf","SetLineWidth")); 

} // SetLineWidth(double dWidth)

/**
 * Draws a line
 *
 * @param dStartX   starting x position
 * @param dStartY   starting y position
 * @param dEndX     ending x position
 * @param dEndY     ending y position
 */
void Pdf::DrawLine(double dStartX, double dStartY, double dEndX, double dEndY )
{
  int rc;
  if ( (rc = m_qp.DrawLine(dStartX, dStartY, dEndX, dEndY )) != 1 )
   throw PdfException(LogRec(util::Strings::Format("Failed to draw line width to (%d)", rc), "Pdf","DrawLine")); 

} // DrawLine(double dStartX, double dStartY, double dEndX, double dEndY )

/**
 * Sets the fill color
 *
 * @param dRed      Red
 * @param dGeeen    Green
 * @param dBlue     Blue
 */
void Pdf::SetFillColor(double dRed, double dGreen, double dBlue)
{

  int rc;

  if ( ( rc = m_qp.SetFillColor(dRed, dGreen, dBlue) ) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to set the filll color (%d)", rc), "Pdf","SetFillColor")); 

} //  SetFillColor(double dRed, double dGreen, double dBlue)


/**
 * Draws a box
 *
 * @param dLeft     Left
 * @param dTop      Top
 * @param dWidth    Width
 * @param dHeight   Height
 * @param iOptions   Options
 */
void Pdf::DrawBox(double dLeft, double dTop, double dWidth, double dHeight, int iOptions)
{
  int rc;

  if ( ( rc = m_qp.DrawBox(dLeft, dTop, dWidth, dHeight, iOptions) ) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to draw box color (%d)", rc), "Pdf","DrawBox")); 

} // DrawBox(double dLeft, double dTop, double dWidth, double dHeight, int iOptions)


/**
 * Selects the specified page
 *
 * @param iPage     Page to be selected
 */
void Pdf::SelectPage( unsigned int iPage)
{

  int rc;

  if ( ( rc = m_qp.SelectPage((int)iPage) ) != 1 )
    throw PdfException(LogRec(util::Strings::Format("Failed to select page %d (%d)", iPage, rc), "Pdf","SelectPage")); 

} // SelectPage( unsigned int iPage)

/**
 * Gets the selected page
 */
unsigned int Pdf::SelectedPage()
{

 return m_qp.SelectedPage();

} // SelectedPage( )

/**
 * Adds image to df
 *
 * @param pBitmap     Bitmap image to be added
 */
int Pdf::AddImage(std::shared_ptr<Gdiplus::Bitmap> pBitmap)
{
  //ULONG_PTR token;
  Gdiplus::Status eStatus;



  try
  {

    const int iWidth(pBitmap->GetWidth());
    const int iHeight(pBitmap->GetHeight());

    Gdiplus::BitmapData lockedBitmapData;
    
    Gdiplus::Rect rect( 0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());


    if ( (eStatus = pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, pBitmap->GetPixelFormat(), &lockedBitmapData)) != Gdiplus::Ok )
      throw PdfException(LogRec(util::Strings::Format("Failed to startup GDI+ (%d)", (int)eStatus), "Pdf","AddImage"));

    const unsigned int iBitmapInfoSize  = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
    const unsigned int iFileHeaderSize  = sizeof(BITMAPFILEHEADER);
    const unsigned int iImageSize       = lockedBitmapData.Height *  abs(lockedBitmapData.Stride);
 
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize            = iBitmapInfoSize;
	  bmi.bmiHeader.biWidth           = iWidth;
	  bmi.bmiHeader.biHeight          = iHeight;
	  bmi.bmiHeader.biSizeImage       = iBitmapInfoSize + iImageSize;
    bmi.bmiHeader.biPlanes          = 1;
    bmi.bmiHeader.biBitCount        = 24;
    bmi.bmiHeader.biCompression     = BI_RGB;
	  bmi.bmiHeader.biClrUsed         = 0;
	  bmi.bmiHeader.biClrImportant    = 0;
	  bmi.bmiHeader.biXPelsPerMeter   = 300;
	  bmi.bmiHeader.biYPelsPerMeter   = 300;

    BITMAPFILEHEADER bmh;
    bmh.bfType       =  ((WORD) ('M' << 8) | 'B');
	  bmh.bfSize       = iFileHeaderSize + bmi.bmiHeader.biSizeImage;
    bmh.bfReserved1  = 0;
    bmh.bfReserved2  = 0;
    bmh.bfOffBits    = iBitmapInfoSize + iFileHeaderSize;


    std::string sSource;


    char * pPtr(__nullptr);
    register unsigned int i(0);
  
    for ( pPtr = (char*)&bmh, i = 0; i < sizeof(BITMAPFILEHEADER); sSource.push_back(*pPtr++), i++ );
    for ( pPtr = (char*)&bmi, i = 0; i < iBitmapInfoSize; sSource.push_back(*pPtr++), i++ );
   
    char * pScan0((char *)lockedBitmapData.Scan0);
    char * pPixelData ( lockedBitmapData.Stride > 0 ? pScan0 : (pScan0 + lockedBitmapData.Stride * (iHeight - 1)) );

    for ( i = 0; i < iImageSize; sSource.push_back( *pPixelData++ ), i++ );


    pBitmap->UnlockBits(&lockedBitmapData);

    int rc(0);

    if ( ( rc = m_qp.AddImageFromString(sSource,0) ) == 0 )
      throw PdfException(LogRec("Failed to add image to PDF","Pdf","AddImage"));

    return rc;

  }
  catch ( PdfException &   )
  {
    throw;
  }
  catch ( ... )
  {
 
    throw PdfException(LogRec("Failed to add image to PDF, unspecified error","Pdf","AddImage"));
  }


} // AddImage(std::shared_ptr<Gdiplus::Bitmap> pBitmap)


/**
 * Sets information about this Pdf.
 *
 * @param eField  The field to update.
 * @param sValue  The value to set.
 */
void Pdf::SetInformation(InformationFields eField, const std::string & sValue)
{
  if(m_qp.SetInformation(eField, PdfUtilities::ToWString(sValue)) != 1)
  {
    throw pdf::PdfException(LogRec("Failed to update PDF information.","Pdf","SetInformation"));
  }
} // SetInformation( eField, sValue )


/**
 * Sets the specified font as active.
 *
 * @param iFontId  The font id to mark as active.
 */
void Pdf::SelectFont(int iFontId)
{
  int rc(0);
  
  if((rc = m_qp.SelectFont(iFontId)) != 1)
  {
    throw PdfNotFoundException(LogRec("Failed to select specified font.  Font Id is invalid.","Pdf","AddImage"));
  }
} // SelectFont( iFontId )


/**
 * Sets the origin to the desired origin.
 *
 * @param eOrigin  The new origin.
 */
void Pdf::SetOrigin(Origin eOrigin)
{
  m_qp.SetOrigin(eOrigin);
} // SetOrigin( eOrigin )


/**
 * Sets the measurement units to the desired units.
 *
 * @param eUnits  The new units.
 */
void Pdf::SetMeasurementUnits(MeasurementUnits eUnits)
{
  m_qp.SetMeasurementUnits(eUnits);
} // SetMeasurementUnits( eUnits )


/**
 * Starts the path at the specified coordinates.
 *
 * @param dStartX  The coordinate to start the path with.
 * @param dStartY  The coordinate to start the path with.
 */
void Pdf::StartPath(double dStartX, double dStartY)
{
  m_qp.StartPath(dStartX, dStartY);
} // StartPath( dStartX, dStartY )


/**
 * Adds a line from the last position to the specified coordinates.
 *
 * @param dEndX  The coordinate to end the line segment.
 * @param dEndY  The coordinate to end the line segment.
 */
void Pdf::AddLineToPath(double dEndX, double dEndY)
{
  m_qp.AddLineToPath(dEndX, dEndY);
} // AddLineToPath( dEndX, dEndY )


/**
 * Moves the current position to a new point.
 *
 * @param dNewX  The coordinate to start the path with.
 * @param dNewY  The coordinate to start the path with.
 */
void Pdf::MovePath(double dNewX, double dNewY)
{
  m_qp.MovePath(dNewX, dNewY);
} // MovePath( dNewX, dNewY )


/**
 * Draws the path out to the page.
 *
 * @param eOptions  What should we draw?
 */
void Pdf::DrawPath(PathOptions eOptions)
{
  m_qp.DrawPath(eOptions);
} // DrawPath( eOptions )


/**
 * Closes path
 */
void Pdf::ClosePath()
{
  m_qp.ClosePath();
} // ClosePath()

/**
 * Saves the PDF
 *
 * @param sFilename     File to save to
 */
void Pdf::Save(const std::string & sFilename)
{
  std::wstring wFilename(PdfUtilities::ToWString(sFilename));

  int rc;
  if ( ( rc = m_qp.SaveToFile((wchar_t *)wFilename.c_str())) != 1 )
    throw PdfIOException(LogRec(util::Strings::Format("Failed to save PDF file (%d) %s", rc, sFilename.c_str()), "Pdf","Save")); 
} // Save(const std::string & sFilename)