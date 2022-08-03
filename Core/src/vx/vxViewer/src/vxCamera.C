// $Id: vxCamera.C,v 1.2 2004/04/26 18:58:39 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxCamera.h"
#include "Matrix.h"
#include <math.h>


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Default constructor
 */
vxCamera::vxCamera()
//*******************************************************************
{
  CommonInit();
} // default constructor


/**
 * Constructor
 * @param reference View reference point
 * @param eye View eye point
 * @param up View normal up vector
 */
vxCamera::vxCamera(const Point<float4> & reference, 
                   const Point<float4> & eye, 
                   const Normal<float4> & up)
//*******************************************************************
{
  CommonInit();

  SetViewReference(reference);
  SetEye(eye);
  SetViewUp(up);
} // constructor()


/**
 * Constructor
 * @param fAspectRation Aspect ratio
 * @param fFovAngle FOV angle
 */
vxCamera::vxCamera(const float4 fAspectRation, 
                   const float4 fFovAngle)
//*******************************************************************
{
  CommonInit();

  SetAspectRatio(fAspectRation);
  SetFovDegrees(fFovAngle);
} // constructor()


/**
 * CommonInit
 */
void vxCamera::CommonInit()
//*******************************************************************
{
  m_viewReferencePoint = Point<float4>(0.0F, 0.0F, 0.0F);
  m_viewEyePoint = Point<float4>(0.0F, -1.0F, 0.0F);
  m_viewUpNormal = Normal<float4>(0.0F, 0.0F, 1.0F);
  m_fAspectRatio  = 1.0F;
  m_fFovVerticalAngleDegrees = 45.0F;
  m_eProjectionType = ORTHOGRAPHIC;
} // CommonInit()


/**
 * Copy constructor
 * @param camera
 * @return const reference to this
 */
vxCamera::vxCamera(const vxCamera & camera)
//*******************************************************************
{
  *this = camera;
}


/**
 * Assignment operator
 * @param camera
 * @return const reference to this
 */
const vxCamera & vxCamera::operator=(const vxCamera & camera)
//*******************************************************************
{
  if (this != &camera)
  {
    m_viewUpNormal             = camera.m_viewUpNormal;
    m_viewReferencePoint       = camera.m_viewReferencePoint;
    m_viewEyePoint             = camera.m_viewEyePoint;
    m_fAspectRatio             = camera.m_fAspectRatio;
    m_fFovVerticalAngleDegrees = camera.m_fFovVerticalAngleDegrees;
    m_eProjectionType          = camera.m_eProjectionType;
    //m_BBox                     = camera.m_BBox;
  }
  return *this;
}


/**
 * Equality operator
 * @param other the other camera
 * @return ==
 */
bool vxCamera::operator==(const vxCamera & other) const
//*******************************************************************
{
  return
    m_viewUpNormal             == other.m_viewUpNormal             &&
    m_viewReferencePoint       == other.m_viewReferencePoint       &&
    m_viewEyePoint             == other.m_viewEyePoint             &&
    m_fAspectRatio             == other.m_fAspectRatio             &&
    m_fFovVerticalAngleDegrees == other.m_fFovVerticalAngleDegrees &&
    m_eProjectionType          == other.m_eProjectionType;

} // operator ==()


/**
 * Set View reference point.
 * @param vrp const Point<float4> &
 * @return void
 */
const Normal<float4> vxCamera::GetViewDirection() const
//*******************************************************************
{
  return Normal<float4>(GetViewReference() - GetEye());
}


/**
 * Set view direction (when you don't care about the distance of the view reference point from the camera).
 * It orients the camera in the right direction by moving the reference point one unit away from the eyepoint.
 * @param direction the desired view direction
 */
void vxCamera::SetViewDirection(const Normal<float4> & direction)
//*******************************************************************
{
  m_viewReferencePoint = m_viewEyePoint + Vector3Df(direction);
}


/**
 * Get right vector
 * @return right vector
 */
Normal<float4> vxCamera::GetViewRight() const
//*******************************************************************
{ 
  return Normal<float4>(GetViewDirection().Cross(GetViewUp()));
}


/**
 * Translate the camera by the vector
 * @param vector the vector to translate by
 */
void vxCamera::TranslateEyePoint(const Vector<float4> & vector)
//*******************************************************************
{
  m_viewEyePoint.m_x += vector.m_x;
  m_viewEyePoint.m_y += vector.m_y;
  m_viewEyePoint.m_z += vector.m_z;
} // TranslateEyePoint()


/**
 * Translate the camera by the vector
 * @param vector the vector to translate by
 */
void vxCamera::TranslateReferencePoint(const Vector<float4> & vector)
//*******************************************************************
{
  m_viewReferencePoint.m_x += vector.m_x;
  m_viewReferencePoint.m_y += vector.m_y;
  m_viewReferencePoint.m_z += vector.m_z;
} // TranslateReferencePoint()


/**
 * Rotate the camera about the axis by the angle.
 * Assume the axis is going through the view reference point
 * (1) translate eye parallel to vrp such that vrp = 000
 * (2) rotate eye about axis 
 * (3) translate eye back parallel to vrp such that vrp is back at initial position
 *
 * @param axis the axis of rotation
 * @param fRadians radians to be rotated
 */
void vxCamera::Rotate(const Normal<float4> & axis, 
                      const float4 fRadians)
