// $Id: TrackBall.C,v 1.2 2006/09/13 15:32:39 frank Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File : TrackBall.C
// Description: A class using 2D interactive device (mouse) to simulate 3D controls 
// just like a trackball. These controls include rotate, translate, scale
// Created: June 1, 2000
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.
#include "stdafx.h"
#include "TrackBall.h"

using namespace ReaderLib;

#define FILE_REVISION "$Revision: 1.2 $"

const float4 fTRACKBALLSIZE = 0.8F;

TrackBall defaultTrackball;

/// Constructor: set up default values and initialize.
TrackBall::TrackBall()
{
  m_fDefaultZoomFactor = iDefaultZoomFactor;
  Reset();
  Init();
} // TrackBall()


/// Destructor.
TrackBall::~TrackBall()
{
} // ~TrackBall()


/**
 * Copy Constructor.
 *
 * @param otherTB   reference to another 'TrackBall' object.
 */
TrackBall::TrackBall(const TrackBall & OtherTB)
{
  Quaternion<float4> tq = OtherTB.m_vLastQuat;
  m_vLastQuat = tq;
  tq = OtherTB.m_vCurrQuat;
  m_vCurrQuat = tq;
  m_vTransferPt = OtherTB.m_vTransferPt;
  m_vRotStartPt = OtherTB.m_vRotStartPt;
  m_vCurrPt = OtherTB.m_vCurrPt;
  m_vMvStartPt = OtherTB.m_vMvStartPt;
  m_fZoomFactor = OtherTB.m_fZoomFactor;
  m_fDefaultZoomFactor = OtherTB.m_fDefaultZoomFactor;
  m_iZoomStart = OtherTB.m_iZoomStart;
} // TrackBall(const TrackBall & otherTB)


/**
 * copy operator
 *
 * @param   rhs     an instance of Trackball.
 * @return  pointer of this trackball.
 */
const TrackBall& TrackBall::operator=(const TrackBall& other)
{
  if ( &other != this ) 
  {
    Quaternion<float4> tq = other.m_vLastQuat;
    m_vLastQuat = tq;
    tq = other.m_vCurrQuat;
    m_vCurrQuat = tq;
    m_vTransferPt = other.m_vTransferPt;
    m_vRotStartPt = other.m_vRotStartPt;
    m_vCurrPt = other.m_vCurrPt;
    m_vMvStartPt = other.m_vMvStartPt;
    m_fZoomFactor = other.m_fZoomFactor;
    m_fDefaultZoomFactor = other.m_fDefaultZoomFactor;
    m_iZoomStart = other.m_iZoomStart;
    m_mCurrMatrix = other.m_mCurrMatrix;
  } // end-if.

  return *this;
} // end operator=


/** 
 * Reset.
 */
void TrackBall::Reset()
{
  m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);
  m_vCurrQuat(-0.707F, 0.0F, 0.0F, -0.707F);
  m_vTransferPt = Point2D<int4>(0,0);
  m_fZoomFactor = m_fDefaultZoomFactor;
  SetZoomLimits(m_fDefaultZoomFactor + (iMinFactor-iDefaultZoomFactor), 
                m_fDefaultZoomFactor + (iMaxFactor-iDefaultZoomFactor));
  BuildRotMatrix();
  m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
} // Reset()


/** 
 * Initialize based on current study orientation
 */
// void TrackBall::Init(const StudyOrientationEnum eOrientation)
void TrackBall::Init(const StudyOrientationEnum eOrientation)
{
  m_vTransferPt = Point2D<int4>(0,0);
  m_fZoomFactor = m_fDefaultZoomFactor;
  m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);
  // initialize m_mCurrMatrix.
  switch ( eOrientation ) 
  {
  case DEFAULT: // intentional fall-through
  case SUPINE:  // initialize it as a supine view.
  case PRONE:   // initialize it as a prone view.
    m_vCurrQuat( -0.707F, 0.0F, 0.0F, -0.707F );
    break;
  case NON_ROTATE:
    m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
  default:
    LogWrn("Bad case.", "TrackBall", "Init");
    break;
  } // end-switch supine/prone orientation
  
  BuildRotMatrix();
  m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
} // InitialBasedOnStudyOrientation()


