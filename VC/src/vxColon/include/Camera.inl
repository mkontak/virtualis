// $Id: Camera.inl,v 1.2 2006/09/26 19:33:13 frank Exp $
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


inline const Point<float4> & Camera::GetVrp() const
{
  return m_viewReferencePoint;
}

inline const Normal<float4> & Camera::GetVpn() const
{
  return m_viewPlaneNormal;
}

inline const Normal<float4> & Camera::GetVup() const
{
  return m_viewUpNormal;
}

inline void Camera::SetVpn(const Normal<float4> & vpn)
{
  m_viewPlaneNormal = vpn;
}

inline void Camera::SetVrp(const Point<float4> & vrp)
{
  m_viewReferencePoint = vrp;
}

inline void Camera::SetVup(const Normal<float4> & vup)
{
  m_viewUpNormal = vup;
}

inline void Camera::MakeVupPerpendicular()
{
  float4 fDot(m_viewPlaneNormal.Dot(m_viewUpNormal));
  Point<float4> vupTip = m_viewReferencePoint + Vector<float4>(m_viewUpNormal);
  vupTip += Vector<float4>(m_viewPlaneNormal) * fDot;
  m_viewUpNormal = Normal<float4>(vupTip - m_viewReferencePoint);
}

inline float4 Camera::GetFovxDegrees() const
{
  return m_fieldOfViewRadiansTriple.m_x / M_PI * 180.0F;
}

inline const float4 & Camera::GetFovxRadians() const
{
  return m_fieldOfViewRadiansTriple.m_x;
}

inline float4 Camera::GetFovyDegrees() const
{
  return m_fieldOfViewRadiansTriple.m_y / M_PI * 180.0F;
}

inline const float4 & Camera::GetFovyRadians() const
{
  return m_fieldOfViewRadiansTriple.m_y;
}

inline void Camera::SetFovxDegrees(const float4 fFov)
{
  // degrees to radians
  SetFovxRadians(fFov * 180.0F * M_PI);
}

inline void Camera::SetFovxRadians(const float4 fFov)
{
  m_fieldOfViewRadiansTriple.m_x = fFov;
  m_tangentFovTriple.m_x = tan(m_fieldOfViewRadiansTriple.m_x / 2.0F);
}

inline void Camera::SetFovyDegrees(const float4 fFov)
{
  // degrees to radians
  SetFovyRadians(fFov * 180.0F * M_PI);
}

inline void Camera::SetFovyRadians(const float4 fFov)
{
  m_fieldOfViewRadiansTriple.m_y = fFov;
  m_tangentFovTriple.m_y = tan(m_fieldOfViewRadiansTriple.m_y / 2.0F);
}

inline void Camera::SetOrthoFrustumSize(const Triple<float4> & orthoFrustumSize)
{
  m_orthoFrustumSize = orthoFrustumSize;
}

inline const Triple<float4> & Camera::GetOrthoFrustumSize()
{
  return m_orthoFrustumSize;
}

// Projection type
inline void Camera::SetProjectionType(const ProjectionTypeEnum eProjection)
{
  m_eProjectionType = eProjection;
}

inline const Camera::ProjectionTypeEnum & Camera::GetProjectionType() const
{
  return m_eProjectionType;
}

/**
 *   Compute the transformation matrix which transforms a point from world coordinate to image coordinate
     Let M be the matrix:
     For orthographic projection,   M * (Xw, Yw, Zw, 1) = (U, V, Depth, 1)
     The inverse matrix exists.     (Inverse of M)  * (U, V, Depth, 1) = (Xw, Yw, Zw, 1)
     (U, V) is pixel position, Depth is z depth. (Xw, Yw, Zw) is world coordinate

	 If you use the Matrix class at VxBase, it is important to multiply the Matrix with a point
	 object, for example, Point<float4> (Xw, Yw, Zw).  It does not work for vector or
	 other objects.
	                 M * Point<float4> (Xw, Yw, Zw) you get Point<float4> (U, V, Depth)

     For perspective projection,  M * (Xw, Yw, Zw, 1) = (U*depth, V*depth, depth, depth)
	 The inverse matrix does not exist.
 */
