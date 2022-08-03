// $Id: StripProjection.h,v 1.7 2007/03/05 11:34:43 frank Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
// 
// Author: Frank Dachille (frank@viatronix.com)


// pragmas
#pragma once


// includes


// namespaces
namespace ReaderLib {


// forward declarations
class Dataset;


/**
 * The class which maintains the correlation between the strip image pixels and
 * the segments
 */
class StripProjection
{
public:

  /// default constructor
  StripProjection();

  /// constructor
  StripProjection( Dataset & dataset, const uint4 uImageHeightPixels );

  /// destructor
  virtual ~StripProjection();

  /// copy constructor
  StripProjection( const StripProjection & other );

  /// assignment operator
  StripProjection & operator =( const StripProjection & other );

  /// get the calculated full image width in pixels
  const uint4 GetImageWidthPixels() const { return m_uImageWidthPixels; }

  /// get the number of segments
  const int4 GetNumberSegments() const { return m_segmentBounds.size(); }

  /// determine the texture pixel x-coordiante related to a given screen pixel x-coordinate
  int4 GetTexturePixelX( const int4 iScreenPixelX, const int4 iDisplayHeightPixels, const int4 iImageHeightPixels ) const;

  /// determine which segment a particular pixel is in
  int4 GetSegment( const int4 iPixelX, const int4 iDisplayHeight, const int4 iImageHeightPixels ) const;

  /// gets the left and right pixel bounds for the given segment
  std::pair< uint4, uint4 > GetSegmentBounds( const int4 iSegment ) const;

  /// sets the windows width in pixels (also resets the scroll position)
  void SetWindowWidth( const uint4 uWindowWidthPixels );

  /// get the current scroll position
  const int4 GetScrollPosition() const { return m_iScrollPosition; }

  /// get the maximum scroll position
  const int4 GetMaxScrollPosition() const { return m_iMaxScrollPosition; }

  /// set the scroll position
  void SetScrollPosition( const int4 iScrollPosition );

  /// get the texture size
  const uint4 GetTextureSize() const { return 1024; }
  // TODO figure out why m_uTextureSize gets whacked after being set in the constructor!
  // note that we don't use the maximum because it can be more efficient to render
  // smaller textures because you only have to load the visible tiles

  /// get the minimum length of any one segment, in millimeters
  static const uint4 GetMinimumSegmentLengthMM() { return 128; }

  /// get the image height, in millimeters
  static const float4 GetImageHeighthMM() { return m_fImageHeightMM; }

private:

  /// the bounding rectangles for the segments (i) in the image looking up and down (first and second)
  std::vector< std::pair< uint4, uint4 > > m_segmentBounds;

  /// the calculated full image width in pixels
  uint4 m_uImageWidthPixels;

  /// the cached image height in pixels
  uint4 m_uImageHeightPixels;

  /// the image height in world coordinates
  static const float4 m_fImageHeightMM;

  /// the current scroll position
  int4 m_iScrollPosition;

  /// the maximum scroll position
  int4 m_iMaxScrollPosition;

  /// the selected texture size
  uint4 m_uTextureSize;

}; // class StripProjection


} // namespace ReaderLib


// $Log: StripProjection.h,v $
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
// Revision 1.4  2007/01/16 10:44:14  frank
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/StripProjection.h,v 1.7 2007/03/05 11:34:43 frank Exp $
// $Id: StripProjection.h,v 1.7 2007/03/05 11:34:43 frank Exp $