/** 
 * Initialize based on orientation
 * 
 * @param eOrientation    orientation
 * @param iRotateDegree   degree rotate (anti-clock) around the viewing-axle (default=0, no rotate)
 */
void TrackBall::Init(const OrientationOfPlaneEnum eOrientation, const int4 iRotateDegree)
{
  m_vTransferPt = Point2D<int4>(0,0);
  m_fZoomFactor = m_fDefaultZoomFactor;

  m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);
  // initialize m_mCurrMatrix based on orientation and degree rotated around viewing axle.
  switch ( eOrientation ) 
  {
  case XYBack:  // 2
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat(0.0F, 0.0F, -0.707F, +0.707F); break;
    case 180: m_vCurrQuat(0.0F, 0.0F, -1.000F,  0.000F); break;
    case 270: m_vCurrQuat(0.0F, 0.0F, -0.707F, -0.707F); break;
    case   0:
    default : m_vCurrQuat(0.0F, 0.0F,  0.000F,  1.000F); break;
    }
    break;

  case YZFace:  // 3
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat(-0.500F,  0.500F, 0.500F, -0.500F); break;
    case 180: m_vCurrQuat(-0.707F,  0.000F, 0.707F,  0.000F); break;
    case 270: m_vCurrQuat(-0.500F, -0.500F, 0.500F,  0.500F); break;
    case   0: 
    default : m_vCurrQuat( 0.000F,  0.707F, 0.000F, -0.707F); break;
    }
    break;

  case YZBack:  // 4
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat(0.000F, -0.707F, 0.000F, -0.707F); break;
    case 180: m_vCurrQuat(0.500F, -0.500F, 0.500F, -0.500F); break;
    case 270: m_vCurrQuat(0.707F,  0.000F, 0.707F,  0.000F); break;
    case   0: 
    default : m_vCurrQuat(0.500F, -0.500F, 0.500F, -0.500F); break;
    }
    break;

  case ZXFace:  // 5
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat(-0.500F, -0.500F, 0.500F,  0.500F); break; 
    case 180: m_vCurrQuat( 0.000F, -0.707F, 0.707F,  0.000F); break;   
    case 270: m_vCurrQuat( 0.500F, -0.500F, 0.500F,  0.500F); break; 
    case   0:
    default:  m_vCurrQuat(-0.707F,  0.000F, 0.000F, -0.707F); break;
    }
    break;

  case ZXBack:  // 6
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat( 0.000F, -0.707F, -0.707F,  0.000F); break;
    case 180: m_vCurrQuat( 0.500F, -0.500F, -0.500F, -0.500F); break;  
    case 270: m_vCurrQuat( 0.707F,  0.000F,  0.000F, -0.707F); break;
    case   0: 
    default:  m_vCurrQuat(-0.500F, -0.500F, -0.500F,  0.500F); break;
    }
    break;

  case XYFace:  // 1
  default:
    switch (iRotateDegree)
    {
    case  90: m_vCurrQuat(   0.0F, -1.000F, 0.0F, 0.0F); break;
    case 180: m_vCurrQuat( 0.707F, -0.707F, 0.0F, 0.0F); break;
    case 270: m_vCurrQuat( 1.000F,  0.000F, 0.0F, 0.0F); break;
    case   0:
    default : m_vCurrQuat(-0.707F, -0.707F, 0.0F, 0.0F); break;
    }
    break;
  } // end-switch supine/prone orientation
  
  BuildRotMatrix();
  m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
} // InitialBasedOnFaceOrientation()


/** 
 * Initialize based on current study.
 */
void TrackBall::Init()
{
  m_vTransferPt = Point2D<int4>(0,0);
  m_fZoomFactor = 1.0f;
  m_fMinFactor = 0.2f;
  m_fMaxFactor = 10.0f;
  m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);

  // initialize m_mCurrMatrix.
  m_vCurrQuat(-0.707F, 0.0F, 0.0F, -0.707F);
  
  BuildRotMatrix();
  m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
} // Init()

/**
 * partial reset for Supine/Prone
 */
void TrackBall::PartialReset()
{
  m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);
  m_vCurrQuat(0.0F, 0.0F, 0.0F, 1.0F);
} // PartialReset()


/**
 *  initial computation.
 *
 *  @param  vw  width of the window.
 *  @param  vh  heigth of the window.
 */
