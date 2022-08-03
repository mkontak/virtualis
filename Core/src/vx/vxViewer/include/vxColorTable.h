// $Id: vxColorTable.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxColorTable.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Wei Li (liwei@viatronix.com)

/**
 *  colormap table
 */

#ifndef __vxColormap_h
#define __vxColormap_h


// includes
#include "enums.h"


// forward declarations
class vxColormap;


// class definition
class VX_VIEWER_DLL vxColorTable
{
// functions
public:

  /// default constructor
  vxColorTable() { m_bActiveRange = true; }
  
  /// copy constructor
  vxColorTable(const vxColorTable & other);

  /// assignment operator
  const vxColorTable & operator=(const vxColorTable & other);

  /// set active range
  void SetActiveRange(bool bFlag) { m_bActiveRange = bFlag; }

  /// is active range enabled
  bool IsActiveRange() { return m_bActiveRange; }

	/// update lookup table according to the colorCurves
	void UpdateLookupTable(vxColormap & colormap);

	/// return the number of entries of the specified color channel
	uint2 GetMaxIndex(nsCOLOR::COLOR_CHANNEL colorIndex);

	/// return the maximal possible value of the specified color channel
	uint2 GetMaxValue(nsCOLOR::COLOR_CHANNEL colorIndex);
	
  /// return the value of the specifice channel at position index
  virtual const uint2 GetColor(const nsCOLOR::COLOR_CHANNEL color, const uint2 index) = 0;

  /// set the value of the specifice channel at position index
  virtual void SetColor(const nsCOLOR::COLOR_CHANNEL color, const uint2 index, const uint2 value) = 0;

protected:	

  /// convert the colormap to discrete color values for a single channel
	void BuildTable(vxColormap & colormap, nsCOLOR::COLOR_CHANNEL colorIndex);

  /// convert the colormap to discrete color values for a single channel
  void BuildTables(vxColormap & colormap);

// data
protected:

  /// dirty flag
  bool m_bDirty;

  /// which range to map the color to: active or full?
  bool m_bActiveRange;

  /// number of entries in alpha channel
	uint2 m_uMaxAlphaIndex;

	/// maximal possible alpha value 
	uint2 m_uMaxAlphaValue;

	/// number of entries in each of the RGB channel
	uint2 m_uMaxColorIndex;

	/// maximal possible RGB value
	uint2 m_uMaxColorValue;
};


#endif // vxColormap_h


