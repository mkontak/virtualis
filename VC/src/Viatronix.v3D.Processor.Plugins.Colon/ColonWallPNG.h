// $Id: ColonWallPNG.h,v 1.2 2006/02/03 19:27:53 frank Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Bin Li

// pragmas
#pragma once


// includes
#include "V3DFile.h"


class ColonWallPNG
{
public:

  ColonWallPNG();

  ColonWallPNG(Triple<float4> & rUnits, std::string & rFileNameRoot);

  virtual ~ColonWallPNG();

	// create no scaled PNG header.
	void CreatePNGHeader();

	// scale the PNG header according to the units and write it out.
	void CreatePNGImage();

private:

  /// Image processing: scaling.
	uint1 * ImageScaling(int4 oldX, int4 oldY, int4 newX, int4 newY, float4 sx, float4 sy, uint1 * InputImage);

// Data members.
//--------------

public:

  /// buffer of PNG images.
	uint1 * m_puRGBImage[6];

	/// scaled PNG image;
	uint1 * m_puScaledRGBImage[6];
	
	/// depth value from different diretion.
	float4 * m_pfXYZPlaneDepthBuf[6];
	
	/// segment Ids.
	uint1 * m_puXYZSegIdBuf[6];

	/// normals.
	Triple<float4> * m_pXYZNormalBuf[6];

private:
	
  /// png header.
	V3DFile m_triHeader;

	/// units of the data volume.
	Triple<float4> m_Units;
	
	/// root of file name.
	std::string m_sFileNameRoot;

	/// segment colors.
	Triple<uint1> m_uColors[15];

}; // class ColonWallPNG


// $Log: ColonWallPNG.h,v $
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
//    Rev 2.0   Sep 16 2001 23:38:58   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:30   ingmar
// Initial revision.
// Revision 1.6  2001/05/30 15:43:31  binli
// code conventions
//
// Revision 1.5  2001/04/12 14:38:47  dmitry
// Sec_j_tri base class added
//
// Revision 1.4  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.3  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.2  2001/03/23 20:44:43  binli
// no message
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ColonWallPNG.h,v 1.2 2006/02/03 19:27:53 frank Exp $
// $Id: ColonWallPNG.h,v 1.2 2006/02/03 19:27:53 frank Exp $