void TrackBall::InitCalc(const int4 vw, const int4 vh)
{
  const float4  fDelta = 1E-6F;
  float4  fTb, fPhi;  // How much to rotate about axis.

  float4 fP1x = (float4)((2.0*m_vRotStartPt.m_x - vw)/vw);
  float4 fP1y = (float4)((vh - 2.0*m_vRotStartPt.m_y)/vh);  // same as horizonal processing
  float4 fP2x = (float4)((2.0*m_vCurrPt.m_x - vw)/vw);
  float4 fP2y = (float4)((vh - 2.0*m_vCurrPt.m_y)/vh);

  if ((fabs(fP1x-fP2x)<fDelta) && (fabs(fP1y-fP2y)<fDelta)) 
  {
    m_vLastQuat(0.0F, 0.0F, 0.0F, 1.0F);
  } 
  else 
  {
    Vector<float4> v1 = Vector<float4>(fP1x, fP1y, ProjectToSphere((float4)fTRACKBALLSIZE, fP1x, fP1y));
    Vector<float4> v2 = Vector<float4>(fP2x, fP2y, ProjectToSphere((float4)fTRACKBALLSIZE, fP2x, fP2y));
    Vector<float4> vAxis = v2.Cross(v1);  // Axis of rotation.
    Vector<float4> vDist = v1 - v2;
  
    fTb = (float4)vDist.GetLength()/(2.0*fTRACKBALLSIZE);
    if (fTb>1.0F)  
    {
      fTb = 1.0F;
    }
    if (fTb<-1.0F) 
    {
      fTb = -1.0F;
    }
    fPhi = (float4)(2.0 * asin(fTb));

    m_vLastQuat.SetRotationAboutAxis(vAxis, fPhi);
  } // end-if
} // InitCalc()


/**
 *  build rotate matrix from current quat.
 *
 *  @param  m[][] rotation matrix.
 */
void TrackBall::BuildRotMatrix(float4 fMatrix[][4])
{
  m_vCurrQuat.BuildMatrixFromQuat(fMatrix);
} // BuildRotMatrix()


/**
 * build rotate matrix.
 *
 * matrix Matrix
 */
void TrackBall::BuildRotMatrix(Matrix<float4> & mMatrix)
{
  m_vCurrQuat.BuildMatrixFromQuat(mMatrix);
} // BuildRotMatrix()


/**
 * build rotate matrix.
 *
 * matrix Matrix
 */
// void TrackBall::BuildRotMatrix(Matrix<float4> & mrMatrix)
void TrackBall::BuildRotMatrix()
{
  m_vCurrQuat.BuildMatrixFromQuat(m_mCurrMatrix);
}

/** 
 *  mapping coordinates to a sphere ... 
 *
 *  @param  radius  radius of the sphere.
 *  @param  (x,y) point in the window.
 *  @return z coordinate in a 3D sphere.
 */
float4 TrackBall::ProjectToSphere(const float4 fRadius, const float4 x, const float4 y)
{
  const float4 fHalfSqrt2 = 0.70710678118654752440F;  //half of the sqrt(2);
  float4 fTb, fDist0;

  float4 fDist = (float4)sqrt(x*x + y*y);
  if (fDist < fRadius * fHalfSqrt2) 
  {  
    // Inside sphere.
    fDist0 = (float4)sqrt(fRadius*fRadius - fDist*fDist);
  } 
  else 
  {   // On hyperbola.
    fTb = (float4)(fRadius/(fHalfSqrt2*2));
    fDist0 = (float4)(fTb*fTb/fDist);
  } // end-if
  return fDist0;
} // ProjectToSphere()


/**
 * form a new current quat according the contents in reserved and current quat.
 */
void TrackBall::AddQuats()
{
  m_vCurrQuat.AddQuats(m_vLastQuat);
} // AddQuats()  


/**
 * Copy matrix to m_mCurrMatrix[][].
 */
void TrackBall::CopyMatrix(const Matrix<float4> & mMatrix)
{
  m_mCurrMatrix = mMatrix;
} // CopyMatrix()

#undef FILE_REVISION

