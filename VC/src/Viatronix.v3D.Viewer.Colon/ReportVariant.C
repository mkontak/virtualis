//////////////////////////////////////////////////////////////////////
// $Id: ReportVariant.C,v 1.3 2007/03/09 23:38:54 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: George Economos  geconomos@viatronix.net

#include "stdafx.h"
#include "viatron.h"
#include "ReportVariant.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ColonReport;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * default constructor
 **/
ReportVariant::ReportVariant()
{
  // no code
} // default constructor


/**
 * constructor
 * @param vQuaternion   view quaternion
 **/
ReportVariant::ReportVariant(const Quaternion<float4> & vQuaternion)
{
  CreateOneDim(VT_R4, 4);

  float v[4];      
  vQuaternion.GetArrayFromQuaternion(v);
  v[3] = vQuaternion[3];
  
  long nIndex[1];
  for(nIndex[0] = 0; nIndex[0] < 4; nIndex[0]++)
  PutElement(nIndex, (void*) &v[nIndex[0]]);
} // constructor


/**
 * constructor
 * @param mMatrix   view matrix
 **/
ReportVariant::ReportVariant(const Matrix<float> & mMatrix)
{
  DWORD dwNumElements[] = {4,4};
  
  Create(VT_R4, 2, dwNumElements);
  
  Matrix<float> m = mMatrix;
  
  long nIndex[2];
  for(nIndex[1]=0; nIndex[1] < 4; nIndex[1]++)
  {
    for(nIndex[0]=0; nIndex[0] < 4; nIndex[0]++)
      PutElement(nIndex, (void*)&(m(nIndex[0],  nIndex[1])));
  }
} // constructor


/**
 * constructor
 * @param vPoint   view point
 **/
ReportVariant::ReportVariant(const Point<float> & vPoint)
{
  CreateOneDim(VT_R4, 3);
  
  long nIndex[1] = {0};
  
  PutElement(nIndex, (void*) &vPoint.m_x);
  
  nIndex[0] +=1;
  PutElement(nIndex, (void*) &vPoint.m_y);
  
  nIndex[0] +=1;
  PutElement(nIndex, (void*) &vPoint.m_z);
} // constructor


/**
 * constructor
 * @param vNormal   view normal
 **/
ReportVariant::ReportVariant(const Normal<float> & vNormal)
{
  CreateOneDim(VT_R4, 3);
  
  long nIndex[1] = {0};
  
  float f = vNormal.X();
  PutElement(nIndex, (void*) &f);
  
  nIndex[0] +=1;
  f = vNormal.Y();
  PutElement(nIndex, (void*) &f);
  
  nIndex[0] +=1;
  f = vNormal.Z();
  PutElement(nIndex, (void*) &f);
} // constructor


/**
 * constructor
 * @param vPoint         view point
 * @param vOrientation   view orientation
 * @param vUp            view up vector
 **/
ReportVariant::ReportVariant(const Point<float4> & vPoint, const Normal<float4> & vOrientation, const Normal<float4> & vUp)
{
  CreateOneDim(VT_R4, 9);

  float f;
  long iIndex = 0;

  f = vPoint.X();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vPoint.Y();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vPoint.Z();
  PutElement(&iIndex, (void*) &f);  iIndex++;

  f = vOrientation.X();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vOrientation.Y();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vOrientation.Z();
  PutElement(&iIndex, (void*) &f);  iIndex++;

  f = vUp.X();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vUp.Y();
  PutElement(&iIndex, (void*) &f);  iIndex++;
  f = vUp.Z();
  PutElement(&iIndex, (void*) &f);  iIndex++;
} // constructor


/**
 * constructor
 * @param vVector   view point
 **/
ReportVariant::ReportVariant(const Vector<float> & vVector)
{
  CreateOneDim(VT_R4, 3);
  
  long nIndex[1] = {0};
  
  PutElement(nIndex, (void*) &vVector.m_x);
  
  nIndex[0] +=1;
  PutElement(nIndex, (void*) &vVector.m_y);
  
  nIndex[0] +=1;
  PutElement(nIndex, (void*) &vVector.m_z);
} // constructor


/**
 * destructor
 * @param vVector   view point
 **/
ReportVariant::~ReportVariant()
{

} // destructor


/**
 * type cast operator
 * @return   equivalent quaternion
 **/
