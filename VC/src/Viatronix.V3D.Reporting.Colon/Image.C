// $Id: Image.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Image.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// File:
// Description: Image object
// Created: 2/1/2001
// Author: George Economos
//
// Complete history on bottom of file

#include "stdafx.h"
#include "ReportLib.h"
#include "Image.h"
#include "ReportRevision.h"

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B') 

STDMETHODIMP Image::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = 
  {
    &IID_IImage
  };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

/**
 * gets the unique ID property 
 *
 * @param   pbsID           contains unique
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_ID(BSTR* pbsID)
{
  if(!pbsID)  
    return E_POINTER;
  
  return m_bsID.CopyTo(pbsID);
}

/**
 * gets the image size property 
 *
 * @param   piImageSize     contains image size upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_ImageSize(long* piImageSize)
{
  if(!piImageSize)
    return E_POINTER;
  *piImageSize = m_hBitmap? m_bi.bmiHeader.biSizeImage : -1;
  
  return S_OK;
}


/**
 * gets the image width property 
 *
 * @param   piWidth        contains image width upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_Width(long* piWidth)
{
  if(!piWidth)
    return E_POINTER;
  *piWidth = m_hBitmap? m_bi.bmiHeader.biWidth : -1;
  
  return S_OK;
}

/**
 * gets the image height property 
 *
 * @param   piHeight        contains image height upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_Height(long* piHeight)
{
  if(!piHeight)
    return E_POINTER;
  *piHeight = m_hBitmap? m_bi.bmiHeader.biHeight : -1;
  
  return S_OK;
}

/**
 * gets the bits per pixel property 
 *
 * @param   piBpp           contains image bits per pixel upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_BitsPerPixel(short* piBpp)
{
  if(!piBpp)
    return E_POINTER;
  *piBpp = m_hBitmap? m_bi.bmiHeader.biBitCount : -1;
  
  return S_OK;
}

/**
 * gets the HBITMAP property 
 *
 * @param   piBitmap        contains HBITMAP handle upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_Bitmap(long* piBitmap)
{
  if(!piBitmap)
    return E_POINTER;
  *piBitmap = reinterpret_cast<long>(m_hBitmap);
  
  return S_OK;
}

/**
 * gets the pixels property 
 *
 * @param   pvPixels        contains image pixels as safe array upon return
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::get_Pixels(VARIANT* pvPixels)
{
  if(!pvPixels)
    return E_POINTER;
  
  if(!m_hBitmap)
    return E_FAIL;  
  
  // create a safe array of unsigned ints
  pvPixels->vt = VT_UI1|VT_ARRAY;
  pvPixels->parray = SafeArrayCreateVector(VT_UI1, 0, m_bi.bmiHeader.biSizeImage);
  
  // access the array
  LPVOID pixels = NULL;
  SafeArrayAccessData(pvPixels->parray, &pixels);
  
  // copy bitmap bits
  memcpy(pixels, m_ppvBits, m_bi.bmiHeader.biSizeImage);
  
  // unaccess
  SafeArrayUnaccessData(pvPixels->parray);
  
  return S_OK;
}


/**
 * sets the pixels of an existing bitmap
 *
 * @param   vBytes         contains new image pixels as safe array
 * @return                 S_OK upon success
 */
STDMETHODIMP Image::SetPixels(VARIANT vBytes)
{
  if(!m_hBitmap)
    return E_FAIL;
  
  // access the array
  LPVOID pvPixels = NULL;
  SafeArrayAccessData(vBytes.parray, &pvPixels);
  
  // copy new image bits to bitmap
  memcpy(m_ppvBits, pvPixels, m_bi.bmiHeader.biSizeImage);
  
  // unaccess
  SafeArrayUnaccessData(vBytes.parray);
  
  return S_OK;
}

/**
 * creates a new bitmap
 *
 * @param   iWidth          width
 * @param   iHeight         height
 * @param   iBpp            bits per pixel
 * @return                  S_OK upon success
 */

