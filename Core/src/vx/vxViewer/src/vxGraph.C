// $Id: vxGraph.C,v 1.19 2007/08/02 19:43:55 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


#include "stdafx.h"
#include "vxgraph.h"

#define FILE_REVISION "1.0"

/*
 * Constructor
 */
vxGraph::vxGraph() 
{
	m_pImage		= NULL;
	m_pGraphics = NULL;
}//vxGraph() 

/*
 * Initializes the Graph object
 */
void vxGraph::Initialize( const int4 width, const int4 height) 
{
	m_width  = width;
	m_height = height;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Create the Image object.
	delete m_pImage;
	delete m_pGraphics;
	m_pImage		= new Gdiplus::Bitmap( m_width, m_height, PixelFormat24bppRGB );
	m_pGraphics = new Gdiplus::Graphics( m_pImage );

	ResetGraphics();

}//Initialize( int4 width, int4 height)


/*
 * Destructor
 */
vxGraph::~vxGraph()
{
	delete m_pGraphics;
	delete m_pImage;
}///~vxGraph

/*
 * Saves the image to a bit map file
 */
void vxGraph::SaveToFile( const std::string & filename ) const
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	m_pImage->Save( std::wstring( filename.begin(), filename.end()).c_str(), &pngClsid, NULL);
}//SaveToFile( std::string filename )


/*
 * Get the stride number
 */
int4 vxGraph::GetStride() const
{
	// lock the down managed bitmap and grab a pointer to it's first scan line
	Gdiplus::Rect bounds ( 0, 0, m_width, m_height );
	Gdiplus::BitmapData bitmapData;

	int4 stride = 0;

	try
	{
		m_pImage->LockBits( &bounds, 0 , m_pImage->GetPixelFormat(), &bitmapData );
		stride = bitmapData.Stride;
		m_pImage->UnlockBits( &bitmapData );
	}
	catch ( ... )
	{
		throw ex::DataCorruptionException( LogRec( "Trouble to Lock Bits.", "GetStride", "vxGraph" ) );
	}

	return stride;
}//GetStride()


/*
 * Saves the Graphics objects such as pen, background etc to the original color
 */
void vxGraph::ResetGraphics() 
{
	SetPen();
	SetBackground();
	SetFontColor( Gdiplus::Color::Yellow );
	SetFont();

	m_pGraphics->SetSmoothingMode( Gdiplus::SmoothingModeHighQuality );
	m_pGraphics->SetInterpolationMode( Gdiplus::InterpolationModeHighQuality );
	m_pGraphics->SetCompositingQuality( Gdiplus::CompositingQualityHighQuality );

}//ResetGraphics()

/*
 * Sets the background color
 */
void vxGraph::SetBackground( Gdiplus::Color color ) const
{ 
	Gdiplus::SolidBrush brush( color );
	m_pGraphics->FillRectangle( &brush, Gdiplus::Rect( 0,0, m_width,m_height ));
}//SetBackground( Color color ) 



/*
 * Unlocks the bitmap data
 */
void vxGraph::UnlockBitmapData( Gdiplus::BitmapData & lockedBitmapData )
{
	if ( m_pImage == NULL )
		throw ex::NullPointerException( LogRec("Image data is an empty pointer", "UnlockBitmapData", "vxGraph") );

	m_pImage->UnlockBits( &lockedBitmapData );
}//UnlockBitmapData( void * p )const



/* 
 * Get the Bitmap Data as void pointer
 */
void vxGraph::GetBitmapData( Gdiplus::BitmapData & lockedBitmapData )
{
	if ( m_pImage == NULL )
		throw ex::NullPointerException( LogRec("Image data is an empty pointer", "GetBitmapVoidData", "vxGraph") );
				
  // lock the down managed bitmap and grab a pointer to it's first scan line
	Gdiplus::Rect bounds ( 0, 0, m_width, m_height );
	try
	{
		m_pImage->LockBits( &bounds, Gdiplus::ImageLockModeRead  , m_pImage->GetPixelFormat(), &lockedBitmapData );
	}
	catch ( ... )
	{
		throw ex::DataCorruptionException( LogRec( "Trouble to Lock Bits.", "GetBitmapData", "vxGraph" ) );
	}
}//GetBitmapData()const


/*
 *GetEncoderClsid : From microsoft
 */ 
int4 vxGraph::GetEncoderClsid(const WCHAR* format, CLSID* pClsid) const
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}//GetEncoderClsid(const WCHAR* format, CLSID* pClsid)


/*
 * Sets the Graph image
 */
void vxGraph::SetGraphImage( Gdiplus::Bitmap * pImage )
{
	m_pImage = pImage->Clone(0,0,m_width, m_height, pImage->GetPixelFormat() );

}//SetGraphImage( Gdiplus::Bitmap * pImage )


// $Log: vxGraph.C,v $
// Revision 1.19  2007/08/02 19:43:55  romy
// added SetImage method
//
// Revision 1.18  2007/07/19 13:50:11  romy
// modified lock and unlock bitmapdata methods
//
// Revision 1.17  2006/11/20 21:15:06  romy
// moved the Rotation to the managed part
//
// Revision 1.16  2006/11/17 22:37:30  romy
// text font size changed to Yellow
//
// Revision 1.15  2006/11/16 18:42:50  romy
// updated the Pixel formats
//
// Revision 1.14  2006/11/13 05:16:02  romy
// comments added
//
// Revision 1.13  2006/11/13 05:15:17  romy
// comments added
//
// Revision 1.12  2006/11/13 04:51:10  romy
// Data structure modified for SetData method also made optimizations
//
// Revision 1.11  2006/11/06 21:40:07  romy
// added GetBitmap data methods
//
// Revision 1.10  2006/11/02 22:40:19  romy
// code optimization
//
// Revision 1.9  2006/11/02 21:45:05  romy
// Initial version - comments added
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxGraph.C,v 1.19 2007/08/02 19:43:55 romy Exp $
// $Id: vxGraph.C,v 1.19 2007/08/02 19:43:55 romy Exp $


#undef FILE_REVISION
