// $Id: ColonWallPNG.C,v 1.2 2006/02/03 19:27:53 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Bin Li

// Description: Create PNG header of the colon wall.

// includes
#include "stdafx.h"
#include "ColonWallPNG.h"
#include "Image.h"


ColonWallPNG::ColonWallPNG()
{
}


ColonWallPNG::ColonWallPNG(Triple<float4> & rUnits, std::string & rFileNameRoot)
{
	m_Units = rUnits;
	m_sFileNameRoot = rFileNameRoot;

	// initialize m_uColors for each segment.
	m_uColors[0].m_x =   0;  m_uColors[0].m_y =   0;  m_uColors[0].m_z =   0;
  m_uColors[1].m_x = 255;  m_uColors[1].m_y =   0;  m_uColors[1].m_z =   0;
  m_uColors[2].m_x =   0;  m_uColors[2].m_y = 255;  m_uColors[2].m_z =   0;
  m_uColors[3].m_x =   0;  m_uColors[3].m_y =   0;  m_uColors[3].m_z = 255;  
	m_uColors[4].m_x = 255;  m_uColors[4].m_y = 255;  m_uColors[4].m_z =   0;
	m_uColors[5].m_x =   0;  m_uColors[5].m_y = 255;  m_uColors[5].m_z = 255;
  m_uColors[6].m_x = 255;  m_uColors[6].m_y = 255;  m_uColors[6].m_z = 255;
  m_uColors[7].m_x = 150;  m_uColors[7].m_y = 150;  m_uColors[7].m_z = 150;
  m_uColors[8].m_x = 200;  m_uColors[8].m_y = 200;  m_uColors[8].m_z = 200;
  m_uColors[9].m_x = 250;  m_uColors[9].m_y = 250;  m_uColors[9].m_z = 250;
  m_uColors[10].m_x = 50;  m_uColors[10].m_y = 50;  m_uColors[10].m_z = 50;
	m_uColors[11].m_x = 255; m_uColors[11].m_y =  0;  m_uColors[11].m_z = 255;
	m_uColors[12].m_x = 155; m_uColors[12].m_y = 55;  m_uColors[12].m_z =  55;
	m_uColors[13].m_x = 55;  m_uColors[13].m_y = 255; m_uColors[13].m_z = 155;
	m_uColors[14].m_x = 0;   m_uColors[14].m_y = 255; m_uColors[14].m_z = 155;

	// initialize.
	int4 i;
	for (i=0; i<3; i++) 
	{
		m_pfXYZPlaneDepthBuf[i] = new float4 [256*256];
		m_puXYZSegIdBuf[i] = new uint1 [256*256];
		m_pXYZNormalBuf[i] = new Triple<float4> [256*256];
		for (int4 j=0; j<256*256; j++) 
		{
			*(m_pfXYZPlaneDepthBuf[i] + j) = -1.0f;
			*(m_puXYZSegIdBuf[i] + j) = 0xff;
		}
	} // end-for

	for (; i<6; i++)
	{
		m_pfXYZPlaneDepthBuf[i] = new float4 [256*256];
		m_puXYZSegIdBuf[i] = new uint1 [256*256];
		m_pXYZNormalBuf[i] = new Triple<float4> [256*256];
		for (int4 j=0; j<256*256; j++) {
			*(m_pfXYZPlaneDepthBuf[i] + j) = 257.0f;
			*(m_puXYZSegIdBuf[i] + j) = 0xff;
		}
	} // end-for
} // constructor::ColonWallPNG()


ColonWallPNG::~ColonWallPNG()
{
}


/*
 * create PNG header file.
 */