STDMETHODIMP Image::Create(long iWidth, long iHeight, short iBpp)
{
  // free existing bitmap
  DeleteObject();
  
  // initialize bitmap header
  BITMAPINFOHEADER bih; 
  memset(&bih, 0, sizeof(BITMAPINFOHEADER));
  bih.biWidth = iWidth; 
  bih.biHeight = iHeight; 
  bih.biSizeImage = BytesPerLine(iWidth, iBpp) * iHeight;
  bih.biSize = sizeof(BITMAPINFOHEADER); 
  bih.biPlanes = 1; 
  bih.biBitCount = iBpp;
  
  // allocate a new image and zero out
  LPBYTE pvPixels = new BYTE[bih.biSizeImage];
  if(pvPixels == NULL)
    return E_OUTOFMEMORY;  
  memset(pvPixels, 0, bih.biSizeImage);
  
  CreateDIB(&bih, pvPixels);
  
  // free image
  delete[] pvPixels;  
  
  return S_OK;
}


/**
 * resizes a bitmap
 *
 * @param   iWidth          new width
 * @param   iHeight         new height
 * @return                  S_OK upon success
 */
STDMETHODIMP Image::Resize( long iWidth, long iHeight )
{
  try
  {
    if( !m_hBitmap )
      return E_FAIL;

    // the width is passed in as negative to indicate the overview image
    // I didn't do it...
    bool bIsOverview( false );
    if( iWidth < 0 )
    {
      bIsOverview = true;
      iWidth = -iWidth;
    }
    
    // create a gdiplus bitmap for the existing image
    Gdiplus::Bitmap * pBitmap = new Gdiplus::Bitmap( m_hBitmap, NULL );

		if (pBitmap->GetWidth() != pBitmap->GetHeight() && iWidth == iHeight)
		{
			int4 iw = pBitmap->GetWidth();
			int4 ih = pBitmap->GetHeight();
			
			Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(iw, iw, PixelFormat24bppRGB);

			register int4 x, y;
			register Gdiplus::Color gColor;
			Gdiplus::Color gBKcolor(172,172,172); /// This color is used as the background color for the ReportViewWnd

			if (iw > ih )
			{
			  Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(iw, iw, PixelFormat24bppRGB);

				for (y = 0; y < ih; y++)
				{
					for (x = 0; x < iw; x++)
					{
						pBitmap->GetPixel(x, y, &gColor);
						pBmp->SetPixel(x, y, gColor);
					} // for x
				} // for y

				for (y = ih; y < iw; y++)
				{
					for (x = 0; x < iw; x++)
					{
						pBmp->SetPixel(x, y, gBKcolor);
					} // for x
				} // for y

				pBitmap = pBmp;
				pBmp = NULL;
			}	//if (iw > ih)
			else
			{
				Gdiplus::Bitmap *pBmp = new Gdiplus::Bitmap(ih, ih, PixelFormat24bppRGB);

				for (y = 0; y < ih; y++)
				{
					for (x = 0; x < iw; x++)
					{
						pBitmap->GetPixel(x, y, &gColor);
						pBmp->SetPixel(x, y, gColor);
					} // for x
				} // for y

				for (y = 0; y < ih; y++)
				{
					for (x = iw; x < ih; x++)
					{
						pBmp->SetPixel(x, y, gBKcolor);
					} // for x
				} // for y

				pBitmap = pBmp;
				pBmp = NULL;
			} // else
		} // if the image is not square, squaring it with background color filling the void
    
    // create the new bitmap to hold the resized version
    Gdiplus::Bitmap * pNewBitmap = new Gdiplus::Bitmap( iWidth, iHeight, PixelFormat24bppRGB );
  
    // create the gaphics object based on the newly create bitmap
    Gdiplus::Graphics * pGraphics = new Gdiplus::Graphics( pNewBitmap );
  
    // use gdiplus' best filter
    pGraphics->SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );
  
    // draw the existing bitmap into the new bitmap
    pGraphics->DrawImage( pBitmap, 0, 0, iWidth, iHeight );
    
    if( pBitmap )
    {
      delete pBitmap;
      pBitmap = NULL;
    }

    if( pGraphics )
    {
      delete pGraphics;
      pGraphics = NULL;
    }

    // labling view type.
    if( bIsOverview )
      pNewBitmap->SetPixel( 0, 0, Gdiplus::Color::MakeARGB( 0xF, 0xF, 0xF, 0xF ) );      
      
    pNewBitmap->RotateFlip( Gdiplus::RotateNoneFlipY );
      
    Gdiplus::Rect rect( 0, 0, pNewBitmap->GetWidth(), pNewBitmap->GetHeight() ); 
  
    Gdiplus::BitmapData data;  
    pNewBitmap->LockBits( &rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &data );

    // initialize bitmap header
    BITMAPINFOHEADER bih; 
    memset( &bih, 0, sizeof( BITMAPINFOHEADER ) );
    bih.biSize = sizeof( bih );
    bih.biWidth = data.Width;
    bih.biHeight = data.Height;
    bih.biSizeImage = data.Stride * data.Height;
    bih.biPlanes = 1; 
    bih.biBitCount = 24;
    
    CreateDIB( &bih, static_cast< uint1*>( data.Scan0 ) );
    
    pNewBitmap->UnlockBits( &data );

    // free the new image bitmap
    if( pNewBitmap )
    {
      delete pNewBitmap;
      pNewBitmap = NULL;
    }
  }
  catch (...) 
  {
    return E_FAIL;
  }
  
  return S_OK;
} // resize().


