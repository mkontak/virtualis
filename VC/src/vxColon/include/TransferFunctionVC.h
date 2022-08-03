// $Id: TransferFunctionVC.h,v 1.2 2006/12/05 20:16:05 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: TransferFunctionVC.h
// Description: A class to support transfer function creation and lookup
// Owner: Frank Dachille (viatronix@usa.com)


#if !defined(TransferFunctionVC_H)
#define TransferFunctionVC_H

// include declarations
#include "Triple.h"
#include "WindowLevel.h"
#include "Component.h"

// class definition
class TransferFunctionVC 
{
// Declarations
public:
  /// Structure to hold threshold values and their associated colours
  class LinearTFParams
  {
  public:
    /// default constructor
    LinearTFParams()
    {  }
    /// constructor
    LinearTFParams(const float4 fThd, const PixelRGBA<float4>& prevClr, const PixelRGBA<float4>& nextClr)
    { m_fThreshold = fThd; m_prevColor = prevClr; m_nextColor = nextClr; }

  public:
    /// threshold value
    float4 m_fThreshold;
    /// colour used prior to the threshold
    PixelRGBA<float4> m_prevColor;
    /// colour used after the threshold
    PixelRGBA<float4> m_nextColor;
  };
  
// Member Functions
public:

  /// Constructor.
	TransferFunctionVC();

  /// Destructor.
	~TransferFunctionVC();

  /// Set the colors in the entire range to a single color.
  void SetConstantColor(const Triple<float4>& vColor);

  /// Ramp up the opacity linearly from zero to some maximum over the full range.
  void SetLinearOpacity(const float4 fMaxOpacity);

  /// Sets the transfer function for viewing the colon wall given a threshold value.
  void SetColonWall(const float4 fColonWallThreshold);

  /// Gets default thresholds for the biopsy (pseudocolor) rendering
  static void GetDefaultBiopsyThresholds(std::vector<LinearTFParams>& vParams);

  /// Sets the transfer function for pseudocolored translucent rendering.
  void SetBiopsy();

  /// Returns the transfer function resolution/size
  uint4 GetResolution() const { return m_uSize; }

  /// Sets the transfer function for Agatston scoring colors.
  //void SetAgatston(Component<uint1> * pComponentInfo);
  
  /// Sets the transfer function for TTP or barium enema visualization.
  void SetTransparentTissueProjection();

  /// Sets the OpenGL texture environment to the current transfer function.
  void PrepareOpenGLTextureEnvironment() const;

  /// Sets the OpenGL pixel map environment to the current transfer function.
  void PrepareOpenGLPixelMapEnvironment() const;

  /// Sets the transfer function for an external view of the colon's surface.
  void SetExternalColonView();

  /// Gets the OpenGL RGB texture
  std::vector<float4> GetOpenGLTexture() const { return m_vfOpenGLTexture; }

  /// Gets the OpenGL RGBA pixel map
  std::vector<float4> GetOpenGLPixelMap() const { return m_vfOpenGLPixelMap; }

protected:

private:

  /// Pre-computes a data structure for efficient OpenGL texture binding at a later time.
  void PrepareOpenGLTexture();

  /// Pre-computes a data structure for efficient OpenGL pixel map binding at a later time.
  void PrepareOpenGLPixelMap();

// Data Members
public:

  /// The red component of the transfer function in the range [0-255].
  float4 * m_pfRed;

  /// The green component of the transfer function in the range [0-255].
  float4 * m_pfGreen;

  /// The blue component of the transfer function in the range [0-255].
  float4 * m_pfBlue;

  /// The alpha component of the transfer function in the range [0-1].
  float4 * m_pfOpacity;

  /// Colon wall threshold value
  uint4 m_fColonWallThreshold;

  /// biopsy thresholds
  std::vector<LinearTFParams> m_vfBiopsyThresholds;

  /// The specific color for indicating a voxel has been seen.
  static const Triple<uint1> m_vSeenScheme1;
  static const Triple<uint1> m_vSeenScheme2;

  /// The specific color for indicating a voxel has not yet been seen.
  static const Triple<uint1> m_vNotSeenScheme1;
  static const Triple<uint1> m_vNotSeenScheme2;
  
  /// Biopsy scale (like window/level).
  float4 m_fTranslucentScale;