// $Log: TrackBall.C,v $
// Revision 1.2  2006/09/13 15:32:39  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.3  2003/01/22 22:28:58  ingmar
// added missing includes
//
// Revision 1.2  2002/07/03 03:23:21  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 1.1  2002/05/02 17:00:31  suzi
// Added TrackBall (only colon needs it).
//
// Revision 3.10  2002/03/11 19:01:38  mkontak
// Merged VC 1.2/CS 1.0
//
// Revision 3.9  2002/02/05 22:15:51  binli
// added new member: default zoom factor
// (which can be changed based on volume size.)
//
// Revision 3.8.2.1  2002/02/11 15:13:35  binli
// issue 1699 in branch: default zoom factor is variable according to volume size.
//
// Revision 3.8  2002/01/30 14:54:09  michael
// fixed zooming problem when zooming very far off, object turned around
//
// Revision 3.7  2002/01/29 20:09:59  michael
// comments, restructuring, and misc
//
// Revision 3.6  2002/01/17 16:25:16  binli
// removed unused variable.
//
// Revision 3.5  2001/12/03 18:42:59  jmeade
// no message
//
// Revision 3.4  2001/12/03 16:31:32  binli
// adjustment of initial function: based on study orientation/face orientation
//
// Revision 3.3  2001/11/21 23:54:27  binli
// initialized trackball based on direction of each face.
//
// Revision 3.2  2001/11/13 18:00:34  dmitry
// Events added and implemented.
//
// Revision 3.1  2001/10/26 02:21:07  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:31  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:04   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:50:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:42   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 23:32:19  ingmar
// new directory structure
//
// Revision 1.37  2001/08/01 16:21:45  liwei
// Added FireOnChanged() and OnChanged().
//
// Revision 1.36  2001/07/03 19:14:23  liwei
// Removed the inclusion of stdafx.h
//
// Revision 1.35  2001/05/24 20:39:04  binli
// moved to vxBase/9_render
//
// Revision 1.34  2001/05/23 19:16:38  liwei
// zoomFactor changed from int4 to float4
//
// Revision 1.33  2001/05/23 16:24:22  binli
// code conventions
//
// Revision 1.32  2001/05/22 17:23:10  liwei
// Allow user configurable default zoom factors
//
// Revision 1.31  2001/05/21 20:21:43  liwei
// TrackBall::Init(void) added
//
// Revision 1.30  2001/05/03 14:41:28  binli
// code walkthrough
//
// Revision 1.29  2001/04/05 20:43:26  binli
// private m_mCurrMatrix
//
// Revision 1.28  2001/04/05 16:27:07  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.27  2001/04/03 22:09:00  binli
// changed (x,y) to Point2D.
//
// Revision 1.26  2001/04/03 21:13:14  binli
// 1st code walkthrough
//
// Revision 1.25  2001/03/30 22:13:06  binli
// int-> int4
// float->float4
//
// Revision 1.24  2001/03/30 21:21:10  binli
// using template Quaternion
//
// Revision 1.23  2001/03/28 18:03:24  binli
// reflected changes made in Quaternion class
//
// Revision 1.22  2001/03/27 23:19:30  binli
// extended the function of class 'Quaternion'.
//
// Revision 1.21  2001/03/27 15:12:29  binli
// reflected Typedef.h
//
// Revision 1.20  2001/03/26 15:01:23  binli
// changed some variable name to make them more meanless.
//
// Revision 1.19  2001/03/16 21:39:16  binli
// renamed some variables according to new code standard
//
// Revision 1.18  2001/03/13 15:49:08  binli
// code walkthrough
//
// Revision 1.17  2001/01/26 19:58:29  binli
// defect 000188: default Overview position: added Init()
//
// Revision 1.16  2001/01/25 22:59:45  binli
// adjusted the initial zoom factor again!
//
// Revision 1.15  2001/01/23 20:26:06  binli
// changed the initial zoom factor again.
//
// Revision 1.14  2001/01/23 19:21:49  binli
// changed default setting of viewing angle (zoom factor)
//
// Revision 1.13  2001/01/17 20:45:16  binli
// change the setting of zoom factor to solve problem (defect 000165): zooming the overview changes the perceived color.
//
// Revision 1.12  2001/01/15 18:03:10  geconomos
// Added assignment operator
//
// Revision 1.11  2000/12/28 20:14:43  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.10  2000/12/28 19:59:04  binli
// used Matrix Class defined in 3_math.
//
// Revision 1.9  2000/11/22 20:50:39  binli
// fixed reset of trackball again
//
// Revision 1.8  2000/11/16 22:41:33  binli
// Simply registation of Supine  and Prone.
//
// Revision 1.7  2000/11/14 18:30:57  binli
// followed changes in COverview.
//
// Revision 1.6  2000/11/10 22:35:52  binli
// worked on both Supine/Prone overview rendering. In global variable 'rdrGlobal',
// two local trackballs are added. Some changes are made in TrackBall and OverView.
// Followed the change in OverView, VerifyView, EndoVisView, StandardView are
// changed a little.
//
// Revision 1.5  2000/09/22 15:48:10  kevin
// Changed the default orientation
//
// Revision 1.4  2000/09/22 14:25:02  binli
// Added some comments
//
// Revision 1.3  2000/09/21 21:19:02  binli
// removed vector operation originally defined in Trackball class.
// The vector operations are replaced by standard Vector defined in libMath.lib
//
// Revision 1.2  2000/09/21 18:39:55  binli
// Quaternion was import
//
// Revision 1.1  2000/09/18 21:38:13  binli
// Changed the source files from 'OverviewTrackball.C, .h' to 'TrackBall.C, .h'
// The revision histories are reserved.
//
// Revision 1.3  2000/09/18 18:19:22  binli
// Start Quaternion stuff
//
// Revision 1.2  2000/09/13 14:58:55  binli
// fitted to the Viatronix C++ coding conventions and standards
//
// Revision 1.1.1.1  2000/09/11 00:33:24  ingmar
// unit tests complete
//
// 
// 21    9/06/00 10:52a Binli
// fitted to the viatronix standard
// 
// 20    9/06/00 10:51a Binli
// fitted to viatronix standard
// 
// 19    8/30/00 6:34p Jeff
// Frame box and translucent modes need not be stored in TrackBall class
// (which stores position and orientation of the overview, i.e. where to
// draw it, not how (-:)
// 
// 18    8/28/00 11:34a Antonio
// updated header
//
//*****************  Version 17  *****************
//User: Binli           Date:  8/23/00  Time: 11:32a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 16  *****************
//User: Binli           Date:  8/23/00  Time: 11:30a
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 15  *****************
//User: Binli           Date:  8/22/00  Time:  9:50a
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  add comments
//
//*****************  Version 14  *****************
//User: Kevin           Date:  8/20/00  Time:  2:33p
//Checked in $/Viatron1000/src/ReaderLib
//Comment:
//  Changed default orientation
//
//*****************  Version 13  *****************
//User: Binli           Date:  8/18/00  Time:  4:33p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 12  *****************
//User: Binli           Date:  8/18/00  Time:  4:16p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 11  *****************
//User: Binli           Date:  8/18/00  Time:  3:26p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 10  *****************
//User: Binli           Date:  8/18/00  Time:  3:07p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 9  *****************
//User: Binli           Date:  8/17/00  Time:  4:55p
//Checked in $/Viatron1000/src/ReaderLib
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/16/00  Time: 10:14a
//Checked in $/ViatronMDI/src/ReaderLib
//Comment:
//  
//
//*****************  Version 7  *****************
//User: Binli           Date:  8/15/00  Time: 11:07a
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 6  *****************
//User: Kevin           Date:  8/14/00  Time:  8:33p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 5  *****************
//User: Binli           Date:  8/14/00  Time:  3:40p
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 4  *****************
//User: Jeff            Date:  8/14/00  Time:  2:49p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Created a new class ViatronStudy for study-specific info (bookmarks,
//narvigation)
//
//*****************  Version 3  *****************
//User: Binli           Date:  8/14/00  Time: 11:19a
//Checked in $/Viatron1000/src/Viatron
//
//*****************  Version 2  *****************
//User: Frank           Date:  8/10/00  Time:  4:37p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Removed compile warnings
//
//*****************  Version 1  *****************
//User: Binli           Date:  8/07/00  Time:  2:45p
//Created OverviewTrackBall.C
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/TrackBall.C,v 1.2 2006/09/13 15:32:39 frank Exp $
// $Id: TrackBall.C,v 1.2 2006/09/13 15:32:39 frank Exp $