void ColonWallPNG::CreatePNGHeader()
{
	// prepare PNG preview header.
	Triple<uint1> vColor;

	// initialize.
	int4 x;
	for (x=0; x<6; x++)
	{
		m_puRGBImage[x] = new uint1 [256*256*3];	// RGB
		for (int4 y=0; y<256*256*3; y++)
			*(m_puRGBImage[x] + y) = 0x0;
	}

	for  (x=0; x<256; x++)
		for (int4 y=0; y<256; y++) 
		{
			// Preview from Z-direction. 
			if (*(m_puXYZSegIdBuf[2] + y*256 + x) != 0xff) 
			{
				uint1 * puPtr = m_puRGBImage[4] + y*256*3 + x*3;
				*puPtr		 = (uint1)(m_uColors[*(m_puXYZSegIdBuf[2] + y*256 + x)].m_x * (*(m_pXYZNormalBuf[2]+y*256+x)).m_z);
				*(puPtr+1) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[2] + y*256 + x)].m_y * (*(m_pXYZNormalBuf[2]+y*256+x)).m_z);
				*(puPtr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[2] + y*256 + x)].m_z * (*(m_pXYZNormalBuf[2]+y*256+x)).m_z);
			}
			// Preview from Y-direction. 
			if (*(m_puXYZSegIdBuf[1] + y*256 + x) != 0xff) 
			{
				uint1 * puPtr = m_puRGBImage[2] + y*256*3 + x*3;
				*puPtr     = (uint1)(m_uColors[*(m_puXYZSegIdBuf[1] + y*256 + x)].m_x * (*(m_pXYZNormalBuf[1]+y*256+x)).m_y);
				*(puPtr+1) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[1] + y*256 + x)].m_y * (*(m_pXYZNormalBuf[1]+y*256+x)).m_y);
				*(puPtr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[1] + y*256 + x)].m_z * (*(m_pXYZNormalBuf[1]+y*256+x)).m_y);
			}
			// Preview from X-direction. 
			if (*(m_puXYZSegIdBuf[0] + y*256 + x) != 0xff) 
			{
				uint1 * puPtr = m_puRGBImage[0] + y*256*3 + x*3;
				*puPtr		 = (uint1)(m_uColors[*(m_puXYZSegIdBuf[0] + y*256 + x)].m_x * (*(m_pXYZNormalBuf[0]+y*256+x)).m_x);
				*(puPtr+1) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[0] + y*256 + x)].m_y * (*(m_pXYZNormalBuf[0]+y*256+x)).m_x);
				*(puPtr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[0] + y*256 + x)].m_z * (*(m_pXYZNormalBuf[0]+y*256+x)).m_x);
			}
			// Preview from negative Z-direction. 
			if (*(m_puXYZSegIdBuf[5] + y*256 + x) != 0xff) 
			{
				uint1 * puPtr = m_puRGBImage[5] + y*256*3 + x*3;
				*puPtr		 = (uint1)(m_uColors[*(m_puXYZSegIdBuf[5] + y*256 + x)].m_x * (-(*(m_pXYZNormalBuf[5]+y*256+x)).m_z));
				*(puPtr+1) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[5] + y*256 + x)].m_y * (-(*(m_pXYZNormalBuf[5]+y*256+x)).m_z));
				*(puPtr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[5] + y*256 + x)].m_z * (-(*(m_pXYZNormalBuf[5]+y*256+x)).m_z));
			}
			// Preview from negative Y-direction. 
			if (*(m_puXYZSegIdBuf[4] + y*256 + x) != 0xff) 
			{
				uint1 * puPtr = m_puRGBImage[3] + y*256*3 + x*3;
				*puPtr     = (uint1)(m_uColors[*(m_puXYZSegIdBuf[4] + y*256 + x)].m_x * (-(*(m_pXYZNormalBuf[4]+y*256+x)).m_y));
				*(puPtr+1) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[4] + y*256 + x)].m_y * (-(*(m_pXYZNormalBuf[4]+y*256+x)).m_y));
				*(puPtr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[4] + y*256 + x)].m_z * (-(*(m_pXYZNormalBuf[4]+y*256+x)).m_y));
			}
			// Preview from negative X-direction. 
			if (*(m_puXYZSegIdBuf[3] + y*256 + x) != 0xff) 
			{
				uint1 * ptr = m_puRGBImage[1] + y*256*3 + x*3;
				*ptr     = vColor.m_x = (uint1)(m_uColors[*(m_puXYZSegIdBuf[3] + y*256 + x)].m_x * (-(*(m_pXYZNormalBuf[3]+y*256+x)).m_x));
				*(ptr+1) = vColor.m_y = (uint1)(m_uColors[*(m_puXYZSegIdBuf[3] + y*256 + x)].m_y * (-(*(m_pXYZNormalBuf[3]+y*256+x)).m_x));
				*(ptr+2) = (uint1)(m_uColors[*(m_puXYZSegIdBuf[3] + y*256 + x)].m_z * (-(*(m_pXYZNormalBuf[3]+y*256+x)).m_x));
			}
		} // end-for.
	
	// free memory.
	for (x=0; x<6; x++) 
	{
		delete [] m_pfXYZPlaneDepthBuf[x];
		delete [] m_puXYZSegIdBuf[x];
		delete [] m_pXYZNormalBuf[x];
	}
} // CreatePNGHeader()