   /// Window level
	/// used by 2d renderer. It should be merged with m_currenWindow, m_currentLevel 
  WindowLevel m_windowLevel;


protected:
private:

  /// The resolution of the transfer function.
  static const uint4 m_uSize;

  /// Contains all the entire transfer function compressed into a raw OpenGL-style texture data structure.
  std::vector<float4> m_vfOpenGLTexture;

  /// Contains all the entire transfer function compressed into a raw OpenGL-style pixel map data structure.
  std::vector<float4> m_vfOpenGLPixelMap;
};

#endif // !defined(TRANSFERFUNCTION_H)


// $Log: TransferFunctionVC.h,v $
// Revision 1.2  2006/12/05 20:16:05  jmeade
// Issue 5047: better organization of translucency settings structure.
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.7.2.4  2005/02/15 20:06:34  jmeade
// merge from vc1.4 branch.
//
// Revision 3.7.2.3.2.1  2004/10/18 20:43:49  jmeade
// const declaration on 'get' function.
//
// Revision 3.7.2.3  2004/05/15 00:34:42  jmeade
// Allow storage/adjustment of all biopsy/transluncency values.
//
// Revision 3.7.2.2  2004/05/14 17:18:50  jmeade
// Adjustable biopsy thresholds.
//
// Revision 3.7.2.1  2003/04/22 18:19:57  jmeade
// Issue 3123: Use std::vector<> for color maps (maybe one day we'll do same for the color arrays?)
//
// Revision 3.7  2002/11/11 14:23:56  kevin
// Store the value used for the colon wall threshold
//
// Revision 3.6  2002/08/14 16:10:34  wenli
// initial version - Vascular
//
// Revision 3.5  2002/05/07 18:26:18  frank
// Removed unneccessary includes.
//
// Revision 3.4  2002/04/30 23:58:44  michael
// Took Plaque and CardiacPlaque out of project file, only working in CS branch
//
// Revision 3.3  2002/03/11 17:39:07  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.2.2.1  2002/02/06 23:51:58  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.2  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.1  2001/11/13 04:00:09  frank
// Updated transfer function to include Agatston colors.
//
// Revision 3.0  2001/10/14 23:02:28  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:48   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.2  2001/07/31 19:18:58  huamin
// Temporarily add windowlevel
//
// Revision 1.1  2001/07/30 15:55:05  liwei
// 6_dataStruct TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.8  2001/07/20 13:16:15  geconomos
// Added event handling to handle state changes
//
// Revision 1.7  2001/07/06 15:45:08  huamin
// Added window level
//
// Revision 1.6  2001/06/06 19:47:48  jmeade
// Coding compliance
//
// Revision 1.5  2001/05/15 17:49:39  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.4  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.3  2001/03/21 14:58:57  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.2  2001/03/21 13:00:37  frank
// Moved SEEN and NOTSEEN colors out of #define into coding compliant Triple<uint1>
//
// Revision 1.1  2001/01/31 19:02:30  kevin
// Moved TransferFunctionVC to datastruct so triangle rendering could get
// correct colors
//
// Revision 1.7  2001/01/26 13:22:56  frank
// Removed bad #define
//
// Revision 1.6  2001/01/24 22:36:44  liwei
// Transfer function for non-transparent external view tuned
//
// Revision 1.5  2001/01/24 19:42:10  frank
// Rename TTP for clarity.
//
// Revision 1.4  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.3  2001/01/19 14:07:09  frank
// Added pixel color map features.
//
// Revision 1.2  2001/01/12 17:58:27  frank
// Added OpenGL texture functions.
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 4     8/28/00 12:49p Antonio
// updated header and footer
//
//*****************  Version 3  *****************
//User: Kevin           Date:  8/21/00  Time:  5:00p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Added scaleing (window/level-like functionality) of the translucent
//transfer function
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/18/00  Time:  1:57p
//Branched at version 2
//Comment:
//  
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/12/00  Time:  9:14p
//Created TransferFunctionVC.h
//Comment:
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/TransferFunctionVC.h,v 1.2 2006/12/05 20:16:05 jmeade Exp $
// $Id: TransferFunctionVC.h,v 1.2 2006/12/05 20:16:05 jmeade Exp $
