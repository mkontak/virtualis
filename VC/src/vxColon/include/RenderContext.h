// $Id: RenderContext.h,v 1.2 2006/01/31 14:04:55 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: A class encapsulating data for volume rendering
// Created: Jan. 24, 2001
// Author: Wei Li (liwei@viatronix.net)


// pragmas
#pragma once


// includes
#include "Typedef.h"
#include "Volume.h"
#include "TransferFunctionVC.h"


// namespaces
namespace ReaderLib
{


// forwards
class Dataset;


class RenderContext
{
// Member Functions
public:

  /// Constructor
  RenderContext();

  /// Destructor
  virtual ~RenderContext();

  /// Clear the imageBuffer only (the depth buffer is not cleared for performance reason)
  void ClearBuffer();

  /// Set the size of display
  void SetDisplaySize(const uint4 uDisplayWidth, const uint4 uDisplayHeight);

  /// Display the image
  void DisplayRenderedImage();

private:

  /// Resize the render buffer
  void SetRenderSize(const uint4 uRenderWidth, const uint4 uRenderHeight);

  /// Return the smallest possible value of power of two that is greater or equal to x (it should not be here if there is one in the utilites lib)
  static uint4 ExpandToPowerOf2(uint4 x);
 
// Data members
public:

  /// Image buffer
  uint1 * m_pImageBuffer;
  
  /// Depth buffer
  float4 * m_pDepthBuffer;
  
	/// Width of the display
	uint4 m_uDisplayWidth;

  /// Height of the display
	uint4 m_uDisplayHeight;

  /// Viewing parameters
  Point<float> m_viewpoint;

  /// Viewing parameters
  Vector<float> m_vViewDirection;

  /// Viewing parameters
  Vector<float> m_vUpDirection;

  /// Viewing parameters
  Vector<float> m_vRight;

  /// Viewing parameters
  Vector<float> m_vDownDirection;

  /// Volumes to render
  LinVolUC * m_pVol;

  /// Space leaping volume
  LinVolUC * m_pSld;

  /// Preclassified and gradient modulated volume for opacity
  LinVolUC * m_pVolOpa;

  /// Inside volume, used for pick up control.
  BlockVolUC * m_pInsVol;

  /// Transfer function for transparent tissue projection (TTP)
  TransferFunctionVC m_TTPTransferFunction;

  /// Transfer function for non-transparent rendering
  TransferFunctionVC m_ExtViewTransferFunction;

  /// Zoom factor
  float m_fZoomFactor;

  /// Field of view, in degrees
  float m_fFov; 
  bool m_bTransparent;

  // the ratio of width/height of the display window (set this value to adjust overview image)
  float m_fRatio;

  /// Pointer to the dataset
  Dataset * m_pDataset;

private:

  /// Width of the actual buffer, set to power of two automatically
  uint4 m_uRenderWidth;

  /// Height of the actual buffer, set to power of two automatically
  uint4 m_uRenderHeight;

  /// Render context used in VolumeRender class
  friend class VolumeRender;

}; // class RenderContext

} // namespace ReaderLib


// $Log: RenderContext.h,v $
// Revision 1.2  2006/01/31 14:04:55  frank
// code review
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.3  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.2  2002/06/26 17:49:55  jmeade
// Moving more hard-coded strings to the string table; Code standards.
//
// Revision 3.1  2002/04/12 02:13:44  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
//    Rev 2.2   Oct 10 2001 11:53:12   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.1   Sep 20 2001 10:09:42   binli
// track  ID: 000523 : transparent rendering of overview, adjust w/h ratio when necessary.
// 
//    Rev 2.0   Sep 16 2001 23:40:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:08   ingmar
// Initial revision.
// Revision 1.9  2001/07/30 16:30:36  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.8  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.7  2001/03/21 21:14:24  liwei
// Updated RenderContext class for coding guideline compliance.
//
// Revision 1.6  2001/03/05 21:48:03  liwei
// Code rearranged to comply with the coding standard
