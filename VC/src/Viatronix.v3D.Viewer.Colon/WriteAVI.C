// $Id: WriteAVI.C,v 1.4 2007/03/07 22:34:05 jmeade Exp $
//
// Copyright © 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author(s): Kevin Kreeger  kevin@viatronix.net


/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1996 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/****************************************************************************
 *
 *  WRITEAVI.C
 *
 *  Creates the file OUTPUT.AVI, an AVI file consisting of a rotating clock
 *  face.  This program demonstrates using the functions in AVIFILE.DLL
 *  to make writing AVI files simple.
 *
 *  This is a stripped-down example; a real application would have a user
 *  interface and check for errors.
 *
 ***************************************************************************/

// includes
#include "stdafx.h"
#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>
#include "writeavi.h"


/**
 * Makes a device independent bitmap.
 *
 * @param hbitmap   Input bitmap.
 * @param bits      Number of bits per pixel in bitmap.
 * @return          Output DIB.
 */
static HANDLE  MakeDib( HBITMAP hbitmap, UINT bits )
{
  HANDLE              hdib ;
  HDC                 hdc ;
  BITMAP              bitmap ;
  UINT                wLineLen ;
  DWORD               dwSize ;
  DWORD               wColSize ;
  LPBITMAPINFOHEADER  lpbi ;
  LPBYTE              lpBits ;
  
  GetObject(hbitmap,sizeof(BITMAP),&bitmap) ;

  //
  // DWORD align the width of the DIB
  // Figure out the size of the colour table
  // Calculate the size of the DIB
  //
  wLineLen = (bitmap.bmWidth*bits+31)/32 * 4;
  wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
  dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
    (DWORD)(UINT)wLineLen*(DWORD)(UINT)bitmap.bmHeight;

  //
  // Allocate room for a DIB and set the LPBI fields
  //
  hdib = GlobalAlloc(GHND,dwSize);
  if (!hdib)
    return hdib ;

  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib) ;

  lpbi->biSize = sizeof(BITMAPINFOHEADER) ;
  lpbi->biWidth = bitmap.bmWidth ;
  lpbi->biHeight = bitmap.bmHeight ;
  lpbi->biPlanes = 1 ;
  lpbi->biBitCount = (WORD) bits ;
  lpbi->biCompression = BI_RGB ;
  lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize ;
  lpbi->biXPelsPerMeter = 0 ;
  lpbi->biYPelsPerMeter = 0 ;
  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
  lpbi->biClrImportant = 0 ;

  //
  // Get the bits from the bitmap and stuff them after the LPBI
  //
  lpBits = (LPBYTE)(lpbi+1)+wColSize ;

  hdc = CreateCompatibleDC(NULL) ;

  GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

  // Fix this if GetDIBits messed it up....
  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

  DeleteDC(hdc) ;
  GlobalUnlock(hdib);

  return hdib ;
} // MakeDIB


/**
 * Constructor 
 *
 * @param lpszFileName   AVI filename.
 * @param xdim           X dimension of AVI frames.
 * @param ydim           Y dimension of AVI frames.
 */
WriteAVI::WriteAVI(LPCTSTR lpszFileName, int xdim, int ydim, int4 iFrameRate)
 :  m_uXDim(xdim), m_uYDim(ydim), m_bOK(true), m_uNumFrames(0)
{

  m_pAVIFile = NULL;
  m_pAVIStream = NULL;
  m_pAVIStreamCompressed = NULL;
  
  AVIFileInit();
  HRESULT hr = AVIFileOpen(&m_pAVIFile, lpszFileName, OF_WRITE | OF_CREATE, NULL);                
  if (hr != AVIERR_OK)
  {
    m_bOK = false;
    return;
  } // endif error on file open
    
  AVISTREAMINFO strhdr;
  _fmemset(&strhdr, 0, sizeof(strhdr));
  strhdr.fccType                = streamtypeVIDEO;// stream type
  strhdr.fccHandler             = 0;
  strhdr.dwScale                = 1;
  strhdr.dwRate                 = iFrameRate;
  strhdr.dwSuggestedBufferSize  = m_uXDim*m_uYDim*3;
  
  SetRect(&strhdr.rcFrame, 0, 0, m_uXDim, m_uYDim);
  
  // And create the stream;
  hr = AVIFileCreateStream(m_pAVIFile,  &m_pAVIStream, &strhdr);
  if (hr != AVIERR_OK)
  {
    m_bOK = false;
    return;
  } // endif error on create stream
  
  AVICOMPRESSOPTIONS opts;
  memset( &opts, 0, sizeof(AVICOMPRESSOPTIONS  ) );
  opts.fccHandler = mmioFOURCC('D', 'I', 'B', ' ');                     // DIB Codec
  opts.dwFlags = AVICOMPRESSF_VALID;                                    // has valid 

  hr = AVIMakeCompressedStream(&m_pAVIStreamCompressed, m_pAVIStream, &opts, NULL);
  if (hr != AVIERR_OK)
  {
    m_bOK = false;
    return;
  } // endif error on make stream
  
  BITMAPINFOHEADER hdr;
  memset(&hdr, 0, sizeof(BITMAPINFOHEADER));
  hdr.biSize = sizeof(BITMAPINFOHEADER);
  hdr.biWidth = m_uXDim;
  hdr.biHeight= m_uYDim;
  hdr.biPlanes = 1;
  hdr.biBitCount = 3*8;
  hdr.biSizeImage = m_uXDim*m_uYDim*3;
  
  hr = AVIStreamSetFormat(m_pAVIStreamCompressed, 0, &hdr, sizeof(hdr));
  if (hr != AVIERR_OK)
  {
    m_bOK = false;
    return;
  } //endif error on set format
    
} // Constructor



