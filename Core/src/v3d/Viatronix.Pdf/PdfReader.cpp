
// includes
#include "stdafx.h"
#include "PdfReader.h"
#include "Exceptions.h"

// namespaces
USING_PDF_NS

using namespace System;
using namespace System::Drawing;
using namespace System::Collections::Generic;


/**
 * Constructor.
 */
PdfReader::PdfReader()
{
} // PdfReader()


/**
 * Returns the page count of the document.
 */
int PdfReader::PageCount::get()
{
  return (*m_pdf)->PageCount();
} // PageCount::get()


/**
 * Returns the list of pages of the document.
 */
cli::array<Bitmap ^> ^ PdfReader::Pages::get()
{
 
  List<Bitmap ^> ^ mpBitmaps = gcnew List<Bitmap ^>();
  std::vector<std::shared_ptr<Gdiplus::Bitmap>> pages = (*m_pdf)->GetPages();

  Gdiplus::Status status;

  for (int i = 0; i < (*m_pdf)->PageCount(); ++i)
  {
    std::shared_ptr<Gdiplus::Bitmap> pBitmap = pages[i];

    HBITMAP hBitmap;
    if ((status = pBitmap->GetHBITMAP(Gdiplus::Color::Transparent, &hBitmap)) == Gdiplus::Status::Ok)
    {
      mpBitmaps->Add(Bitmap::FromHbitmap(IntPtr(hBitmap)));
    }
  }

  return mpBitmaps->ToArray();
} // Pages::get()


/**
 * Creats the reader and loads the specified file.
 *
 * @param mpFilename  The filename.
 * @returns  The reader ready to go.
 */
PdfReader ^ PdfReader::Create(String ^ mpFilename)
{
  PdfReader ^ mpReader = gcnew PdfReader();

  try
  {
    mpReader->m_pdf.reset(new std::shared_ptr<pdf::Pdf>(pdf::Pdf::Create(ss(mpFilename))));
  }
  catch (pdf::PdfOutOfMemoryException & ex)
  {
    throw gcnew Viatronix::Pdf::OutOfMemoryException(gcnew String(ex.GetTextMessage().c_str()));
  }
  catch (pdf::PdfException & ex)
  {
    throw gcnew Viatronix::Pdf::PdfException(gcnew String(ex.GetTextMessage().c_str()));
  }

  return mpReader;
} // Create( mpFilename )
