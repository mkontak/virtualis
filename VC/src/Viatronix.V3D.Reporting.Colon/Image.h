// $Id: Image.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Image.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
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

#ifndef Image_h__
#define Image_h__

#include "resource.h" 
#include "Point2D.h" 

class ATL_NO_VTABLE Image : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<Image, &CLSID_Image>,
	public ISupportErrorInfo,
	public IDispatchImpl<IImage, &IID_IImage, &LIBID_ReportLib>
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGE)
DECLARE_NOT_AGGREGATABLE(Image)
DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(Image)
	COM_INTERFACE_ENTRY(IImage)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_THIS()
END_COM_MAP()

public:
	
  /// called when object is fully created
  HRESULT FinalConstruct();
  
  /// called when final reference to object has been released
  void FinalRelease();

  /// required interface method for ISupportErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  /// gets image size
  STDMETHOD(get_ImageSize)( long *piImageSize);
  
  /// gets image pixels as safearray
  STDMETHOD(get_Pixels)(VARIANT *pvPixels);
  
  /// gets image unique ID
  STDMETHOD(get_ID)(BSTR *pbsID);
  
	/// gets image BitsPerPixel
  STDMETHOD(get_BitsPerPixel)(short *piBpp);
	
  /// gets image height
  STDMETHOD(get_Height)(long *piHeight);
	
  /// gets image width
  STDMETHOD(get_Width)(long *piWidth);
	
  /// gets image as HBITMAP
  STDMETHOD(get_Bitmap)(long *piBitmap);
	
	/// creates a new image
  STDMETHOD(Create)(long iWidth, long iHeight, short iBpp);
	
	/// resizes an image
  STDMETHOD(Resize)(long iWidth, long iHeight);
	
	/// sets the pixels for an image
  STDMETHOD(SetPixels)(VARIANT vBytes);	

  /// object setup routine
  HRESULT Setup(CComPtr<IReportRevision>& spRevision, CComBSTR  bs);
  
  /// saves bitmap within a storage object
  HRESULT WriteBitmap(CComPtr<IStorage>& spStg);
 
  /// reads bitmap from a storage object
  HRESULT ReadBitmap(CComPtr<IStorage>& spStg);
  
private:
	
	/// sets the bimap internals
  HRESULT CreateDIB(LPBITMAPINFOHEADER pbih, uint1* lpBits);   
	
  /// frees bitmap
  void DeleteObject();

  // resets all class variables
  void Reset();
  
  /// aligns the bitmap width to a WORD boundary
  inline int4 BytesPerLine(int4 iWidth, int4 iBitsPerPixel);

	inline float4 Interpolate(const float4 & a, const float4 & b, const float4 & c, const float4 & d,const float4 & f);

  /// return RGB at given position
  inline Vector<float4> GetPixelSafe(const Point2D<uint4> & position) const;

  /// return RGB at given position (bounded)
  inline Vector<float4> GetPixelBound(const Point2D<uint4> & position) const;

	/// resample at given position
  inline Vector<float4> Resample(const Point2D<float4> & position) const;	

  /// handle to current bitmap
  HBITMAP	m_hBitmap;
  
  /// bitmap internals
  BITMAPINFO m_bi;
  
  /// quick access to image bits
  LPVOID m_ppvBits;
  
  /// quick access to image width
  int4 m_iWidth;
  
  /// quick access to image height
  int4 m_iHeight;
  
  /// parent revision object
  CComPtr<IReportRevision> m_spRevision;
  
  /// image's unique ID
  CComBSTR  m_bsID;
};

#include "Image.inl"

#endif

// $Log: Image.h,v $
// Revision 1.1  2005/08/05 12:55:55  geconomos
// moved from vx repository
//
// Revision 3.3  2003/01/22 22:31:04  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 3.2  2002/03/11 21:49:47  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.0.4.1  2002/02/11 16:37:26  binli
// Improved image quality in report file
//
// Revision 3.0  2001/10/14 23:01:55  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:36   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:30   ingmar
// Initial revision.
// Revision 1.1  2001/05/10 21:03:35  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.8  2001/05/08 16:20:02  geconomos
// More coding standards
//
// Revision 1.7  2001/04/16 13:50:42  geconomos
// Coding standards(or there lack of)
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.V3D.Reporting.Colon/Image.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
// $Id: Image.h,v 1.1 2005/08/05 12:55:55 geconomos Exp $