/**
 * Destructor 
 */
WriteAVI::~WriteAVI()
{
  Close();
} // Destructor



/**
 * Closes AVI stream.
 */
void WriteAVI::Close()
{
  if (m_pAVIStream) AVIStreamClose(m_pAVIStream);

  if (m_pAVIStreamCompressed) AVIStreamClose(m_pAVIStreamCompressed);

  if (m_pAVIFile) 
  {
    AVIFileClose(m_pAVIFile);
    AVIFileExit();
  }

  m_pAVIFile = NULL;
  m_pAVIStream = NULL;
  m_pAVIStreamCompressed = NULL;
} // Destructor


/**
 * Adds <iNumFrames> of the given bitmap.
 *
 * @param pdata        Pointer to movie frame bitmap data.
 * @param iNumFrames   Number of frames this will occupy.
 * @return             Success/failure.
 */
bool WriteAVI::AddFrame(void * pdata, const uint4 iNumFrames)
{
  if (!m_bOK) return false;

  // flip RGB
  unsigned char * pImage = (unsigned char *)pdata;
  for(int i=0;i<m_uXDim*m_uYDim;i++)
  {
    std::swap(pImage[i*3], pImage[i*3+2]);
  } // endfor all pixels to flip

  HRESULT hr = AVIStreamWrite(m_pAVIStreamCompressed, m_uNumFrames, 1, pdata, (m_uXDim*m_uYDim*3), AVIIF_KEYFRAME, NULL, NULL);
  if(hr != AVIERR_OK)
  {
    m_bOK = false;
    return false;
  } // error on write

  m_uNumFrames += iNumFrames;
  return true;
} // AddFrame


/**
 * Adds <iNumFrames> of the given bitmap.
 *
 * @param bmp          Movie frame bitmap data.
 * @param iNumFrames   Number of frames this will occupy.
 * @return             Success/failure.
 */
bool WriteAVI::AddFrame(const CBitmap & bmp, const uint4 iNumFrames)
{
  if (!m_bOK) return false;

  LPBITMAPINFOHEADER pAlpbi = (LPBITMAPINFOHEADER)GlobalLock(MakeDib(bmp, 24));
  if (pAlpbi == NULL) return false;
  
  if ((m_uXDim != pAlpbi->biWidth) || (m_uYDim != pAlpbi->biHeight) )
  {
    GlobalFreePtr(pAlpbi);
    return false;
  }

  HRESULT hr = AVIStreamWrite(m_pAVIStreamCompressed, m_uNumFrames, 1,
                              (LPBYTE) pAlpbi + pAlpbi->biSize + pAlpbi->biClrUsed * sizeof(RGBQUAD),
                              pAlpbi->biSizeImage,  AVIIF_KEYFRAME, NULL, NULL);

  if (hr != AVIERR_OK)
  {
    GlobalFreePtr(pAlpbi);
    m_bOK = false;
    return false;
  }

  GlobalFreePtr(pAlpbi);
  m_uNumFrames += iNumFrames;
  return true;
} // AddFrame()


// $Log: WriteAVI.C,v $
// Revision 1.4  2007/03/07 22:34:05  jmeade
// code standards.
//
// Revision 1.3  2006/01/31 14:29:16  frank
// code review
//
// Revision 1.2  2005/10/28 13:28:28  geconomos
// code cleanup
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.7  2002/01/09 03:47:03  jmeade
// Standards.
//
// Revision 1.6  2001/12/19 20:51:43  jmeade
// Tabs to spaces.
//
// Revision 1.5  2001/12/12 20:17:25  jmeade
// Allow specification of number of frames in AddFrame(); Comments, standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/WriteAVI.C,v 1.4 2007/03/07 22:34:05 jmeade Exp $
// $Id: WriteAVI.C,v 1.4 2007/03/07 22:34:05 jmeade Exp $
