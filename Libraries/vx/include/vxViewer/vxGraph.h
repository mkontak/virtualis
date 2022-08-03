
// $Id: vxGraph.h,v 1.18 2007/08/02 19:43:55 romy Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)


#pragma once


// Includes
#include "vxElement.h"

class VX_VIEWER_DLL vxGraph : public vxElement
{
public:

	///Constructor
	vxGraph();
	
	///destructor
	virtual ~vxGraph();

	//Initialize the Graph method
	void Initialize( const int4 width, const int4 height);
	
	//Draw the image
	virtual void Draw(){/* If you need to create an image override this */;}

	///Reset the graphic properties
	virtual void ResetGraphics( ) ;

public:

	///Get Width
	int4 GetWidth() const { return m_width; }

	///Gets the Height
	int4 GetHeight() const { return m_height; }

	///Gets the stride
	int4 GetStride() const;

	///Gets the Bitmap data
	void GetBitmapData( Gdiplus::BitmapData & bitmapData );

	///Unlock the bitmap data
	void UnlockBitmapData( Gdiplus::BitmapData & bitmapData );

	///Get the graph image
	Gdiplus::Bitmap* GetGraphImage() const { return m_pImage; }

	///Set the graph image
	void SetGraphImage( Gdiplus::Bitmap * pImage );

	///Save the image to a file
	void SaveToFile( const std::string & filename ) const;

	///Sets the font name
	void SetFont(const std::string fontName = "Arial" , int4 fontSize = 8 ) { m_font = fontName; m_fontSize = fontSize; }

	///Sets the font color
	virtual void SetFontColor( Gdiplus::Color color = Gdiplus::Color::White ) 
														{ m_fontColor = color; }

	///Sets the pen
	virtual void SetPen( const int4 width = 2, Gdiplus::Color color = Gdiplus::Color::Blue ) 
															{	m_iPenWidth  = width;	m_penColor   = color;	}  

	///sets the Background color
	virtual void SetBackground( Gdiplus::Color color = Gdiplus::Color::Black ) const;


protected:
	
	///Get encoder class id
	int GetEncoderClsid( const WCHAR* format, CLSID* pClsid )const;

	///implement only if you need
	vxGraph( const vxGraph & other ){}

protected:
	/// Width property
	int4 m_width;

	///Height property
	int4 m_height;

	//Drawing properties
	///Pen width
	int4 m_iPenWidth;
	
	///Pen color
	Gdiplus::Color m_penColor;

	///Bitmap object
	Gdiplus::Bitmap * m_pImage;

	///Graphics object
	Gdiplus::Graphics * m_pGraphics; 

	///Font string.
	std::string m_font; 

	///Font size
	int4 m_fontSize;

	///Pen color
	Gdiplus::Color m_fontColor;

};


// $Log: vxGraph.h,v $
// Revision 1.18  2007/08/02 19:43:55  romy
// added SetImage method
//
// Revision 1.17  2007/07/19 13:50:11  romy
// modified lock and unlock bitmapdata methods
//
// Revision 1.16  2007/03/07 18:06:10  geconomos
// code review
//
// Revision 1.15  2006/11/17 22:37:45  romy
// text font size changed to Yellow
//
// Revision 1.14  2006/11/13 05:15:17  romy
// comments added
//
// Revision 1.13  2006/11/13 04:51:10  romy
// Data structure modified for SetData method also made optimizations
//
// Revision 1.12  2006/11/06 21:40:07  romy
// added GetBitmap data methods
//
// Revision 1.11  2006/11/02 22:43:41  romy
// increased font size
//
// Revision 1.10  2006/11/02 22:40:19  romy
// code optimization
//
// Revision 1.9  2006/11/02 21:45:05  romy
// Initial version - comments added
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxGraph.h,v 1.18 2007/08/02 19:43:55 romy Exp $
// $Id: vxGraph.h,v 1.18 2007/08/02 19:43:55 romy Exp $
