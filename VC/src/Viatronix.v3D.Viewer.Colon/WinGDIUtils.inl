// $Id: WinGDIUtils.inl,v 1.3 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: WinGDIUtils.inl
// Owner(s): Jeffrey Meade  jmeade@viatronix.net
//

#ifndef WINDOWS_GDI_UTILS_INL
#define WINDOWS_GDI_UTILS_INL


//////////////////////////////////////////////////////////////////////////////////////
// BrushAndPen


/**
 * Create brush and pen with color and penwidth
 * 
 * @param color        Color for brush and pen
 * @param iPenwidth    Pen width
 */
void BrushAndPen::Create(const COLORREF & color, const int & iPenwidth)
{
  m_brush.CreateSolidBrush(color); m_pen.CreatePen(PS_SOLID, iPenwidth, color);
} // Create()


/**
 * Destructor 
 */
BrushAndPen::~BrushAndPen()
{
  m_brush.DeleteObject(); m_pen.DeleteObject();
} // destructor


/**
 * Creates a pattern brush based on bitmap, and a pen of specified width
 * 
 * @param bmp         Pattern bitmap for brush
 * @param iPenwidth   Pen width
 */
void BrushAndPen::Create(CBitmap & bmp, const int & iPenwidth)
{
  m_brush.CreatePatternBrush(&bmp);
  m_pen.CreatePen(PS_NULL, iPenwidth, COLORREF(0x888888));
} // Create()


/**
 * Selects brush AND pen into specified device context
 * 
 * @param pDC   Device context to select in to
 */
void BrushAndPen::Select(CDC * pDC)
{
  pDC->SelectObject(&m_brush); pDC->SelectObject(&m_pen);
} // Select()

//////////////////////////////////////////////////////////////////////////////////////
// BitmapDC


/**
 * Default constructor 
 * @param pDC   Device context to create bitmap for
 */
BitmapDC::BitmapDC(CDC * pDC /* = NULL */) : m_bmp(m_privateBmp), m_previousBitmap( NULL )
{
  Reset();
  if (pDC != NULL)
  {
    CreateBmpDC(pDC);
  }
} // constructor


/**
 * Constructor with external bitmap
 * @param bmp   bitmap source
 * @param pDC   device context
 */
BitmapDC::BitmapDC(CBitmap& bmp, CDC * pDC /* = NULL */) : m_bmp(bmp), m_previousBitmap( NULL )
{
  Reset();
  if (pDC != NULL)
  {
    CreateBmpDC(pDC);
  }
} // constructor


/**
 * Destructor 
 */
BitmapDC::~BitmapDC()
{
  Reset();
} // destructor


/**
 * Resets bitmap and DC
 */
void BitmapDC::Reset()
{
  if( m_previousBitmap )
  {
    m_dc.SelectObject( m_previousBitmap );
    m_previousBitmap = NULL;
  }
  
  m_privateBmp.DeleteObject(); 
  m_dc.DeleteDC(); 
  memset(&m_info, 0, sizeof(BITMAP));
} // Reset()


/**
 * True if both bitmap and DC are valid
 * 
 * @return   True if valid
 */
bool BitmapDC::IsValid()
{
  return ((HBITMAP(m_bmp)!=NULL) && (HDC(m_dc)!=NULL));
} // IsValid()


/**
 * Creates compatible bitmap and DC, selects bitmap into DC
 * 
 * @param pDC   Source DC for creation
 */
void BitmapDC::CreateBmpDC(CDC * pDC)
{
  m_dc.DeleteDC(); 
  m_dc.CreateCompatibleDC(pDC);
  StoreInfo();
  m_previousBitmap = m_dc.SelectObject(&m_bmp);
} // CreateBmpDC()


/**
 * Loads a bitmap from the current application resource file and stores related information.
 *
 * @param uBitmapResID   ID of bitmap resource.
 * @return               Success of bitmap load.
 */
bool BitmapDC::LoadBitmap(const uint4 uBitmapResID)
{
  if (m_bmp.LoadBitmap(uBitmapResID))
  {
    StoreInfo();
    return true;
  }
  return false;
} // LoadBitmap()


/**
 * Store info for (an already-valid) bitmap
 */
void BitmapDC::StoreInfo()
{
  m_bmp.GetBitmap(&m_info);
} // StoreInfo()


/**
 * Creates a bitmap to fit a specified window
 * 
 * @param pWnd   Window to fit bitmap and DC
 * @param src    Source bitmap and DC
 * @return       Indicates success/failure.
 */
