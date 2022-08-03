// $Id: TrackBall.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : OverviewTrackBall.h
// Description: A class using 2D interactive device (mouse) to simulate 3D controls 
// just like a trackball. These controls include rotate, translate, scale
// Created: June 18, 2000 (First day under control: July 18, 2000)
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

const static int4 iMaxFactor = 70;
const static int4 iMinFactor = 8;
const static int4 iDefaultZoomFactor = 24;

/**
 * set up start point for rotate calculation.
 *
 * @param (x,y)			start point (rotate).
 */
inline void TrackBall::SetRotateStartPoint(const Point2D<int4> & vPt2D)
{
  m_vRotStartPt = vPt2D;
} // SetStartPoint()


/**
 *	set up current point for rotate calculation.
 *
 *	@param (x,y)		current point (rotate).
 */
inline void TrackBall::SetRotateCurrentPoint(const Point2D<int4> & vPt2D)
{
	m_vCurrPt = vPt2D;
} // SetCurrentPoint()


/**
 *	set start point (y) for a scale calculation.
 *
 *	@param y		start point (zoom).
 */
inline void TrackBall::StartZoom(const int4 iZoom)
{
	m_iZoomStart = iZoom;
} // StartZoom()


/**
 *	changing scaling factor by using mouse middle button.
 *
 *	@param y		current point (zoom).
 */
inline void TrackBall::SetZoom(const int4 iZoom)
{
	m_fZoomFactor += ((float)(iZoom - m_iZoomStart))/10;
	if (m_fZoomFactor > m_fMaxFactor) 
	{
		m_fZoomFactor = m_fMaxFactor;
	} // the zoom factor should be no larger than the maxFactor
	
	if (m_fZoomFactor < m_fMinFactor) 
	{
		m_fZoomFactor = m_fMinFactor;
	} // the zoom factor should be no smaller than the minFactor
} // SetZoom()


/** 
 *  set default zoom factor.
 *
 *  @param fZoomFactor  default zoom factor.
 */
inline void TrackBall::SetDefaultZoomFactor(const float4 fZoomFactor)
{
  m_fDefaultZoomFactor = fZoomFactor;
  SetZoomLimits(m_fDefaultZoomFactor + (iMinFactor-iDefaultZoomFactor), 
                m_fDefaultZoomFactor + (iMaxFactor-iDefaultZoomFactor));
} // SetDefaultZoomFactor().


/**
 *	set zoom limitsc
 *
 *	@param(fMin, mMax)	min and max limits
 */
inline void TrackBall::SetZoomLimits(const float4 fMin, const float4 fMax)
{
  m_fMaxFactor = (fMax < m_fDefaultZoomFactor) ? m_fDefaultZoomFactor : fMax;
  m_fMinFactor = (fMin > m_fDefaultZoomFactor) ? m_fDefaultZoomFactor : fMin;
  if (m_fMinFactor <8.0F) m_fMinFactor = 8.0F;
} // SetZoomLimimits().


/** 
 * reset zoom factor.
 */
inline void TrackBall::ResetZoomFactor()
{
  m_fZoomFactor = m_fDefaultZoomFactor;
} // ResetZoomFactor()


/**
 *	change scaling factor by using mouse wheel.
 *
 *	@param delta		scale factor.
 */
inline void TrackBall::MouseWheelZoom(const float4 iDelta)
{
  m_fZoomFactor -= iDelta;
	
  if (m_fZoomFactor > m_fMaxFactor) 
  {
    m_fZoomFactor = m_fMaxFactor;
  } // the zoom factor should be no larger than the maxFactor
	
  if (m_fZoomFactor < m_fMinFactor) 
  {
    m_fZoomFactor = m_fMinFactor;
  } // the zoom factor should be no smaller than the minFactor
} // MouseWheelZoom()


/**
 *  Set m_qCurrQuat
 *
 * @param rQuat reference to a (float) quaternion.
 */
inline void TrackBall::SetCurrentQuat(const Quaternion<float4> & vQuat) 
{ 
  m_vCurrQuat = vQuat; 
} // SetCurrQuat()


/**
 * Get m_qCurrQuat.
 *
 * @return quaternion.
 */
inline const Quaternion<float4> & TrackBall::GetCurrentQuat() const 
{ 
  return m_vCurrQuat;
} // GetCurrentQuat()


/**
 *	set up start point for translate calculation.
 *
 *	@param (x,y)		start point (move).
 */
inline void TrackBall::SetMoveStartPoint(const Point2D<int4> & vPt2D)
{
  m_vMvStartPt = vPt2D;
} // StartMove()


/**
 *	set up offset of a translate operation and 
 *	set up new start point for the next translate.
 *
 *	@param (x,y)		current point (move).
 */
inline void TrackBall::SetMoveCurrentPoint(const Point2D<int4> & vPt2D)
{
  m_vTransferPt += vPt2D - m_vMvStartPt;
} // SetMove()


/**
 *  retrieve scale factor.
 *
 *	@return		value of zoom factor.
 */
inline float4 TrackBall::GetZoomFactor()	const
{
  return m_fZoomFactor;
} // GetZoomFactor()


/**
 *  set zoom factor.
 *
 *	@param	zoomFactor 
 */
