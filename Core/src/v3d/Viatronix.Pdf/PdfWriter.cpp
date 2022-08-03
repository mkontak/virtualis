
// includes
#include "Stdafx.h"
#include "Exceptions.h"
#include "PdfWriter.h"


// namespaces
USING_PDF_NS

using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Collections::Generic;


/**
 * Constructor.
 */
PdfWriter::PdfWriter()
{
  try
  {
    m_pdf.reset(new std::shared_ptr<pdf::Pdf>(pdf::Pdf::Create()));
  }
  catch(pdf::PdfOutOfMemoryException & ex)
  {
    throw gcnew Viatronix::Pdf::OutOfMemoryException(gcnew String(ex.GetTextMessage().c_str()));
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // PdfWriter()


/**
 * Gets the number of pages in the report.
 */
int PdfWriter::PageCount::get()
{
  return (*m_pdf)->PageCount();
} // PageCount::get()


/**
 * Gets the currently selected page.
 */
unsigned int PdfWriter::SelectedPage::get()
{
  unsigned int uiResult = 0;

  try
  {
    uiResult = (*m_pdf)->SelectedPage();
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return uiResult;
} // SelectedPage::get()


/**
 * Adds an image to the document.
 *
 * @param mpBitmap  The image to add.
 * @returns  The handle of the image.
 */
int PdfWriter::AddImage(Bitmap ^ mpBitmap)
{
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

  scoped_ptr<std::shared_ptr<Gdiplus::Bitmap>> pBitmap;
  pBitmap.reset(new std::shared_ptr<Gdiplus::Bitmap>(new Gdiplus::Bitmap(mpBitmap->Width, mpBitmap->Height, PixelFormat24bppRGB)));
  
  Gdiplus::BitmapData bitmapData;
  (*pBitmap)->LockBits(new Gdiplus::Rect(0, 0, mpBitmap->Width, mpBitmap->Height), Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bitmapData);

  System::Drawing::Imaging::BitmapData ^ mpBitmapData = mpBitmap->LockBits(System::Drawing::Rectangle(0, 0, mpBitmap->Width, mpBitmap->Height), 
    System::Drawing::Imaging::ImageLockMode::ReadOnly, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

  int stride = Math::Abs(bitmapData.Stride);
  IntPtr destinationOrigin = IntPtr(bitmapData.Scan0);

  for(unsigned int i = 0; i < bitmapData.Height; ++i)
  {
    IntPtr destinationScanPos = destinationOrigin + (stride * ((bitmapData.Height - 1) - i));
    IntPtr sourceScanPos = mpBitmapData->Scan0 + (stride * i);
    ::memcpy_s(destinationScanPos.ToPointer(), stride, sourceScanPos.ToPointer(), stride);
  }

  mpBitmap->UnlockBits(mpBitmapData);
  (*pBitmap)->UnlockBits(&bitmapData);

  Gdiplus::GdiplusShutdown(gdiplusToken);

  return (*m_pdf)->AddImage(*pBitmap);
} // AddImage( mpBitmap )


/**
 * Adds an image from the filesystem to the document.
 *
 * @mpFilename 
 */
int PdfWriter::AddImageFromFile(String ^ mpFilename, int iOptions)
{
  int iResult = 0;
  
  try
  {
    iResult = (*m_pdf)->AddImageFromFile(ss(mpFilename), iOptions);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return iResult;
} // AddImageFromFile( mpFilename, iOptions )


/**
 * Adds a line from the last position to the specified coordinates.
 *
 * @param dEndX  The coordinate to end the line segment.
 * @param dEndY  The coordinate to end the line segment.
 */
void PdfWriter::AddLineToPath(double dEndX, double dEndY)
{
  (*m_pdf)->AddLineToPath(dEndX, dEndY);
} // AddLineToPath( dEndX, dEndY )


/**
 * Adds a true type font to the document.
 *
 * @param mpFontName  The name of the font.
 * @param iEmbed  Should the font be embedded in the PDF file.
 */
int PdfWriter::AddTrueTypeFont(String ^ mpFontName, bool iEmbed)
{
  int iResult = 0;
  
  try
  {
    iResult = (*m_pdf)->AddTrueTypeFont(ss(mpFontName), iEmbed);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return iResult;
} // AddTrueTypeFont( mpFontName, iEmbed )


/**
 * Closes the path.
 */
void PdfWriter::ClosePath()
{
  (*m_pdf)->ClosePath();
} // ClosePath()


/**
 * Draws a box on the selected page with the specified coordinates.
 *
 * @param dStartX  The X coordinate of the top/left position.
 * @param dStartY  The Y coordinate of the top/left position.
 * @param dWidth   The width of the box.
 * @param dHeight  The height of the box.
 * @param iOptions The options.
 */
void PdfWriter::DrawBox(double dStartX, double dStartY, double dWidth, double dHeight, int iOptions)
{
  try
  {
    (*m_pdf)->DrawBox(dStartX, dStartY, dWidth, dHeight, iOptions);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // DrawBox( dStartX, dStartY, dWidth, dHeight, iOptions )


/**
 * Draws a line on the selected page with the specified coordinates.
 *
 * @param dStartX  The X coordinate of the top/left position.
 * @param dStartY  The Y coordinate of the top/left position.
 * @param dEndX    The X coordinate of the bottom/right position.
 * @param dEndY    The Y coordinate of the bottom/right position.
 */
void PdfWriter::DrawLine(double dStartX, double dStartY, double dEndX, double dEndY)
{
  try
  {
    (*m_pdf)->DrawLine(dStartX, dStartY, dEndX, dEndY);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // DrawLine( dStartX, dStartY, dEndX, dEndY )


/**
 * Draws the path out to the page.
 *
 * @param eOptions  What should we draw?
 */
void PdfWriter::DrawPath(PathOptions eOptions)
{
  pdf::Pdf::PathOptions eLLOptions;

  switch(eOptions)
  {
    case Viatronix::Pdf::PdfWriter::PathOptions::Outline:
      eLLOptions = pdf::Pdf::PATH_OUTLINE;
      break;
    case Viatronix::Pdf::PdfWriter::PathOptions::Fill:
      eLLOptions = pdf::Pdf::PATH_FILL;
      break;
    case Viatronix::Pdf::PdfWriter::PathOptions::Outline_Fill:
      eLLOptions = pdf::Pdf::PATH_OUTLINE_FILL;
      break;
  }

  (*m_pdf)->DrawPath(eLLOptions);
} // DrawPath( eOptions )


/**
 * Draws text on the current page.
 *
 * @param dX     The X coordinate of the top/left position of the text.
 * @param dY     The Y coordinate of the top/left position of the text.
 * @param mpText The text to draw to the current page.
 */
void PdfWriter::DrawText(double dX, double dY, String ^ mpText)
{
  try
  {
#ifdef UNICODE
    (*m_pdf)->DrawTextW(dX, dY, ws(mpText));
#else
    (*m_pdf)->DrawTextA(dX, dY, ss(mpText));
#endif
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // DrawText( dX, dY, mpText )


/**
 * Draws text wrapped to the specified width.
 *
 * @param dX     The top left coordinate.
 * @param dY     The top left coordinate.
 * @param dWidth The width of the text line.  If text is longer a new line will be started.
 * @param mpText The text to display.
 */
void PdfWriter::DrawWrappedText(double dX, double dY, double dWidth, String ^ mpText)
{
  try
  {
    (*m_pdf)->DrawWrappedText(dX, dY, dWidth, ss(mpText));
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // DrawWrappedText( dX, dY, dWidth, mpText )


/**
 * Draws the image at the selected coordinates with the specified transform.
 *
 * @param dLeft                 The top/left coordinates.
 * @param dTop                  The top/left coordinates.
 * @param dWidth                The width of the image.
 * @param dHeight               The height of the image.
 * @param iHorizontalAlignment  The horizontal alignment.
 * @param iVerticalAlignment    The vertical alignment.
 * @param iRotate               The rotation constant.
 */
void PdfWriter::FitImage(double dLeft, double dTop, double dWidth, double dHeight, int iHorizontalAlignment, int iVerticalAlignment, int iRotate)
{
  try
  {
    (*m_pdf)->FitImage(dLeft, dTop, dWidth, dHeight, iHorizontalAlignment, iVerticalAlignment, iRotate);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // FitImage( dLeft, dTop, dWidth, dHeight, iHorizontalAlignment, iVerticalAlignment, iRotate )


/**
 * Gets the selected image's width.
 *
 * @returns  The image width.
 */
int PdfWriter::GetImageWidth()
{
  return (*m_pdf)->ImageWidth();
} // GetImageWidth()


/**
 * Gets the selected image's height.
 *
 * @returns  The image height.
 */
int PdfWriter::GetImageHeight()
{
  return (*m_pdf)->ImageHeight();
} // GetImageHeight()


/**
 * Gets the text height.
 *
 * @returns  The text height.
 */
double PdfWriter::GetTextHeight()
{
  return (*m_pdf)->GetTextHeight();
} // GetTextHeight()


/**
 * Gets the wrapped text height.
 *
 * @param dWidth  The width.
 * @param mpText  The text to measure.
 * @returns       The height of the text.
 */
double PdfWriter::GetWrappedTextHeight(double dWidth, String ^ mpText)
{
  return (*m_pdf)->GetWrappedTextHeight(dWidth, ss(mpText));
} // GetWrappedTextHeight( dWidth, mpText )


/**
 * Moves the current position to a new point.
 *
 * @param dNewX  The coordinate to start the path with.
 * @param dNewY  The coordinate to start the path with.
 */
void PdfWriter::MovePath(double dNewX, double dNewY)
{
  (*m_pdf)->MovePath(dNewX, dNewY);
} // MovePath( dNewX, dNewY )


/**
 * Adds a new page to the document.
 */
void PdfWriter::NewPage()
{
  (*m_pdf)->NewPage();
} // NewPage()


/**
 * Saves the document out to the specified file.
 */
void PdfWriter::Save(String ^ mpFilename)
{
  try
  {
    (*m_pdf)->Save(ss(mpFilename));
  }
  catch(pdf::PdfIOException & ex)
  {
    throw gcnew Viatronix::Pdf::IOException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // Save( mpFilename )


/**
 * Marks the specified font as active.
 *
 * @param iFontId  The font id.
 */
void PdfWriter::SelectFont(int iFontId)
{
  try
  {
    (*m_pdf)->SelectFont(iFontId);
  }
  catch(pdf::PdfNotFoundException & ex)
  {
    throw gcnew Viatronix::Pdf::NotFoundException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SelectFont( iFontId )


/**
 * Selects the specified page.
 *
 * @param iPage  The page to select.
 */
void PdfWriter::SelectPage(unsigned int iPage)
{
  try
  {
    (*m_pdf)->SelectPage(iPage);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SelectPage( iPage )


/**
 * Sets the fill color.
 *
 * @param dRed    The red component.
 * @param dGreen  The green component.
 * @param dBlue   The blue component.
 */
void PdfWriter::SetFillColor(double dRed, double dGreen, double dBlue)
{
  try
  {
    (*m_pdf)->SetFillColor(dRed, dGreen, dBlue);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SetFillColor( dRed, dGreen, dBlue )


/**
 * Sets information about this pdf.
 *
 * @param eField  The field to update.
 * @param mpValue  The value to set.
 */
void PdfWriter::SetInformation(Viatronix::Pdf::PdfWriter::InformationFields eField, String ^ mpValue)
{
  try
  {
    pdf::Pdf::InformationFields eLLField;

    switch(eField)
    {
      case Viatronix::Pdf::PdfWriter::InformationFields::Version:
        eLLField = pdf::Pdf::INFO_VERSION;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Author:
        eLLField = pdf::Pdf::INFO_AUTHOR;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Title:
        eLLField = pdf::Pdf::INFO_TITLE;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Subject:
        eLLField = pdf::Pdf::INFO_SUBJECT;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Keywords:
        eLLField = pdf::Pdf::INFO_KEYWORDS;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Creator:
        eLLField = pdf::Pdf::INFO_CREATOR;
        break;
      case Viatronix::Pdf::PdfWriter::InformationFields::Producer:
        eLLField = pdf::Pdf::INFO_PRODUCER;
        break;
    }

    (*m_pdf)->SetInformation(eLLField, ss(mpValue));
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SetInformation( eField, mpValue )


/**
 * Sets the line width.
 *
 * @param dWidth  The new width.
 */
void PdfWriter::SetLineWidth(double dWidth)
{
  try
  {
    (*m_pdf)->SetLineWidth(dWidth);
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SetLineWidth( dWidth )


/**
 * Sets the measurement units.
 *
 * @param eUnits  The new units.
 */
void PdfWriter::SetMeasurementUnits(Viatronix::Pdf::PdfWriter::MeasurementUnits eUnits)
{
  pdf::Pdf::MeasurementUnits eLLUnits;

  switch(eUnits)
  {
    case Viatronix::Pdf::PdfWriter::MeasurementUnits::Points:
      eLLUnits = pdf::Pdf::UNITS_POINTS;
      break;
    case Viatronix::Pdf::PdfWriter::MeasurementUnits::Inches:
      eLLUnits = pdf::Pdf::UNITS_INCHES;
      break;
    case Viatronix::Pdf::PdfWriter::MeasurementUnits::Millimeters:
      eLLUnits = pdf::Pdf::UNITS_MILLIMETERS;
      break;
  }

  (*m_pdf)->SetMeasurementUnits(eLLUnits);
} // SetMeasurementUnits( eUnits )


/**
 * Updates the origin on the pdf.
 *
 * @param eOrigin  The new origin.
 */
void PdfWriter::SetOrigin(Viatronix::Pdf::PdfWriter::Origin eOrigin)
{
  pdf::Pdf::Origin eLLOrigin;
  switch(eOrigin)
  {
    case Viatronix::Pdf::PdfWriter::Origin::BottomLeft:
      eLLOrigin = pdf::Pdf::ORIGIN_BOTTOM_LEFT;
      break;
    case Viatronix::Pdf::PdfWriter::Origin::TopLeft:
      eLLOrigin = pdf::Pdf::ORIGIN_TOP_LEFT;
      break;
    case Viatronix::Pdf::PdfWriter::Origin::TopRight:
      eLLOrigin = pdf::Pdf::ORIGIN_TOP_RIGHT;
      break;
    case Viatronix::Pdf::PdfWriter::Origin::BottomRight:
      eLLOrigin = pdf::Pdf::ORIGIN_BOTTOM_RIGHT;
      break;
  }

  (*m_pdf)->SetOrigin(eLLOrigin);
} // SetOrigin( eOrigin )


/**
 * Sets the document's page size.
 *
 * @param mpPageSize  The new page size.
 */
void PdfWriter::SetPageSize(String ^ mpPageSize)
{
  try
  {
    (*m_pdf)->SetPageSize(ss(mpPageSize));
  }
  catch(pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }
} // SetPageSize( mpPageSize )


/**
 * Sets the text alignment from the enum.
 *
 * @param align  The new alignment.
 */
void PdfWriter::SetTextAlignment(Alignment align)
{
  pdf::Pdf::Alignment alignment;

  switch(align)
  {
    case Alignment::Left:
      alignment = pdf::Pdf::ALIGN_LEFT;
      break;
    case Alignment::Center:
      alignment = pdf::Pdf::ALIGN_CENTER;
      break;
    case Alignment::Right:
      alignment = pdf::Pdf::ALIGN_RIGHT;
      break;
    case Alignment::Justified:
      alignment = pdf::Pdf::ALIGN_JUSTIFIED;
      break;
    case Alignment::ForceJustified:
      alignment = pdf::Pdf::ALIGN_FORCE_JUSTIFIED;
      break;
    case Alignment::LastLineJustified:
      alignment = pdf::Pdf::ALIGN_LAST_LINE_JUSTIFIED;
      break;
  }

  (*m_pdf)->SetTextAlignment(alignment);
} // SetTextAlignment( align )


/**
 * Sets the text size.
 *
 * @param iSize  The new size.
 */
void PdfWriter::SetTextSize(unsigned int iSize)
{
  (*m_pdf)->SetTextSize(iSize);
} // SetTextSize( iSize )


/**
 * Sets the text underline mode.
 *
 * @param eOptions  The desired underline mode.
 */
void PdfWriter::SetTextUnderline(Viatronix::Pdf::PdfWriter::UnderlineOptions eOptions)
{
  pdf::Pdf::UnderlineOptions eLLOptions;

  switch(eOptions)
  {
    case Viatronix::Pdf::PdfWriter::UnderlineOptions::None:
      eLLOptions = pdf::Pdf::UNDERLINE_NONE;
      break;
    case Viatronix::Pdf::PdfWriter::UnderlineOptions::Single:
      eLLOptions = pdf::Pdf::UNDERLINE_SINGLE;
      break;
    case Viatronix::Pdf::PdfWriter::UnderlineOptions::Double:
      eLLOptions = pdf::Pdf::UNDERLINE_DOUBLE;
      break;
    case Viatronix::Pdf::PdfWriter::UnderlineOptions::Strikeout:
      eLLOptions = pdf::Pdf::UNDERLINE_STRIKEOUT;
      break;
    case Viatronix::Pdf::PdfWriter::UnderlineOptions::Over:
      eLLOptions = pdf::Pdf::UNDERLINE_OVER;
      break;
  }

  (*m_pdf)->SetTextUnderline(eLLOptions);
} // SetTextUnderline( eOptions )


/**
 * Starts the path at the specified coordinates.
 *
 * @param dStartX  The coordinate to start the path with.
 * @param dStartY  The coordinate to start the path with.
 */
void PdfWriter::StartPath(double dStartX, double dStartY)
{
  (*m_pdf)->StartPath(dStartX, dStartY);
} // StartPath( dStartX, dStartY )
