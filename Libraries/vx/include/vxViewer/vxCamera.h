// $Id: vxCamera.h,v 1.2 2004/04/26 19:01:27 michael Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: jenny hizver (jenny@viatronix.com)

#ifndef vxCamera_h
#define vxCamera_h


// includes
#include "Point.h"
#include "Normal.h"
#include "Vector.h"
#include "Box.h"
#include "vxElement.h"


// forward declarations


// class definition
class VX_VIEWER_DLL vxCamera : public vxElement
{
// constants and enumerations
public:

  enum ProjectionTypeEnum { PERSPECTIVE, ORTHOGRAPHIC };

// methods
public:

  /// default constructor
  vxCamera();
  
  /// constructor
  vxCamera(const Point<float4> & reference, const Point<float4> & eye, const Normal<float4> & up);

  /// constructor
  vxCamera(const float4 fAspectRatio, const float4 fFovAngle);

  /// copy constructor
  vxCamera(const vxCamera & other);

  /// assignment operator
  const vxCamera & operator=(const vxCamera & other);

  /// equality operator
  bool operator ==(const vxCamera & other) const;

  /// view normal up vector
  const Normal<float4> & GetViewUp() const { return m_viewUpNormal; };

  /// get View Reference Point
  const Point<float4> & GetViewReference() const { return m_viewReferencePoint; };

  /// get view right vector
  Normal<float4> GetViewRight() const;

  /// get view direction
  const Normal<float4> GetViewDirection() const;
  
  /// Set view direction (when you don't care about the distance of the view reference point from the camera)
  void SetViewDirection( const Normal<float4> & direction );

  /// get view eye point
  const Point<float4> & GetEye() const { return m_viewEyePoint; };

  /// set view eye point
  void SetEye(const Point<float4> & eye) { m_viewEyePoint = eye; };
  
  /// view normal up vector
  void SetViewUp(const Normal<float4> & vector) {m_viewUpNormal = vector; };

  /// set view reference point
  void SetViewReference(const Point<float4> & position) { m_viewReferencePoint = position; };

  /// get field of view
  float4 GetFovDegrees() const { return m_fFovVerticalAngleDegrees; };
  
  /// set field of view
  void SetFovDegrees(const float4 & fFovAngle) { m_fFovVerticalAngleDegrees = fFovAngle; };

  /// get aspect ratio
  float4 GetAspectRatio() const { return m_fAspectRatio; };

  /// set aspect ratio
  void SetAspectRatio(const float4 & fAspectRatio) { m_fAspectRatio = fAspectRatio; };

  /// get projection type
  const ProjectionTypeEnum & GetProjectionType() const { return m_eProjectionType; };

  /// set projection type
  void SetProjectionType(const ProjectionTypeEnum eProjection) { m_eProjectionType = eProjection; };

  /// translate the eye point by vector
  void TranslateEyePoint(const Vector<float4> & fVector);
  
  /// translate the reference point by vector
  void TranslateReferencePoint(const Vector<float4> & fVector);

  /// rotate the camera about the axis in radians
  void Rotate(const Normal<float4> & axis, const float4 fRadians);

  /// rotate the camera about the axis in radians
  void Rotate(const Normal<float4> & axis, const float4 fRadians, const Point<float4> & center);

private:

  /// initialize to default values
  void CommonInit();

// data
private:

  /// view reference point (point the camera is pointing at)
  Point<float4> m_viewReferencePoint;

  /// eye point (camera location)
  Point<float4> m_viewEyePoint;

  /// view up normal
  Normal<float4> m_viewUpNormal;

  /// field of view vertical
  float4 m_fAspectRatio;

  /// field of view vertical angle in degrees
  float4 m_fFovVerticalAngleDegrees;

  /// type of projection (eg, perspective)
  ProjectionTypeEnum m_eProjectionType;
};


#endif


// Revision History:
// $Log: vxCamera.h,v $
// Revision 1.2  2004/04/26 19:01:27  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2003/09/02 14:48:20  frank
// Added equality operator
//
// Revision 1.19  2003/08/21 13:11:53  michael
// change naming from "angle" to "radians"
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
// Revision 1.15  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.14  2003/04/18 16:04:10  michael
// initial code review
//
// Revision 1.13  2003/04/14 21:08:58  michael
// cosmetics
//
// Revision 1.12  2003/03/14 15:23:03  frank
// Const parameters should be const!
//
// Revision 1.11  2003/01/22 21:43:43  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.10  2003/01/21 20:22:45  frank
// Added SetViewDirection()
//
// Revision 1.9  2002/12/17 17:40:03  frank
// Made some functions const because they are...
//
// Revision 1.8  2002/11/14 16:33:04  michael
// changed vxCamera::GetRightDirection() to vxCamera::GetViewRight()
//
// Revision 1.7  2002/10/23 22:54:48  frank
// consts are nice
//
// Revision 1.6  2002/10/18 20:28:57  frank
// Derived camera from vxElement.
//
// Revision 1.5  2002/10/16 23:12:33  michael
// added copy constructor
//
// Revision 1.4  2002/10/14 15:33:52  michael
// added comments
//
// Revision 1.3  2002/10/13 23:38:40  michael
// moved set/getBoundingBox to header file
//
// Revision 1.2  2002/10/13 21:40:10  michael
// returning bounding box should be const
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.3  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.2  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.1  2002/06/27 16:47:43  michael
// added vxCamera to replace CameraNew ...
//
// Revision 1.10  2002/06/21 16:20:13  frank
// Fixed Rotate()
//
// Revision 1.9  2002/06/17 23:29:08  uday
// Added Rotate() about a point.
//
// Revision 1.8.2.2  2002/05/15 17:39:24  frank
// Consts should be const.
//
// Revision 1.8.2.1  2002/05/15 16:20:29  frank
// Added GetRightDirection()
//
// Revision 1.8  2002/04/29 14:55:50  michael
// Added class RendererUtils for preset camera views
//
// Revision 1.7  2002/04/22 17:17:34  frank
// Added six standard views to 3D view.
//
// Revision 1.6  2002/03/26 20:56:31  uday
// GetPlanes for the view volume has bPrecision
// 1> true for bounding box limits
// 2> false for bounding sphere limits
//
// Revision 1.5  2002/03/15 01:29:13  michael
// fixed typo
//
// Revision 1.4  2002/03/11 22:25:29  michael
// added helper method
//
// Revision 1.3  2002/03/04 16:43:26  michael
// formatting and coding guidelines
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxCamera.h,v 1.2 2004/04/26 19:01:27 michael Exp $
// $Id: vxCamera.h,v 1.2 2004/04/26 19:01:27 michael Exp $