inline Matrix<float4> Camera::GetMatrix(uint4 uImgWidth, uint4 uImgHeight)
{
  // Compute world coordinate to view coordinate matrix. 
  /**  World to View Matrix
   Given a camera position C, viewing direction  N,
   up Vector V, and right Vector U = (N x V),  the matrix to
   transfrom points from world space to view space is:
             | Ux  Uy  Uz   0  |   | 0   0  0  -Cx  |
	  M =    | Vx  Vy  Vz   0  | * | 0   1  0  -Cy  |
	         | Nx  Ny  Nz   0  |   | 0   0  1  -Cz  |
			 | 0   0   0    1  |   | 0   0  0   1   | 
   The origin of the view coordinate is at left bottom corner
  **/

 // MakeVupPerpendicular();

  Normal<float4> viewRightNormal(m_viewPlaneNormal.Cross(m_viewUpNormal));

  assert(fabsf(viewRightNormal.Dot(m_viewUpNormal))<0.001 &&
	     fabsf(viewRightNormal.Dot(m_viewPlaneNormal))<0.001 &&
		 fabsf(m_viewPlaneNormal.Dot(m_viewUpNormal))<0.001);

  Matrix<float4> translationMatrix(1.0F, 0.0F, 0.0F, -m_viewReferencePoint.m_x,
	                               0.0F, 1.0F, 0.0F, -m_viewReferencePoint.m_y,
								   0.0F, 0.0F, 1.0F, -m_viewReferencePoint.m_z,
								   0.0F, 0.0F, 0.0F, 1.0F);

  Matrix<float4> rotationMatrix(viewRightNormal.X(), viewRightNormal.Y(), viewRightNormal.Z(), 0.0F,
                                  m_viewUpNormal.X(),    m_viewUpNormal.Y(),    m_viewUpNormal.Z(),  0.0F  ,
                                  m_viewPlaneNormal.X(), m_viewPlaneNormal.Y(), m_viewPlaneNormal.Z(), 0.0F,
	                              0.0F,                  0.0F,                  0.0F,                  1.0F);


  Matrix<float4> world2ViewMatrix  (rotationMatrix * translationMatrix);
  //   Projection Matrix.  Scree Coordinate (-1, 1, -1, 1)
   
  Matrix<float4> projectionMatrix;

  if(m_eProjectionType == ORTHOGRAPHIC) {
    projectionMatrix = Matrix<float4> (2.0F/m_orthoFrustumSize.m_x,  0.0F,   0.0F,  0.0F,
	                                   0.0F,                      2.0F/m_orthoFrustumSize.m_y,  0.0F,  0.0F,
									   0.0F,                      0.0F,   1.0F,  0.0F,
									   0.0F,                      0.0F,   0.0F,  1.0F);
  }

  if(m_eProjectionType == PERSPECTIVE) {
    projectionMatrix = Matrix<float4> (1.0F/m_tangentFovTriple.m_x,  0.0F,  0.0F,  0.0F,
	                                   0.0F,  1.0F/m_tangentFovTriple.m_y,  0.0F,  0.0F,
									   0.0F,  0.0F,  1.0F,  0.0F,
									   0.0F,  0.0F,  1.0F,  0.0F);
  }

  //   Viewport Matrix.  Screen Coordinate (0, imgWidth, 0, imgHeight)

  Matrix<float4> viewportMatrix (uImgWidth/2.0F,   0.0F,   0.0F,  uImgWidth/2.0F,
	                             0.0F,        uImgHeight/2.0F, 0.0F, uImgHeight/2.0F,
								 0.0F,        0.0F,   1.0F,  0.0F,
								 0.0F,        0.0F,   0.0F,  1.0F);

//  return(Matrix<float4> (world2ViewMatrix));
  return(Matrix<float4> (viewportMatrix * projectionMatrix * world2ViewMatrix));

}


// $Log: Camera.inl,v $
// Revision 1.2  2006/09/26 19:33:13  frank
// replaced hardcoded numbers with constants
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.7  2003/01/22 21:53:25  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.6  2002/05/07 18:31:49  frank
// Cleaned up includes.
//
// Revision 3.5  2002/03/11 15:25:25  ingmar
// removed calls to Normalize().  The whole point about class Normal is that it is always normalized. No need to call Normalize explicitly.
//
// Revision 3.4  2001/10/26 19:47:13  huamin
// Do not call MakeUpPerpendicular before computing the matrix
//
// Revision 3.3  2001/10/24 19:59:39  huamin
// Call  MakeVupPerpendicular() before compute the matrix
//
// Revision 3.2  2001/10/24 19:46:47  huamin
// Rewrite the comments for the matrix
//
// Revision 3.1  2001/10/24 19:17:22  huamin
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
//    Rev 2.0   Sep 16 2001 23:49:44   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:37:20   ingmar
// Initial revision.
// Revision 1.6  2001/07/30 15:26:24  huamin
// Added OrthoFrustumSize for Orthographic projection
//
// Revision 1.5  2001/07/20 16:07:42  frank
// Fixed everything.
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
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Camera.inl,v 1.2 2006/09/26 19:33:13 frank Exp $
// $Id: Camera.inl,v 1.2 2006/09/26 19:33:13 frank Exp $