inline void TrackBall::SetZoomFactor(const float4 fZoomFactor)
{
  m_fZoomFactor = fZoomFactor;
} // SetZoomFactor()


/**
 *	get x-offset for transfer.
 *
 *	@return		value of x-offset.
 */
inline int4 TrackBall::GetTransferX() const
{
	return m_vTransferPt.m_x;
} // GetTransferX()


/**
 *	get y-offset for transfer.
 *
 *	@return		value of y-offset.
 */
inline int4 TrackBall::GetTransferY()	const
{
	return m_vTransferPt.m_y;
} // GetTransferY()


/**
 *	set x and y-offset for transfer.
 *
 *	@parm transfer offset
 */
inline void TrackBall::SetTransfer(const Point2D<int4> & vPt2D)
{
  m_vTransferPt = vPt2D;
} // SetTransfer()


/**
 * Get m_mCurrMatrix.
 *
 * @return matrix<float4>
 */
inline const Matrix<float4> & TrackBall::GetCurrentMatrix() const 
{ 
  return m_mCurrMatrix; 
} // GetCurrentMatrix()


/**
 * Get m_mCurrMatrix.
 *
 * @return pointer to the current matrix.
 */
inline Matrix<float4> * TrackBall::GetCurrentMatrixPtr()
{ 
  return & m_mCurrMatrix; 
} // GetCurrentMatrixPtr()


/**
 * Set m_mCurrMatrix.
 *
 * @param mOther reference to a matrix<float4>
 */
inline void TrackBall::SetCurrentMatrix(Matrix<float4> & mOther) 
{ 
  m_mCurrMatrix = mOther;
} // SetCurrentMatrix()


// $Log: TrackBall.inl,v $
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.2  2002/05/09 20:54:15  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_05-06-2002)
//
// Revision 1.1  2002/05/02 17:00:30  suzi
// Added TrackBall (only colon needs it).
//
// Revision 3.3  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.2  2002/02/05 22:15:51  binli
// added new member: default zoom factor
// (which can be changed based on volume size.)
//
// Revision 3.1.2.1  2002/02/11 15:13:35  binli
// issue 1699 in branch: default zoom factor is variable according to volume size.
//
// Revision 3.1  2001/11/16 04:10:05  soeren
// Fixed typo limmiiits to limits
//
// Revision 3.0  2001/10/14 23:02:32  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:50:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:44   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:19  ingmar
// new directory structure
//
// Revision 1.25  2001/08/08 19:57:22  liwei
// Arguments to MouseWheelZoom() changed from int4 to float4.
//
// Revision 1.24  2001/05/23 19:16:38  liwei
// zoomFactor changed from int4 to float4
//
// Revision 1.23  2001/05/23 16:24:22  binli
// code conventions
//
// Revision 1.22  2001/05/22 17:23:10  liwei
// Allow user configurable default zoom factors
//
// Revision 1.21  2001/04/05 20:43:27  binli
// private m_mCurrMatrix
//
// Revision 1.20  2001/04/05 16:27:07  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.19  2001/04/03 22:09:00  binli
// changed (x,y) to Point2D.
//
// Revision 1.18  2001/04/03 21:13:14  binli
// 1st code walkthrough
//
// Revision 1.17  2001/04/02 20:08:52  binli
// reflected "Typedef.h"
//
// Revision 1.16  2001/03/30 22:13:06  binli
// int-> int4
// float->float4
//
// Revision 1.15  2001/03/27 21:33:21  binli
// continued code walkthrough.
//
// Revision 1.14  2001/03/26 15:01:23  binli
// changed some variable name to make them more meanless.
//
// Revision 1.13  2001/03/16 21:39:16  binli
// renamed some variables according to new code standard
//
// Revision 1.12  2001/02/15 01:16:00  jeff
// mouse WHEEL zoom and mouse BUTTON zoom should move intuitively
// in the same direction (up with wheel and up with mouse should do the same thing)
//
// Revision 1.11  2001/01/25 22:59:45  binli
// adjusted the initial zoom factor again!
//
// Revision 1.10  2001/01/24 08:15:14  geconomos
// Added SetTransfer() And SetZoomFactor()
//
// Revision 1.9  2001/01/23 20:26:06  binli
// changed the initial zoom factor again.
//
// Revision 1.8  2001/01/23 19:21:49  binli
// changed default setting of viewing angle (zoom factor)
//
// Revision 1.7  2001/01/17 20:45:16  binli
// change the setting of zoom factor to solve problem (defect 000165): zooming the overview changes the perceived color.
//
// Revision 1.6  2000/11/22 20:50:39  binli
// fixed reset of trackball again
//
// Revision 1.5  2000/11/22 20:23:21  binli
// fixed reset of trackball
//
// Revision 1.4  2000/11/17 21:27:13  binli
// adjusted the rendering order of colon and other stuffs in translucent mode.
//
// Revision 1.3  2000/09/22 14:25:02  binli
// Added some comments
//
// Revision 1.2  2000/09/18 21:38:13  binli
// Changed the source files from 'OverviewTrackball.C, .h' to 'TrackBall.C, .h'
// The revision histories are reserved.
//
// Revision 1.1  2000/09/18 19:49:29  binli
// Moved inline functions from .h to this .inl
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/TrackBall.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
// $Id: TrackBall.inl,v 1.1 2005/09/13 13:00:10 geconomos Exp $
