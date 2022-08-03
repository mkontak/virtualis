// $Id: vxSliceRenderer.h,v 1.10 2006/09/26 15:59:08 geconomos Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos (george@viatronix.com)

// pragmas
#pragma once

#ifndef VX_VIEWER_DLL
#define VX_VIEWER_DLL _declspec( dllimport )
#endif

// forward declarations
class vxSliceRendererImpl;
class vxEnvironment;
class vxRendererSoftwareMPR;
class vxSliceRendererData;
class PointF;

// include declarations
#include "vxIntensityStatistic.h"


// class declaration
class VX_VIEWER_DLL vxSliceRenderer
{
// enums and constants
public:

  enum vxSliceOrientationEnum
  {
    Axial = 0,
    Sagittal = 1,
    Coronal = 2,
    Oblique = 3,
  };

  enum vxSliceRenderingModeEnum
  {
    Default  = 0,
    XRay     = 1,
    ThinSlab = 2,
    MIP      = 3,    
  };

// member functions
public:
  
  /// default constructor
  vxSliceRenderer();
  
  /// destructor
  virtual ~vxSliceRenderer();
  
  /// renders an orthogonal slice into the image buffer
  void RenderSlice( const int4 iSliceNumber, const bool & bShowUncleansed, const bool & bShowPseudoColor ); 
  
  /// renders an oblique slice into the image buffer
  void RenderSlice( const Point3Df & bottomLeft,      const Point3Df  & bottomRight, 
                    const Point3Df & topLeft,         const Point3Df  & topRight, 
                    const bool   & bShowUncleansed,   const bool    & bShowPseudoColor ); 

  /// gets the slice renderer data
  const vxSliceRendererData & GetData() const { return *m_pData; }

  /// sets the slice renderer data
  void SetData( vxSliceRendererData * pData );

  /// gets the slice orientation
  const vxSliceOrientationEnum & GetOrientation() const { return m_eOrientation; }
  
  /// sets the slice orientation
  void SetOrientation( const vxSliceOrientationEnum & eOrientation );
  
  /// gets the rendering mode
  const vxSliceRenderingModeEnum & GetRenderingMode() const { return m_eRenderingMode; }
  
  /// sets the rendering mode
  void SetRenderingMode( const vxSliceRenderingModeEnum & eRenderingMode );

  /// gets the slab thickness in millimeters
  float4 GetSlabThickness() const;

  /// sets the slab thickness in millimeters
  void SetSlabThickness( const float4 fSlabThickness );
  
  /// sets the window level for the default colormap
  void SetDefaultWindowLevel( float4 fWindowHU, float4 fLevelHU );  

  /// sets the image width and height
  void SetObliqueImageSize( const int4 iWidth, const int4 iHeight );

  /// gets the slice width
  int4 GetImageWidth() const;
  
  /// gets the slice height
  int4 GetImageHeight() const;

  /// gets the slice image pixel data
  const uint1 * GetImageData() const;

  /// calculates the intensity statistics for the specified ellipse
  void GetIntensityStatistics( vxIntensityStatistic & outStatistics, 
                               const Point3Df & firstCornerPosition,
                               const Vector3Df & verticalEdge, 
                               const Vector3Df & horizontalEdge );

  /// converts a raw voxel value into a modality-specific one based on the current volume (cleansed/not)
  float4 ConvertRawToModality( const float4 fRawValue );

  /// converts a modality-specific value into a raw voxel one based on the current volume (cleansed/not)
  float4 ConvertModalityToRaw( const float4 fModalityValue );

 private:
 
  /// render functionality common to all orientations
  void RenderSlice();
  
  /// sets the cleansed or uncleansed volume as the actvie one
  void SetActiveVolume( bool bShowUncleansed );
  
  /// sets the default or pseudocolor colormap as the active one
  void SetActiveColormap( bool bShowPseudoColor );
  
// member variables
private:

  /// slice renderer data
  vxSliceRendererData * m_pData;
  
  /// rendering environment
  vxEnvironment * m_pEnvironment;
  
  /// current renderer
  vxRendererSoftwareMPR * m_pRenderer;
  
  /// slice orientation
  vxSliceOrientationEnum m_eOrientation;
  
  /// rendering mode
  vxSliceRenderingModeEnum m_eRenderingMode;

}; // class vxSliceRenderer

// $Log: vxSliceRenderer.h,v $
// Revision 1.10  2006/09/26 15:59:08  geconomos
// removed pimpl classes
//
// Revision 1.9  2005/07/13 14:44:42  geconomos
// Issue #4255: Improved the speed (and accuracy) of the ROI statistics
//
// Revision 1.8  2005/06/06 12:09:12  geconomos
// exposed image slice for oblique renderers
//
// Revision 1.7  2004/09/16 15:53:31  geconomos
// Moved default colormap from renderer data to renderer
//
// Revision 1.6  2004/05/19 13:04:07  geconomos
// Moved manipulation of window level to vxSliceRendererData.
//
// Revision 1.5  2004/05/17 13:29:19  geconomos
// Changed signature of SetColormap()
//
// Revision 1.4  2004/04/23 15:35:27  geconomos
// Added methods for setting the rendering mode and related properties.
//
// Revision 1.3  2004/04/21 18:33:39  geconomos
// Continued effort integrating 2D views within vc.
//
// Revision 1.2  2004/04/05 18:25:56  geconomos
// Initial effort to get volume files to load into vc in the block volume format.
//
// Revision 1.1  2004/04/05 15:44:44  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxSliceRenderer.h,v 1.10 2006/09/26 15:59:08 geconomos Exp $
// $Id: vxSliceRenderer.h,v 1.10 2006/09/26 15:59:08 geconomos Exp $