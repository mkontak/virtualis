// $Id: StripProjection.C,v 1.7 2007/03/05 11:34:43 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include <algorithm>
#include "Dataset.h"
#include "StripViewer.h"
#include "ReaderGlobal.h"
#include "RenderUtil.h"
#include "Plane.h"
#include "Line.h"


// defines
#define FILE_REVISION "$Revision: 1.7 $"


// namespaces
using namespace ReaderLib;
using namespace ReaderLib::SliceOrientations;


// static member declarations
// TODO: make the zoom adjustable so you don't miss anything
const float4 StripProjection::m_fImageHeightMM = 30.0F;


/**
 * default constructor
 */
StripProjection::StripProjection() :
m_uImageWidthPixels( 0 )
{
} // StripProjection()


/**
 * constructor
 */
StripProjection::StripProjection( Dataset & dataset, const uint4 uImageHeightPixels )
{

  // compute the full length of the image in millimeters
  float4 fSumLengthMM = 0.0F;
  std::vector< float4 > segmentEndPositionMM( dataset.m_straightSegmentNodes.size() - 1 );
  for ( int i=0; i<dataset.m_straightSegmentNodes.size()-1; ++i )
  {
    // get the two skeleton nodes at opposite ends of the current segment
    uint4 uIndex1 = dataset.m_straightSegmentNodes[ i     ];
    uint4 uIndex2 = dataset.m_straightSegmentNodes[ i + 1 ];
    Point<float4> endpoint1 = dataset.m_joinedSkeleton.GetWorldPos( uIndex1 );
    Point<float4> endpoint2 = dataset.m_joinedSkeleton.GetWorldPos( uIndex2 );
    float4 fSegmentLengthMM = ( endpoint2 - endpoint1 ).GetLength();
    // enforce the minimum segment length
    //if ( fSegmentLengthMM < GetMinimumSegmentLengthMM() )
    //  fSegmentLengthMM = GetMinimumSegmentLengthMM();
    fSumLengthMM += fSegmentLengthMM;
    segmentEndPositionMM[i] = fSumLengthMM;
  }

  // we want to keep a unit aspect ratio so compute the required image width to maintain it
  const float4 fImageWidthPixels = fSumLengthMM / m_fImageHeightMM * uImageHeightPixels;

  // TODO: possibly add gaps between segments

  // check the texture size
  GLint iMaxTextureSize = 0;
  glGetIntegerv( GL_MAX_TEXTURE_SIZE, & iMaxTextureSize );
  const uint4 uDesiredTextureSize = 1024;
  m_uTextureSize = min( iMaxTextureSize, uDesiredTextureSize );

  // initially, there will be no gaps between segments
  // make the image isotropic
  // make it at least as many pixels as the texture size
  m_uImageWidthPixels = max( m_uTextureSize, static_cast< uint4 >( fImageWidthPixels ) );

  // scale the segment lengths from MM to pixels and store for later
  const float4 fScaleFactor = static_cast< float4 >( m_uImageWidthPixels ) / fSumLengthMM;
  m_segmentBounds.resize( dataset.m_straightSegmentNodes.size()-1 );
  float4 fStartPositionPixels = 0.0f;
  for ( int i=0; i<dataset.m_straightSegmentNodes.size()-1; ++i )
  {
    m_segmentBounds[i] = std::make_pair( fStartPositionPixels, segmentEndPositionMM[ i ] * fScaleFactor );
    fStartPositionPixels = m_segmentBounds[i].second;
  }

  // determine the maximum scroll position - guess for now until we get the real data below in SetWindowWidth()
  m_iScrollPosition = 0;
  m_iMaxScrollPosition = m_uImageWidthPixels - 1024;

} // StripProjection( dataset, image, max texture size )


/**
 * destructor
 */
StripProjection::~StripProjection()
{
}


/**
 * copy constructor
 *
 * @param   other   the other object
 */
StripProjection::StripProjection( const StripProjection & other ) :
m_segmentBounds( other.m_segmentBounds ),
m_uImageWidthPixels( other.m_uImageWidthPixels )
{
} // StripProjection( other )


/**
 * assignment operator
 */