// Revision History:
// $Log: vxColorTable.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2003/11/16 23:45:52  michael
// Added alternative method of scan converting the piece-wise linear colormap
// functions. For each bin that is fully "covered" by one linear segment, the
// values are incrementally computed. For all the others, the color is averaged
// and using opacity weighted colors. This solves the issue of aliasing artifacts
// as created by the other approach.
//
// Revision 1.18  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.17  2003/06/04 18:33:04  michael
// tookout namespace declaration, should not be in header file!!!
//
// Revision 1.16  2003/05/16 11:45:01  michael
// code review
//
// Revision 1.15  2003/05/12 15:28:01  frank
// spelling
//
// Revision 1.14  2003/04/25 19:16:09  dongqing
// code review
//
// Revision 1.13  2003/04/19 01:53:17  michael
// some more code review
//
// Revision 1.12  2003/04/16 21:29:30  michael
// fixed default value for active/full range
//
// Revision 1.11  2003/04/16 17:28:18  michael
// active and full range are now working
//
// Revision 1.10  2003/04/16 12:47:27  michael
// performed initial code review
//
// Revision 1.9  2003/03/30 20:58:31  michael
// formatting and cosmetics
//
// Revision 1.8  2002/10/02 22:49:24  frank
// Removed promotion of namespaces in header files!
//
// Revision 1.7  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.6  2002/09/24 16:58:04  michael
// cleaned-up colortable and colormap ... hopefully we'll be able to remove
// member colortable from colormap.
//
// Revision 1.5  2002/09/19 14:25:24  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.4  2002/09/17 13:00:22  michael
// adustments for changes in transfer function
//
// Revision 1.3  2002/09/10 22:58:30  frank
// Cleaned up conversion from color&opacity to window/level range.
//
// Revision 1.2  2002/08/14 22:46:31  wenli
// initial version - Vascular
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 3.7  2002/07/24 15:28:19  ingmar
// code beautify
//
// Revision 3.6  2002/07/18 15:18:53  ingmar
// removed obsolete Initialize()
// added destructor to clean up memory leak
//
// Revision 3.5  2002/05/07 18:32:09  frank
// Cleaned up includes.
//
// Revision 3.4  2002/05/03 20:29:29  soeren
// BuildLookupTable takes const TF.
//
// Revision 3.3  2002/04/25 01:39:57  michael
// no more VLI table, only ColorTableCpu!
//
// Revision 3.2  2001/12/18 17:14:44  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.1  2001/11/06 02:56:33  liwei
// Modified for rendering tag volume.
//
// Revision 3.0  2001/10/14 23:02:30  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.3   Oct 12 2001 17:59:44   liwei
// Added control point list to the transfer function class according to the requirement of the transfer function editor.
// 
//    Rev 2.2   Sep 27 2001 18:15:28   liwei
// RendererTexMap uses its own colorTable, instead of one inside the transferFunction. Therefore we could compensate for different sampling distance owing to different viewing angle and different interpolation factor byscale the lookup table.
// 
//    Rev 2.1   Sep 27 2001 14:40:44   liwei
// Added gradient opacity modulation to RendererVolumePro.The computation of colortable reversed to the previous form to make VolumrPro to run properly. There may be bugs in software raycaster.
// 
//    Rev 2.0   Sep 16 2001 23:49:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:22   ingmar
// Initial revision.
// Revision 1.12  2001/08/23 21:40:21  liwei
// {Set, Get}{Red, Green, Blue, Alpha}() are changed to inline functions while
// {Set, Get}Color() changed to virtual functions.
//
// Revision 1.11  2001/08/06 16:03:10  liwei
// GLColorTable renamed to ColorTableGL.
// VLIColorTable renamed to ColorTableVLI.
//
// Revision 1.10  2001/08/03 23:42:51  liwei
// TransferFunction now contains several lookup tables as its data members.
//
// Revision 1.9  2001/08/03 22:40:59  liwei
// Added virtual function AdjustRange().
//
// Revision 1.8  2001/07/30 15:55:42  liwei
// ColorCurves renamed to TransferFunction.
//
// Revision 1.7  2001/07/12 22:05:15  liwei
// Moved inline functions of ColorTable ColorTable.inl.
//
// Revision 1.6  2001/07/12 20:55:26  liwei
// ApplyColorTable() need to be called from outside, so it is now public.
//
// Revision 1.5  2001/07/11 15:25:32  liwei
// Revised for the compliance of coding standard.
//
// Revision 1.4  2001/07/10 22:32:56  liwei
// ColorTable separated to ColorTable ColorTableGL
//
// Revision 1.3  2001/07/10 20:16:28  liwei
// Added to ZeroOne to handle variables of the value in [0, 1]
//
// Revision 1.2  2001/07/10 00:07:10  liwei
// Coordinates of the points in TransferFunction are scaled to the range of [0, 1].
//
// Revision 1.1  2001/06/29 16:53:08  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/26 20:27:14  liwei
// Code restructured.
//
// Revision 1.3  2001/06/25 21:21:45  geconomos
// added default return to GetColor()
//
// Revision 1.2  2001/06/20 20:00:47  liwei
// Bugs in ColorTable SetRed() through SetAlpha() fixed
//
// Revision 1.1  2001/06/13 14:28:04  liwei
// Added to CVS
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxColorTable.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
// $Id: vxColorTable.h,v 1.1.1.1 2004/03/02 03:58:41 geconomos Exp $