bool BitmapDC::StretchFit(CWnd * pWnd, BitmapDC & src)
{
  CClientDC winDC(pWnd);
  CRect winRect;  pWnd->GetClientRect(winRect);
  return StretchFit(winDC, winRect, src);
} // StretchFit()


/**
 * Creates a bitmap and DC to fit specified rectangle
 * 
 * @param winDC      Source DC
 * @param destRect   Bitmap dimensions
 * @param src        Source bitmap and DC
 * @return           Indicates success/failure.
 */
bool BitmapDC::StretchFit(CDC & winDC, const CRect & destRect, BitmapDC & src)
{
  // Update the source bitmap information
  src.StoreInfo();

  // initialize the stretched bitmap
  Reset();
  if (!m_bmp.CreateCompatibleBitmap(&winDC, destRect.Width(), destRect.Height()))
  {
    return false;
  }

  // initialize the stretched DC
  CreateBmpDC(&winDC);

  // stretch it!
  m_dc.SetStretchBltMode(HALFTONE);
  const bool bSuccess(m_dc.StretchBlt(0,0,destRect.Width(),destRect.Height(),
    &src.m_dc, 0,0, src.m_info.bmWidth, src.m_info.bmHeight,  SRCCOPY) == TRUE);

  // Update the destination bitmap information
  StoreInfo();

  return bSuccess;
} // StretchFit()

//////////////////////////////////////////////////////////////////////////////////////
//

/**
 * Resizes a bitmap.
 *
 * @param pDC         Valid device context.
 * @param pBuffer     Original bitmap.
 * @param origSize    Original bitmap size.
 * @param newSize     Requested size for resized bitmap .
 * @param resizeBDC   [In] Initialized BDC, [OUT] The resized bitmap.
 */
inline void ResizeBmp(CDC * pDC, LPBYTE pBuffer, const Point2D<uint4>& origSize, const Point2D<uint4>& newSize, BitmapDC& resizeBDC)
{
  static const uint1 uBytesPerPixel(3);
  BITMAPINFO bmpInfo;
  memset(&bmpInfo, 0, sizeof(bmpInfo));
  bmpInfo.bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
  bmpInfo.bmiHeader.biPlanes     = 1;
  bmpInfo.bmiHeader.biBitCount   = uBytesPerPixel*8;
  bmpInfo.bmiHeader.biWidth      = origSize.m_x;
  bmpInfo.bmiHeader.biHeight     = origSize.m_y;
  bmpInfo.bmiHeader.biSizeImage  = origSize.m_x * origSize.m_y;
  Gdiplus::Bitmap origImage(&bmpInfo, static_cast<void*>(pBuffer));

  HBITMAP hBmp;
  origImage.GetHBITMAP(Gdiplus::Color(0,0,0), &hBmp);
  CBitmap * pOrigBmp = CBitmap::FromHandle(hBmp);

  BitmapDC origBDC(*pOrigBmp, pDC);
  resizeBDC.StretchFit(*pDC, CRect(0,0,newSize.m_x,newSize.m_y), origBDC);

  origBDC.Reset();
  DeleteObject(hBmp);
} // ResizeBmp()

#endif // if _WINDOWS_GDI_UTILS_INL

// Revision History:
// $Log: WinGDIUtils.inl,v $
// Revision 1.3  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.2  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.1.4.1  2006/05/09 13:56:17  geconomos
// restoring old bitmap in class BitmapDC
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.4.2.1  2004/03/22 21:46:22  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 3.4  2003/01/03 22:41:02  jmeade
// ResizeBmp method to store/resize a bitmap buffer.
//
// Revision 3.3  2002/07/03 03:23:22  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.2.2.1  2002/07/01 19:53:45  jmeade
// Issue 2521:  Extended BitmapDC class usability by allowing use of an external
// (e.g. already initialized) bitmap.
//
// Revision 3.2  2001/12/22 00:24:55  jmeade
// StretchFit to indicate success.
//
// Revision 3.1  2001/11/20 02:57:34  jmeade
// Load bitmap convenience function.
//
// Revision 3.0  2001/10/14 23:01:52  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:48   ingmar
// Initial revision.
// Revision 1.4  2001/05/11 23:13:50  jmeade
// Coding conventions
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WinGDIUtils.inl,v 1.3 2007/03/07 22:34:05 jmeade Exp $
// $Id: WinGDIUtils.inl,v 1.3 2007/03/07 22:34:05 jmeade Exp $