StripProjection & StripProjection::operator =( const StripProjection & other )
{
  if ( & other != this )
  {
    m_segmentBounds = other.m_segmentBounds;
    m_uImageWidthPixels = other.m_uImageWidthPixels;
  }

  return * this;
} // operator =( other )


/**
 * determine the texture pixel x-coordiante related to a given screen pixel x-coordinate
 *
 * @param   iScreenPixelX         the input x position in screen pixel coordinates
 * @param   iDisplayHeightPixels  the height of the image in screen pixel coordinates
 * @param   iImageHeightPixels    the height of the image in texture pixel coordinates
 *
 * @return                        the texture pixel x-coordinate
 */
int4 StripProjection::GetTexturePixelX( const int4 iScreenPixelX, const int4 iDisplayHeightPixels, const int4 iImageHeightPixels ) const
{

  // scale by image drawing scale factor
  const float4 fIsotropic = static_cast< float4 >( iImageHeightPixels ) / static_cast< float4 >( iDisplayHeightPixels );

  // offset pixel by scroll position
  return static_cast< int4 >(  iScreenPixelX * fIsotropic + m_iScrollPosition );

} // GetTexturePixelX()

/**
 * determine which segment a particular pixel is in
 *
 * @param   iPixelX               the input x position in pixel coordinates
 * @param   iDisplayHeightPixels  the height of the image in screen pixel coordinates
 * @param   iImageHeightPixels    the height of the image in texture pixel coordinates
 *
 * @return                        the output segment number or -1 if not found
 */
int4 StripProjection::GetSegment( const int4 iPixelX, const int4 iDisplayHeightPixels, const int4 iImageHeightPixels ) const
{

  // determine the x texture pixel coordinate
  const int4 iRenderedPixelX = GetTexturePixelX( iPixelX, iDisplayHeightPixels, iImageHeightPixels );

  // search through the segment bounds
  for ( int i=0; i<m_segmentBounds.size(); ++i )
  {
    if ( m_segmentBounds[ i ].first <= iRenderedPixelX &&
         m_segmentBounds[ i ].second > iRenderedPixelX )
         return i;
  }

  // not found, return -1
  return -1;

} // GetSegment( iPixelX, iDisplayHeightPixels, iImageHeightPixels )


/**
 * Gets the left and right pixel bounds for the given segment
 *
 * @param   iSegment    the given segment index
 * @return              the left and right pixel bounds for the given segment
 */
std::pair< uint4, uint4 > StripProjection::GetSegmentBounds( const int4 iSegment ) const
{
  return m_segmentBounds[ iSegment ];
} // GetSegmentBounds( iSegment )


/**
 * sets the windows width in pixels (also resets the scroll position)
 *
 * @param   uWindowWidthPixels    the width of the window in pixels
 */
void StripProjection::SetWindowWidth( const uint4 uWindowWidthPixels )
{

  m_iScrollPosition = 0;
  m_iMaxScrollPosition = m_uImageWidthPixels - uWindowWidthPixels;

} // SetWindowWidth( width )


/**
 * set the scroll position
 *
 * @param   uScrollPosition   the new scroll position
 */
void StripProjection::SetScrollPosition( const int4 iScrollPosition )
{
  m_iScrollPosition = min( iScrollPosition, m_iMaxScrollPosition );
} // SetScrollPosition( iScrollPosition )


// $Log: StripProjection.C,v $
// Revision 1.7  2007/03/05 11:34:43  frank
// Working on stretched strip images
// Added centerline rendering
// Reversed strip segment direction
//
// Revision 1.6  2007/02/17 17:43:07  frank
// synchronized 3D view when dragging left mouse button on strip
//
// Revision 1.5  2007/02/14 21:01:21  frank
// fixed up scrolling and segment selection
//
// Revision 1.4  2007/01/16 10:43:41  frank
// working on scrolling
//
// Revision 1.3  2007/01/04 06:52:24  frank
// refactoring
//
// Revision 1.2  2006/12/23 21:45:41  frank
// more refactoring
//
// Revision 1.1  2006/12/23 21:11:32  frank
// split out StripProjection class
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/StripProjection.C,v 1.7 2007/03/05 11:34:43 frank Exp $
// $Id: StripProjection.C,v 1.7 2007/03/05 11:34:43 frank Exp $