Quaternion<float4> ReportVariant::asQuaternion()
{
  float v[4];
  
  long nIndex[1];
  for(nIndex[0] = 0; nIndex[0] < 4; nIndex[0]++)
   GetElement(nIndex, &v[nIndex[0]]);
  
  Quaternion<float4> quat;
	quat(v[0], v[1], v[2], v[3]);
  
  return quat;
} // asQuaternion()


/**
 * type cast operator
 * @return   equivalent matrix
 **/
Matrix<float> ReportVariant::asMatrix()
{
  float m[4][4];
  
  long nIndex[2];
  for(nIndex[1]=0; nIndex[1] < 4; nIndex[1]++)
  {
    for(nIndex[0]=0; nIndex[0] < 4; nIndex[0]++)
      GetElement(nIndex, (void*) &(m[nIndex[0]][nIndex[1]]));
  }
  
  Matrix<float> mat
  (
  m[0][0],  m[1][0],  m[2][0],  m[3][0],
  m[0][1],  m[1][1],  m[2][1],  m[3][1],
  m[0][2],  m[1][2],  m[2][2],  m[3][2],
  m[0][3],  m[1][3],  m[2][3],  m[3][3]
  );

  return mat;
} // asMatrix()


/**
 * type cast operator
 * @return   equivalent point
 **/
Point<float> ReportVariant::asPoint()
{
  Point<float> point;
  
  long nIndex[1] = {0};
  
  GetElement(nIndex, (void*) &point.m_x);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &point.m_y);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &point.m_z);  

  return point;
} // asPoint()


/**
 * type cast operator
 * @return   equivalent view vector
 **/
Vector<float> ReportVariant::asVector()
{
  Vector<float> v;
  
  long nIndex[1] = {0};
  
  GetElement(nIndex, (void*) &v.m_x);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &v.m_y);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &v.m_z);  
  
  return v;
} // asVector()


/**
 * type cast operator
 * @return   equivalent normal
 **/
Normal<float> ReportVariant::asNormal()
{
  long nIndex[1] = {0};

  float f[3];
  
  GetElement(nIndex, (void*) &f[0]);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &f[1]);
  
  nIndex[0] +=1;
  GetElement(nIndex, (void*) &f[2]);  

  return Normal<float>(f[0], f[1], f[2]);
} // asNormal()


/**
 * type cast operator
 * @return   equivalent camera coordinate values
 **/
void ReportVariant::asCameraCoordinates(Point<float4> & vPoint, Normal<float4> & vOrientation, Normal<float4> & vUp)
{
  long iIndex = 0;

  float f[3];

  GetElement(&iIndex, (void*) &f[0]); iIndex++;
  GetElement(&iIndex, (void*) &f[1]); iIndex++;
  GetElement(&iIndex, (void*) &f[2]); iIndex++;
  vPoint = Point<float>(f[0], f[1], f[2]);

  GetElement(&iIndex, (void*) &f[0]); iIndex++;
  GetElement(&iIndex, (void*) &f[1]); iIndex++;
  GetElement(&iIndex, (void*) &f[2]); iIndex++;
  vOrientation = Normal<float>(f[0], f[1], f[2]);

  GetElement(&iIndex, (void*) &f[0]); iIndex++;
  GetElement(&iIndex, (void*) &f[1]); iIndex++;
  GetElement(&iIndex, (void*) &f[2]); iIndex++;
  vUp = Normal<float>(f[0], f[1], f[2]);
} // asCameraCoordinates()



// Revision History:
// $Log: ReportVariant.C,v $
// Revision 1.3  2007/03/09 23:38:54  jmeade
// code standards.
//
// Revision 1.2  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.0.20.1  2004/07/17 03:00:07  jmeade
// Camera coordinates to a variant stream.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:42   ingmar
// Initial revision.
// Revision 1.8  2001/05/24 22:17:10  jmeade
// Quaternion::GetVectorFromQuatenion() changed to GetArrayFromQuaternion()
//
// Revision 1.7  2001/04/13 02:34:23  jmeade
// headers and footers
//
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportVariant.C,v 1.3 2007/03/09 23:38:54 jmeade Exp $
// $Id: ReportVariant.C,v 1.3 2007/03/09 23:38:54 jmeade Exp $
