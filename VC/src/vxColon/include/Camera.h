// $Id: Camera.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.net)
//
// Complete history at bottom of file.

#ifndef Camera_h
#define Camera_h


// includes
#include "Triple.h"
#include "Point.h"
#include "Normal.h"
#include "Matrix.h"
#include "assert.h"


// forward declarations


class Camera  
{
public:
  enum ProjectionTypeEnum { PERSPECTIVE, ORTHOGRAPHIC };

  enum EventsEnum
  {
    EVENT_MODIFIED = 0x00000001,  
  };

// Member Functions
public:

  /// Default constructor.
  Camera();
  /// Handy constructor.
  Camera(const Point<float4> & vrp, const Normal<float4> & vpn, const Normal<float4> & vup);

  /// Get View reference point, i.e. Camera location
  inline const Point<float4> & GetVrp() const;

  /// View plane normal
  inline const Normal<float4> & GetVpn() const;

  /// View normal up vector
  inline const Normal<float4> & GetVup() const;

  /// Set View reference point, i.e. Camera location
  inline void SetVrp(const Point<float4> & vrp);

  /// View plane normal
  inline void SetVpn(const Normal<float4> & vpn);
  
  /// View normal up vector
  inline void SetVup(const Normal<float4> & vup);

  inline void MakeVupPerpendicular();
  
  // Supersampling in image and z-direction
  inline const int & GetSupersampling() const;
  inline void SetSupersampling(const int supersampling);
  inline const double & GetSamplingRate() const;
  inline void SetSamplingRate(const double samplingRate);
  
  // Field of view
  inline float4 GetFovxDegrees() const;
  inline const float4 & GetFovxRadians() const;
  inline float4 GetFovyDegrees() const;
  inline const float4 & GetFovyRadians() const;
  inline void SetFovxDegrees(const float4 fFov);
  inline void SetFovxRadians(const float4 ffov);
  inline void SetFovyDegrees(const float4 fFov);
  inline void SetFovyRadians(const float4 fFov);

  // Orthographic projection
  inline void SetOrthoFrustumSize(const Triple<float4> & orthoFrustumSize);
  inline const Triple<float4> & GetOrthoFrustumSize();

  // Projection type
  inline void SetProjectionType(const ProjectionTypeEnum eProjection);
  inline const ProjectionTypeEnum & GetProjectionType() const;

  // Transformation matrix from world coordinate to image coordiate
  inline Matrix<float4> GetMatrix(uint4 uImgWidth, uint4 uImgHeight);

  virtual void Modified(uint4 uFlags){};

  /// Input stream operator
  friend std::istream& operator>>(std::istream&, Camera&);
  /// Output stream operator
  friend std::ostream& operator<<(std::ostream&, const Camera&);

// Data Members
private:

  /// The view reference point.
  Point<float> m_viewReferencePoint;

  /// The view plane normal.
  Normal<float> m_viewPlaneNormal;

  /// The view up normal.
  Normal<float4> m_viewUpNormal;

  /// The field of view (horizontal, vertical, unused).
  Triple<float4> m_orthoFrustumSize;

  /// The field of view (horizontal, vertical, unused).
  Triple<float4> m_fieldOfViewRadiansTriple;

  /// The tangent of the field of view (horizontal, vertical, unused).
  Triple<float4> m_tangentFovTriple;

  /// The sampling rate (horizontal=1, vertical=1, intersample=1).
  Triple<float4> m_sampleRateTriple;

  /// The type of projection (eg, perspective).
  ProjectionTypeEnum m_eProjectionType;

};

#include "Camera.inl"

#endif

// $Log: Camera.h,v $
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.10  2002/09/27 19:01:12  jmeade
// Input/output stream operators.
//
// Revision 3.9  2002/05/07 18:31:49  frank
// Cleaned up includes.
//
// Revision 3.8  2002/05/07 18:28:48  frank
// Removed unneccessary includes.
//
// Revision 3.7  2002/03/07 22:31:08  dmitry
// Modified to fit new architecture.
//
// Revision 3.6  2002/02/06 22:02:26  michael
// no message
//
// Revision 3.5  2001/12/18 17:14:30  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 3.4  2001/12/12 20:02:12  jmeade
// Constructor for camera parameters.
//
// Revision 3.3  2001/11/08 22:23:49  geconomos
// Started new event processing (Have fun Dmitry!)
//
// Revision 3.2  2001/10/24 19:47:09  huamin
// Added some include files
//
// Revision 3.1  2001/10/24 19:17:47  huamin
// Added transfromation matrix which transform a point from world space to
// image space.
//
// Revision 3.0  2001/10/14 23:02:30  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:49:42   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:20   ingmar
// Initial revision.
// Revision 1.8  2001/07/30 15:26:32  huamin
// Added OrthoFrustumSize for Orthographic projection
//
// Revision 1.7  2001/07/20 16:07:27  frank
// Fixed types.
//
// Revision 1.6  2001/07/20 15:48:42  frank
// Working on Camera class.
//
// Revision 1.5  2001/07/10 18:26:28  frank
// work in progress
//
// Revision 1.4  2001/06/29 16:53:09  geconomos
// Moved from to 53_rendererDataStruct
//
// Revision 1.1  2001/06/25 21:23:06  geconomos
// moved from 51_applicationModel
//
// Revision 1.2  2001/06/20 19:27:05  frank
// Added code from home.
//
// Revision 1.1  2001/06/18 21:53:46  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Camera.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
// $Id: Camera.h,v 1.1 2005/08/05 12:54:57 geconomos Exp $
