// $Id: Camera.C,v 1.3 2006/08/24 01:31:49 jmeade Exp $
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


#include "stdafx.h"
#include "Camera.h"

// define declarations
#define FILE_REVISION "$Revision: 1.3 $"

/**
 * Default constructor.
 */
Camera::Camera()
{
  m_viewReferencePoint = Point<float4>(0.0F, 0.0F, 0.0F);
  m_viewPlaneNormal = Normal<float4>(0.0F, 0.0F, 1.0F);
  m_viewUpNormal = Normal<float4>(0.0F, 1.0F, 0.0F);
  m_fieldOfViewRadiansTriple = Triple<float4>(90.0F, 90.0F, 90.0F);
  m_tangentFovTriple = Triple<float4>(tan(m_fieldOfViewRadiansTriple.m_x),
                                      tan(m_fieldOfViewRadiansTriple.m_y),
                                      tan(m_fieldOfViewRadiansTriple.m_z));
  m_sampleRateTriple = Triple<float4>(1.0F, 1.0F, 1.0F);
  m_eProjectionType = PERSPECTIVE;
} // Camera()


/**
 * Handy constructor.
 *
 * @param const Point<float4> & vrp   View reference point, i.e. Camera location
 * @param const Normal<float4> & vpn  View plane normal
 * @param const Normal<float4> & vup  View normal up vector
 */
Camera::Camera(const Point<float4> & vrp, const Normal<float4> & vpn, const Normal<float4> & vup)
{
  *this = Camera();
  SetVrp(vrp);
  SetVpn(vpn);
  SetVup(vup);
}


/**
 * Input stream Camera operator
 *
 */
std::istream& operator>>(std::istream& is, Camera& cam)
{
  char dummyChar;
  Vector<float4> tmp;
  std::string sVersion;
  is >> sVersion;

  char iOneChar=0;
  for(; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> cam.m_viewReferencePoint.m_x >> dummyChar >> cam.m_viewReferencePoint.m_y >> dummyChar >> cam.m_viewReferencePoint.m_z >> dummyChar; // read "Text:(#,#,#)"
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> tmp.m_x >> dummyChar >> tmp.m_y >> dummyChar >> tmp.m_z >> dummyChar; // read "Text:(#,#,#)"
  cam.m_viewPlaneNormal = Normal<float4>(tmp);
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> tmp.m_x >> dummyChar >> tmp.m_y >> dummyChar >> tmp.m_z >> dummyChar; // read "Text:(#,#,#)"
  cam.m_viewUpNormal = Normal<float4>(tmp);
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> cam.m_orthoFrustumSize.m_x >> dummyChar >> cam.m_orthoFrustumSize.m_y >> dummyChar >> cam.m_orthoFrustumSize.m_z >> dummyChar; // read "Text:(#,#,#)"
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> cam.m_fieldOfViewRadiansTriple.m_x >> dummyChar >> cam.m_fieldOfViewRadiansTriple.m_y >> dummyChar >> cam.m_fieldOfViewRadiansTriple.m_z >> dummyChar; // read "Text:(#,#,#)"
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> cam.m_tangentFovTriple.m_x >> dummyChar >> cam.m_tangentFovTriple.m_y >> dummyChar >> cam.m_tangentFovTriple.m_z >> dummyChar; // read "Text:(#,#,#)"
  
  for(iOneChar=0; iOneChar!= '('; is >> iOneChar ) if (!is.good()) throw ex::EOFException(LogRec("End of file reached while looking for triple","Camera","operator>>"));
  is >> cam.m_sampleRateTriple.m_x >> dummyChar >> cam.m_sampleRateTriple.m_y >> dummyChar >> cam.m_sampleRateTriple.m_z >> dummyChar; // read "Text:(#,#,#)"
  
  int4 iProjectType(0); is >> iProjectType; cam.m_eProjectionType = Camera::ProjectionTypeEnum(iProjectType);
  return is;
}


/**
 * Output stream Camera operator
 *
 */
std::ostream& operator<<(std::ostream& os, const Camera& cam)
{
  os << std::string("Camera-Version01") << std::ends << std::endl;

  os << "VRP:(" << cam.m_viewReferencePoint.m_x << "," << cam.m_viewReferencePoint.m_y << "," << cam.m_viewReferencePoint.m_z << ")";
  
  os << "VPN:(" << cam.m_viewPlaneNormal.X() << "," << cam.m_viewPlaneNormal.Y() << "," << cam.m_viewPlaneNormal.Z() << ")";
  
  os << "VUP:(" << cam.m_viewUpNormal.X() << "," << cam.m_viewUpNormal.Y() << "," << cam.m_viewUpNormal.Z() << ")";
  
  os << "Frustum:(" << cam.m_orthoFrustumSize.m_x << "," << cam.m_orthoFrustumSize.m_y << "," << cam.m_orthoFrustumSize.m_z << ")";
  
  os << "FOV:(" << cam.m_fieldOfViewRadiansTriple.m_x << "," << cam.m_fieldOfViewRadiansTriple.m_y << "," << cam.m_fieldOfViewRadiansTriple.m_z << ")";
  
  os << "TAN:(" << cam.m_tangentFovTriple.m_x << "," << cam.m_tangentFovTriple.m_y << "," << cam.m_tangentFovTriple.m_z << ")";
  
  os << "Sample:(" << cam.m_sampleRateTriple.m_x << "," << cam.m_sampleRateTriple.m_y << "," << cam.m_sampleRateTriple.m_z << ")";
  
  os << cam.m_eProjectionType;
  return os;
}


#undef FILE_REVISION

// $Log: Camera.C,v $
// Revision 1.3  2006/08/24 01:31:49  jmeade
// spacing.
//
// Revision 1.2  2006/06/01 20:25:50  frank
// updated to visual studio 2005
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.3  2002/10/29 19:35:20  kevin
// Normal read/write is not consistent. SO rather than risk breakign
// something else, I put a LARGE warnign in Normal read and
// fixed the Camera class I/O outside of the Normal elements.
//
// Revision 3.2  2002/09/27 19:01:12  jmeade
// Input/output stream operators.
//
// Revision 3.1  2001/12/12 20:02:12  jmeade
// Constructor for camera parameters.
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
// Revision 1.2  2001/07/20 16:08:00  frank
// initializes members now
//
// Revision 1.1  2001/07/20 15:32:28  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Camera.C,v 1.3 2006/08/24 01:31:49 jmeade Exp $
// $Id: Camera.C,v 1.3 2006/08/24 01:31:49 jmeade Exp $