/**
 * called when object is fully created
 *
 * @return                  S_OK upon success
 */
HRESULT Image::FinalConstruct()
{
  Reset();
  
  return S_OK;
}

/**
 * called when final reference to object has been released
 */
void Image::FinalRelease()
{
  // free bitmap
  DeleteObject();
}


/**
 * resizes a bitmap
 *
 * @param   spRevision      smart pointer to parent revision
 * @param   bsID            image unique ID
 * @return                  S_OK upon success
 */
HRESULT Image::Setup(CComPtr<IReportRevision>& spRevision, CComBSTR bsID)
{
  m_spRevision = spRevision;
  m_bsID = bsID;

  return S_OK;
}

/**
 * creates a new Windows DIB
 *
 * @param   pbih        BITMAPINFOHEADER pointer
 * @param   lpBits      image bits
 * @return              S_OK upon success
 */
HRESULT Image::CreateDIB(LPBITMAPINFOHEADER pbih,  uint1* lpBits)
{
  // free existing bitmap
  DeleteObject();
  
  if (!pbih || !lpBits)
    return E_POINTER;  
		
  // copy bitmap header to internal bitmap info structure
  memcpy(&m_bi.bmiHeader, pbih, sizeof(BITMAPINFOHEADER));
  
  // get desktop device context
  HDC hDC = ::GetDC(NULL);
  if (!hDC) 
    return HRESULT_FROM_WIN32(GetLastError());
		
  // create DIB setion
  m_hBitmap = CreateDIBSection(hDC, &m_bi,DIB_RGB_COLORS, &m_ppvBits, NULL, 0);
  
  ::ReleaseDC(NULL, hDC);
  
  if(!m_hBitmap)
    return HRESULT_FROM_WIN32(GetLastError());
  
  // copy the image
  memcpy(m_ppvBits, lpBits, m_bi.bmiHeader.biSizeImage);
  
  // assign to fast variables
  m_iWidth = m_bi.bmiHeader.biWidth;
  m_iHeight =  m_bi.bmiHeader.biHeight;
  
  return S_OK;
}

/**
 * frees a bitmap
 */
void Image::DeleteObject()
{  
  // release bitmap resources
  if (m_hBitmap)
    ::DeleteObject(m_hBitmap);

  Reset();
}

/**
 * resets all class variables
 */
void Image::Reset()
{
  memset(&m_bi, 0, sizeof(BITMAPINFO));
  m_hBitmap = NULL;
  m_ppvBits = NULL;
  m_iWidth = -1;
  m_iHeight = -1;  
}

/**
 * saves bitmap within a storage object
 *
 * @param   spStg       smart pointer to parent storage
 * @return              S_OK upon success
 */