//*******************************************************************
{
  Matrix<float4> matrixCombined, matrixTranslateToOrigin, matrixRotate, matrixTranslateToReferencePoint;
  matrixCombined.Identity();
  matrixTranslateToOrigin.Identity();
  matrixRotate.Identity();
  matrixTranslateToReferencePoint.Identity();

  matrixTranslateToOrigin.Translate(Vector3D<float4>(m_viewReferencePoint * -1));
  matrixRotate.Rotate(axis, fRadians);
  matrixTranslateToReferencePoint.Translate(Vector3D<float4>(m_viewReferencePoint));
  
  matrixCombined = matrixTranslateToReferencePoint * matrixRotate * matrixTranslateToOrigin;
  
  m_viewEyePoint = matrixCombined * m_viewEyePoint;
  m_viewUpNormal = matrixRotate * m_viewUpNormal;
} // Rotate()


/**
 * Rotate the camera about the axis by the angle
 * @param axis     axis of rotation
 * @param fRadians radians to be rotated
 * @param center   center of rotation
 */
void vxCamera::Rotate(const Normal<float4> & axis, 
                      const float4 fRadians, 
                      const Point<float4> & center)
//*******************************************************************
{
  Matrix<float4> matrixCombined, matrixTranslate, matrixRotate;
  matrixCombined. Identity();
  matrixTranslate.Identity();
  matrixRotate.   Identity();

  matrixTranslate.Translate(Vector3D<float4>(center));
  matrixRotate.Rotate(axis, fRadians);
  
  matrixCombined = matrixCombined * matrixTranslate;
  matrixCombined = matrixCombined * matrixRotate;
  matrixTranslate.Identity();
  matrixTranslate.Translate(Vector3D<float4>(center * -1));
  matrixCombined = matrixCombined * matrixTranslate;  
  
  m_viewEyePoint       = matrixCombined * m_viewEyePoint;
  m_viewUpNormal       = matrixCombined * m_viewUpNormal;
  m_viewReferencePoint = matrixCombined * m_viewReferencePoint;
} // Rotate()


#undef FILE_REVISION 


// Revision History:
// $Log: vxCamera.C,v $
// Revision 1.2  2004/04/26 18:58:39  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.23  2003/09/02 14:48:20  frank
// Added equality operator
//
// Revision 1.22  2003/08/21 13:11:53  michael
// change naming from "angle" to "radians"
//
// Revision 1.21  2003/08/20 15:48:17  michael
// using M_PI instead of typed in values for it
//
// Revision 1.20  2003/08/20 14:11:00  michael
// using M_PI instead of 3.1415...
//
// Revision 1.19  2003/05/20 02:01:12  michael
// better doxygen comments
//
// Revision 1.18  2003/05/16 11:45:00  michael
// code review
//
// Revision 1.17  2003/04/19 16:26:09  frank
// Did not compile due to typo and signature mismatch
//
// Revision 1.16  2003/04/19 01:49:07  michael
// some more code review
//
// Revision 1.15  2003/04/14 21:08:58  michael
// cosmetics
//
// Revision 1.14  2003/02/13 17:16:54  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.13  2003/02/12 14:24:29  frank
// Initialized camera to anterior view
//
// Revision 1.12.2.1  2003/02/12 22:51:02  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.12  2003/01/22 21:43:43  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.11  2003/01/21 20:44:10  michael
// cosmetics
//
// Revision 1.10  2003/01/21 20:22:45  frank
// Added SetViewDirection()
//
// Revision 1.9  2002/12/17 17:40:03  frank
// Made some functions const because they are...
//
// Revision 1.8  2002/11/14 19:09:47  ingmar
// bounded near plane distance to never go below 0.01
//
// Revision 1.7  2002/10/23 22:54:48  frank
// consts are nice
//
// Revision 1.6  2002/10/21 23:14:52  ingmar
// slightly cleaner rotate code
//
// Revision 1.5  2002/10/16 23:12:34  michael
// added copy constructor
//
// Revision 1.4  2002/10/13 23:38:40  michael
// moved set/getBoundingBox to header file
//
// Revision 1.3  2002/10/13 21:48:16  michael
// accounting for the const Box<T> returned from the camera object
//
// Revision 1.2  2002/09/24 16:01:51  michael
// fixed copy constructor and assignment operator that weren't working at all
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.2  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.1  2002/06/27 16:47:43  michael
// added vxCamera to replace CameraNew ...
//
// Revision 1.15  2002/06/21 16:20:13  frank
// Fixed Rotate()
//
// Revision 1.14  2002/06/17 23:29:08  uday
// Added Rotate() about a point.
//
// Revision 1.13  2002/04/29 14:55:50  michael
// Added class RendererUtils for preset camera views
//
// Revision 1.12  2002/04/22 21:53:11  uday
// Made default camera orthographic.
//
// Revision 1.11  2002/04/22 17:17:34  frank
// Added six standard views to 3D view.
//
// Revision 1.10  2002/04/18 17:32:54  uday
// rotate update
//
// Revision 1.9  2002/03/29 16:08:05  michael
// fixed compiler warnings
//
// Revision 1.8  2002/03/29 01:38:22  ingmar
// added  #undef FILE_REVISION
//
// Revision 1.7  2002/03/28 21:40:35  uday
// sign change in top/bottom planes
//
// Revision 1.6  2002/03/28 18:51:47  dmitry
// Corrected the view volume planes (Michael)
//
// Revision 1.5  2002/03/26 20:56:31  uday
// GetPlanes for the view volume has bPrecision
// 1> true for bounding box limits
// 2> false for bounding sphere limits
//
// Revision 1.4  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 1.3  2002/03/04 16:43:26  michael
// fomatting and coding guidelines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxCamera.C,v 1.2 2004/04/26 18:58:39 michael Exp $
// $Id: vxCamera.C,v 1.2 2004/04/26 18:58:39 michael Exp $
