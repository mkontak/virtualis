// $Id: CrossPlane.h,v 1.1 2006/08/30 21:18:56 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Wenli Cai (wenli@viatronix.com)

/**
 *	a class to maintain the crossplane and detect the shape of the contour
 */


// pragmas
#pragma once


// includes
#include "Matrix.h"
#include "Image.h"


/**
 *	a class to maintain the crossplane and detect the shape of the contour
 */
class VX_VIEWER_DLL CrossPlane
{
// functions
public:

  /// constructor - set the iSize of the crossplane
  CrossPlane( const int4 iSize );
  
  /// constructor - set the position and orientation plus the iSize of the crossplane
  CrossPlane( const Point<float4> & centerPnt, const Vector<float4> & vNormal, const Vector<float4> & vUp, const int4 iSize );
  
  /// Destructor
  virtual ~CrossPlane( );


public:

  /// move the crossplane to a new position and set vUp the orientation
  void MoveCrossPlane( const Point<float4> & centerPnt, const Vector<float4> & vNormal, const Vector<float4> & vUp );  

  /// fill the crossplane by resampling the volume specified
  void FillCrossPlane( vxBlockVolume<uint2> & samVol );

  /// fill the crossplane by resampling the volume specified
  void FillCrossPlane( vxBlockVolume<bool> & componentVol );

  /// isolate the object by region growing with a threshold
  void IsolateMask( const float4 fMaskTH = -1.0f ); // if minus, relative mask

  /// clear the mask buffer
  void ClearMaskBuf();

  /// set the fScale of the crossplane 
  void SetScale( const float4 fScale ) { m_fScale = fScale;  };

  /// get the crossplane to volume coordinate transformation matrix
  Matrix<float4> GetIM2VolTransformMatrix( const Triple<float4> & volUnits ) const;

  /// get the crossplane to world coordiante transformation matrix
  Matrix<float4> GetIM2WorldTransformMatrix( const Triple<float4> & volUnits ) const;

  /// transform one point on crossplane to the volume coordiante 
  Point<float4> ConvertPnt2VolPos( const Triple<float4> & volUnits, const Point<float4> & pnt ) const;

  /// detect whether the crossplane is a valid crossplane
  bool IsValid() const;

  /// is the valid mask
  bool IsValidMask() const { return m_bValidMask; }

  /// calculate all the shape parameters of the crossplabe
  void CalShapePars(); 

  /// get the area of the vessel on the crossplane
  const float4 GetArea() const { return m_fArea; }

  /// get the centroid of the vessel on the crossplane
  const Point<float4> GetCentroid() const { return m_centroid; }

  /// get the width of the vessel on the crossplane -- short axis
  const float4 GetWidth() const { return m_fWidth; }; 

  /// get the diameter of the vessel on the crossplane -- long axis
  const float4 GetDiameter() const { return m_fDiameter;  };    

  /// get the direction of the long axis
  const Vector<float4> & GetLongAxis() const { return m_vLongAxis; }

  /// get the direction of the short axis
  const Vector<float4> & GetShortAxis() const { return m_vShortAxis; }

  /// get the two end points of the long axis
  void GetLongAxisEndPoints( Point<float4> & p0, Point<float4> & p1 ) const { p0 = m_longAxisP0; p1 = m_longAxisP1; }

  /// get the two end points of the short axis
  void GetShortAxisEndPoints( Point<float4> & p0, Point<float4> & p1 ) const { p0 = m_shortAxisP0; p1 = m_shortAxisP1; }
  

  /// extract the contour of the vessel on the crossplane
  void ExtractContour( const int2 iNumOfPoints, std::vector< Point<float4> > & contourPoint );

  /// shrink the wall contour to inner contour
  void ShrinkContour( const std::vector< Point<float4> > &wallContour, std::vector< Point<float4> > &innerContour, const int4 iPlaqueTh );
  
  /// is there calcium on the cross section
  bool IsCalcified( vxBlockVolume<uint2> & intensityVol, const int4 iPlaqueTh );

  /// move the center to an non-plaque positino
  bool FindNonPlaqueCenter( Point<float4> & vNonPlaqueCenter, const int4 iPlaqueTh );

  /// save the crossplane into PPM format image
  void SaveSnapshot( const std::string & sFileName ) const;

  
  /// calcuate the area of the vessel on the crossplane
  float4 CalArea();


private: // member functions

  /// calculate the centroid of the vessel on the crossplane
  Point<float4> CalCentroid( );

  /// calculate the long and short axis
  void CalTwoAxis(); 

  // calculate the moments of the crossplane
  void CalMoments( float4 &u20, float4 &u02, float4 &u11, float4 &xm, float4 &ym, int4 &xycount );