/*
 * write PNG header to a image file.
 */
void ColonWallPNG::CreatePNGImage()
{
	int4 iNewSizeX = (int4)(256 * m_Units.m_x + 0.5);
	int4 iNewSizeY = (int4)(256 * m_Units.m_y + 0.5);
	int4 iNewSizeZ = (int4)(256 * m_Units.m_z + 0.5);
	int4 iWidth  = (iNewSizeX > iNewSizeY) ? iNewSizeX : iNewSizeY;
	int4 iHeight = (iNewSizeY > iNewSizeZ) ? iNewSizeY : iNewSizeZ;

	//m_triHeader = V3DFile( iWidth*3, iHeight*2 );
  Image< PixelRGB<uint1> > image( Triple<uint4>( iWidth*3, iHeight*2, 0 ), PixelRGB<uint1>( 0, 0, 0 ) );

	// set up background.
	int4 x;
	for (x=0; x<iWidth*3; x++)
		for (int4 y=0; y<iHeight*2; y++) 
		{
      Point2D<uint2> position( x, y );
      const PixelRGB<uint1> backgroundPixel( m_uColors[0].m_x, m_uColors[0].m_y, m_uColors[0].m_z );
			image.SetPixel( position, backgroundPixel );
		}
	
	// write out PNG header.
	// Positive X-direction.
	int4 iBaseX = 0;
	int4 iBaseY = 0;
	Triple< uint1 > vColor;
	m_puScaledRGBImage[0] = ImageScaling(256, 256, iNewSizeY, iNewSizeZ, m_Units.m_y, m_Units.m_z, m_puRGBImage[0]);
	int4 y;
	for (y=0; y<iNewSizeZ; y++)
		for (int4 x=0; x<iNewSizeY; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[0] + y*iNewSizeY*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[0] + y*iNewSizeY*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[0] + y*iNewSizeY*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY + (iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor (positive x).

	// negative X-direction.
	iBaseX = 0;
	iBaseY = iHeight;
	m_puScaledRGBImage[1] = ImageScaling(256, 256, iNewSizeY, iNewSizeZ, m_Units.m_y, m_Units.m_z, m_puRGBImage[1]);
	for (y=0; y<iNewSizeZ; y++)
		for (int4 x=0; x<iNewSizeY; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[1] + y*iNewSizeY*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[1] + y*iNewSizeY*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[1] + y*iNewSizeY*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY+(iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor (negative x).

	// Positive Y-direction.
	iBaseX = iWidth;
	iBaseY = 0;
	m_puScaledRGBImage[2] = ImageScaling(256, 256, iNewSizeX, iNewSizeZ, m_Units.m_x, m_Units.m_z, m_puRGBImage[2]);
	for (y=0; y<iNewSizeZ; y++)
		for (int4 x=0; x<iNewSizeX; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[2] + y*iNewSizeX*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[2] + y*iNewSizeX*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[2] + y*iNewSizeX*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY + (iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor (positive y).

	// Negative Y-direction.
	iBaseX = iWidth;
	iBaseY = iHeight;
	m_puScaledRGBImage[3] = ImageScaling(256, 256, iNewSizeX, iNewSizeZ, m_Units.m_x, m_Units.m_z, m_puRGBImage[3]);
	for (y=0; y<iNewSizeZ; y++)
		for (int4 x=0; x<iNewSizeX; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[3] + y*iNewSizeX*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[3] + y*iNewSizeX*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[3] + y*iNewSizeX*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY + (iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor (negative y).

	// Positive Z-direction.
	iBaseX = iWidth*2;
	iBaseY = 0;
	m_puScaledRGBImage[4] = ImageScaling(256, 256, iNewSizeX, iNewSizeY, m_Units.m_x, m_Units.m_y, m_puRGBImage[4]);
	for (y=0; y<iNewSizeY; y++)
		for (int4 x=0; x<iNewSizeX; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[4] + y*iNewSizeX*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[4] + y*iNewSizeX*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[4] + y*iNewSizeX*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY + (iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor(positive z)

	// Negative Z-direction.
	iBaseX = iWidth*2;
	iBaseY = iHeight;
	m_puScaledRGBImage[5] = ImageScaling(256, 256, iNewSizeX, iNewSizeY, m_Units.m_x, m_Units.m_y, m_puRGBImage[5]);
	for (y=0; y<iNewSizeY; y++)
		for (int4 x=0; x<iNewSizeX; x++) 
		{
			vColor.m_x = *(m_puScaledRGBImage[5] + y*iNewSizeX*3 + x*3);
			vColor.m_y = *(m_puScaledRGBImage[5] + y*iNewSizeX*3 + x*3 + 1);
			vColor.m_z = *(m_puScaledRGBImage[5] + y*iNewSizeX*3 + x*3 + 2);
      Point2D<uint2> position( iBaseX+x, iBaseY + (iHeight-1-y) );
      const PixelRGB<uint1> pixel( vColor.m_x, vColor.m_y, vColor.m_z );
			image.SetPixel( position, pixel );
		} // endfor(negative z)

	// write out preview header.
  std::string sFileName  = m_sFileNameRoot + ".tph";
  m_triHeader.SetImage( image );
	m_triHeader.Write(sFileName); // (PNG) preview header.
	for (x=0; x<6; x++) 
	{
		delete [] m_puRGBImage[x];
		delete [] m_puScaledRGBImage[x];
	}
}


/**
 * 2D Image scaling.
 *
 * @param oldSizeX, oldSizeY	the size of original image.
 * @param newSizeX, newSizeY	the size of scaled image.
 * @param sx, sy		factor for scaling.
 * @InputImage			orignal image.
 * @return					pointer to the scaled image.
 */
uint1 * ColonWallPNG::ImageScaling(int4 oldSizeX, int4 oldSizeY, 
																	 int4 newSizeX, int4 newSizeY, 
																	 float4 sx, float4 sy, uint1 * InputImage)
{
	uint1 * ScaledImage = new uint1 [newSizeX * newSizeY * 3];
	float4 value[3];

	int4 i;
	for (i=0; i<newSizeX*newSizeY*3; i++)
		* (ScaledImage+i) = 0x0;

	for (i=1; i<newSizeX-1; i++) 
		for (int4 j=1; j<newSizeY-1; j++) 
		{
			float4 oldCoordinateX = i/sx;
			float4 oldCoordinateY = j/sy;
			
			int4 cornerX = (int4) oldCoordinateX;
			int4 cornerY = (int4) oldCoordinateY;
			
			float4 dx = oldCoordinateX - cornerX;
			float4 dy = oldCoordinateY - cornerY;
			
			uint1 * ptr = InputImage + cornerY*oldSizeX*3 + cornerX*3;
			for (int4 k=0; k<3; k++, ptr++) 
			{
				value[k] = ((*ptr) * (1-dx) * (1-dy) + (*(ptr+3)) * dx * (1-dy) 
								 + (*(ptr+oldSizeX*3)) * (1-dx) * dy + (*(ptr+oldSizeX*3+3)) * dx * dy);
			} // end-for().
			ptr = ScaledImage + j*newSizeX*3 + i*3;
			* ptr = value[0];
			*(ptr+1) = value[1];
			*(ptr+2) = value[2];
		} // end-for (i)

	return ScaledImage;
} // ImageScaling().

// $Log: ColonWallPNG.C,v $
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1  2005/04/14 12:50:04  geconomos
// initial revision
//
// Revision 3.1  2003/01/24 18:01:55  dongqing
// solve conflict due to vxBase modification: Triple, Vector, Point ...
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:38:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:30   ingmar
// Initial revision.
// Revision 1.7  2001/08/29 16:30:23  sarang
// BUG_FIX: fixed scope of loop variable
// gcc/linux ok now
//
// Revision 1.6  2001/07/03 14:59:47  soeren
// removed stdafx.c and .h
//
// Revision 1.5  2001/05/30 15:43:31  binli
// code conventions
//
// Revision 1.4  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.3  2001/03/27 19:49:50  binli
// code walkthrough: name convension
//
// Revision 1.2  2001/03/23 20:44:43  binli
// no message
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ColonWallPNG.C,v 1.2 2006/02/03 19:27:53 frank Exp $
// $Id: ColonWallPNG.C,v 1.2 2006/02/03 19:27:53 frank Exp $