HRESULT Image::WriteBitmap(CComPtr<IStorage>& spStg)
{
  HRESULT hr(S_OK);

  BITMAPFILEHEADER hdr;
  
  DWORD_PTR dwBitmapInfoSize = sizeof(BITMAPINFO) - sizeof(RGBQUAD);
  DWORD_PTR dwFileHeaderSize = dwBitmapInfoSize + sizeof(hdr);
  
  // fill in file header
  hdr.bfType       = DS_BITMAP_FILEMARKER;
  hdr.bfSize       = dwFileHeaderSize + m_bi.bmiHeader.biSizeImage;
  hdr.bfReserved1  = 0;
  hdr.bfReserved2  = 0;
  hdr.bfOffBits    = dwFileHeaderSize;
  
  // create a enw stream overwriting any
  CComPtr<IStream> spStream;
  if(FAILED(hr=CreateStream(spStg, IMAGE_PROPERTY_BITMAP, spStream)))
    return hr;

  // write out file header
  spStream->Write(&hdr, sizeof(hdr), NULL);
  
  // write out bitmap header
  spStream->Write(&m_bi.bmiHeader, dwBitmapInfoSize, NULL);
  
  // write out image bits
  spStream->Write(m_ppvBits, m_bi.bmiHeader.biSizeImage, NULL);

  // commit the changes
  spStg->Commit(STGC_DEFAULT);
  
  return S_OK;
}

/**
 * reads bitmap from a storage object
 *
 * @param   spStg       smart pointer to parent storage
 * @return              S_OK upon success
 */
HRESULT Image::ReadBitmap(CComPtr<IStorage>& spStg)
{
  HRESULT hr(S_OK);
  
  ReportRevision* pRevision = GetCoClass<ReportRevision>(m_spRevision);
  if(pRevision==NULL)
    return E_POINTER;
  
  // open the bitmap stream
  CComPtr<IStream> spStream;
  if(FAILED(hr=OpenStream(spStg, IMAGE_PROPERTY_BITMAP, spStream, pRevision->GetReadOnly())))
    return hr;
  
  // read in bitmap file header
  ULONG nRead(0);
  BITMAPFILEHEADER hdr;
  spStream->Read(&hdr, sizeof(hdr), &nRead);  
  if(nRead == 0) 
    return S_OK;
  
  // read in bitmap info structure
  BITMAPINFO	bi;
  spStream->Read(&bi, sizeof(BITMAPINFO) - sizeof(RGBQUAD), NULL);
  
  // allocate new image
  uint1* pBits = new uint1[bi.bmiHeader.biSizeImage];
  if (!pBits) 
    return E_OUTOFMEMORY;
  
  // read in the bits
  spStream->Read(pBits, bi.bmiHeader.biSizeImage, NULL);
  
  hr = CreateDIB(&bi.bmiHeader, pBits);
  
  // delete image
  delete[] pBits;

  return hr;  
}


// $Log: Image.C,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.5.2.2  2004/09/17 14:00:29  geconomos
// Fixed "upside down" image
//
// Revision 3.5.2.1  2004/08/11 16:45:37  geconomos
// Reworked Resize method to use gdiplus.
//
// Revision 3.5  2003/01/22 22:30:49  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 3.4  2002/03/11 21:49:47  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.3  2002/02/04 20:17:54  binli
// new Resize(): improved image quality in report file
//
// Revision 3.2.2.1  2002/02/11 16:37:26  binli
// Improved image quality in report file
//
// Revision 3.2  2002/01/07 21:44:10  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.1  2001/10/22 13:34:46  binli
// issue 1210: image resize when window has different size of W & H: set X/YOffset
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:30   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:35  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.9  2001/05/08 16:20:02  geconomos
// More coding standards
//
// Revision 1.8  2001/04/16 13:50:42  geconomos
// Coding standards(or there lack of)
//
// Revision 1.7  2001/03/21 16:30:47  geconomos
// Detected and fixed memory leak
//
// Revision 1.6  2001/02/07 16:24:51  geconomos
// Added revisions to report file
//
// Revision 1.5  2001/02/05 14:29:27  geconomos
// Changed all class to use base class
//
// Revision 1.4  2001/02/01 16:34:32  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Image.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: Image.C,v 1.1 2005/08/05 12:55:55 geconomos Exp $