  /// calculate the eigen vector of the crossplane
  void CalTwoEigenVectors( Vector<float4> &longAxis, Vector<float4> &shortAxis );

  /// get the resampling pixel value on the crossplane
  float4 GetResampledValueRaw( Image<float4> &buf, Point<float4> &pnt ) const;

  /// get the maximum pixel value on the crossplane in the neighbors of pnt
  float4 GetResampledValueMax( Image<float4> &buf, Point<float4> &pnt ) const;

  /// parameterise the contour 
  void Parameterize( const std::vector< Point<float4> > & orgPointList, const int4 numOfPoints, std::vector< Point<float4> > & resPointList ) const;
  
  /// smooth the contour points
  void SmoothPointList(std::vector< Point<float4> > &orgPointList ) const;

// data
private: 

  /// the image buffer of the crossplane
  Image<float4> m_crossBuf; //buffer iSize = 2*half + 1 

  /// the mask of the crossplane
  Image<bool> m_crossBufMask; 

  /// X-axis of the crossplane
  Vector<float4> m_vDirU;
  
  /// Y-axis of the crossplane
  Vector<float4> m_vDirV;

  /// X-axis half size of the buffer
  int4 m_iBufXlenHalf;

  /// Y-axis half size of the buffer
  int4 m_iBufYlenHalf;

  /// buffer resolution
  Triple<uint4> m_bufDim;

  /// center point position of the crossplane 
  Point<float4> m_centerPnt;

  /// Normal of the crossplane 
  Vector<float4> m_vNormal;

  /// Up vector of the crossplane
  Vector<float4> m_vUp;

  /// the fScale of the crossplane
  float4 m_fScale;

  /// transformation matrix of the crossplane
  Matrix<float4> m_matIM2Obj;


  ///shape parameters
  /// centroid
  Point<float4> m_centroid;

  /// eigenvalue
  Vector<float4> m_vEigenV1, m_vEigenV2;

  /// long axis 
  Vector<float4> m_vLongAxis;

  /// short axis
  Vector<float4> m_vShortAxis;
  
  /// two end points of long axis
  Point<float4> m_longAxisP0, m_longAxisP1;

  /// two end points of short axis
  Point<float4> m_shortAxisP0, m_shortAxisP1;

  /// maximum distance between two points of the polygon
  float4 m_fDiameter; 

  ///minimum distance between parallel lines of support
  float4 m_fWidth; 

  ///mean center
  float4 m_iMeanXCenter, m_iMeanYCenter;

  /// area
  float4 m_fArea;

  /// the threshold of the mask 
  float4 m_maskTH;

  /// the current mask is valid
  bool m_bValidMask;

  /// RGBA LUT table for save image
  float *m_pColorTbl;
};


// $Log: CrossPlane.h,v $
// Revision 1.1  2006/08/30 21:18:56  frank
// moved here from vascular
//
// Revision 1.2  2005/06/08 14:52:01  vxconfig
// formatting
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/01/16 21:25:28  wenli
// Code Reviewer
//
// Revision 1.10  2004/01/14 18:51:23  wenli
// Fill cross-section with Bool volume is added
//
// Revision 1.9  2003/11/03 16:31:52  frank
// formatting
//
// Revision 1.8  2003/08/21 13:08:03  michael
// using M_PI instead of typed in values for it
//
// Revision 1.7  2003/06/19 20:15:07  wenli
// save the crosssection to a frame (Image: RGBA<uint1>)
//
// Revision 1.6  2003/06/18 15:19:44  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.5  2003/05/27 20:28:25  wenli
// fixing the lumencontour
//
// Revision 1.4  2003/05/20 21:19:57  wenli
// calculate a new centerPnt if centerPnt is in plaque
//
// Revision 1.3  2003/05/14 18:10:43  wenli
// add WallContour and LumenContour
//
// Revision 1.2  2003/05/08 21:06:55  geconomos
// Fixed Michael's stuff.
//
// Revision 1.1  2003/05/08 19:59:22  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.6  2003/05/07 20:59:46  dongqing
// code walkthrough
//
// Revision 1.5  2003/04/30 19:16:10  wenli
// code reviewed
//
// Revision 1.4  2003/03/25 16:50:50  wenli
// set the crosssection iSize dynamicly
//
// Revision 1.3  2003/03/18 19:06:58  wenli
// code refinement
//
// Revision 1.2  2003/03/17 18:54:23  wenli
// CenterlineRefiner is splited into two files and unused codes are cleared.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/CrossPlane.h,v 1.1 2006/08/30 21:18:56 frank Exp $
// $Id: CrossPlane.h,v 1.1 2006/08/30 21:18:56 frank Exp $



