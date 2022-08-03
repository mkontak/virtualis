// $Id: vxTestPoint3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#include "stdafx.h"
#include "vxTestPoint3D.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"
#include "utilities.h"


#define FILE_REVISION "$Revision: 1.3 $"

using namespace std;

/**
 * Unit test of vxVolumeIterators stuff
 * @param out stream
 * @return success
 */
bool vxTestPoint3D::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateConstructors",  ValidateConstructors, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateAssignment",  ValidateAssignment, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateIndexAccess",  ValidateIndexAccess, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateComparators",  ValidateComparators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateGetMaxComponent",  ValidateGetMaxComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateGetMaxAbsComponent",  ValidateGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateGetMaxComponentCoord",  ValidateGetMaxComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateGetMaxAbsComponentCoord",  ValidateGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateGetScaledDividedUnIso",  ValidateGetScaledDividedUnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateEuclideanDistance",  ValidateEuclideanDistance, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateScalarMultDiv",  ValidateScalarMultDiv, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateVectorPlusMinus",  ValidateVectorPlusMinus, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateClamp",  ValidateClamp, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidatePermute",  ValidatePermute, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateScaleDivideAnIso",  ValidateScaleDivideAnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateNonConstOperators",  ValidateNonConstOperators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "ValidateStreamIO",  ValidateStreamIO, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestPoint3D::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "TestSpeedGetMaxAbsComponent",  TestSpeedGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestPoint3D", "TestSpeedGetMaxAbsComponentCoord",  TestSpeedGetMaxAbsComponentCoord, bOk);

  return bOk;
} // PerformanceTest()


////////////////////////////////////////////////////////////////////////
/// Demo

/**
 * Demonstrate basic class functionality
 * @param stream
 * @return true if success
 */
bool vxTestPoint3D::Demo(std::ostream & stream)
{
  Point3Df point3D(1.5F, 0, 0);

  LogFyi("point3D=" + point3D.ToString(), "vxTestPoint3D", "Demo");
  return(true);
} // Demo



////////////////////////////////////////////////////////////////////////
/// Validation

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateConstructors(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a;                                       bOk &= (a.X() == 0    && a.Y() == 0    && a.Z() == 0); 
  Point3Df b(1,2,3);                                bOk &= (b.X() == 1    && b.Y() == 2    && b.Z() == 3);
  Point3Df c(b);                                    bOk &= (c.X() == 1    && c.Y() == 2    && c.Z() == 3);

  // copy constructors
  Triplef   triple  (1,2,3);  Point3Df d(triple );  bOk &= (d == Point3Df(1,2,3));
  Point3Df  point3D (1,2,3);  Point3Df e(point3D);  bOk &= (e == Point3Df(1,2,3));
  Vector3Df vector3D(1,2,3);  Point3Df f(vector3D); bOk &= (f == Point3Df(1,2,3));

  return(bOk);
} // ValidateConstructors


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateAssignment(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a;   a.SetXYZ(1,2,3);             bOk &= (a == Point3Df(1,2,3));
  Point3Df b;   b = a;                       bOk &= (b == Point3Df(1,2,3));
  Point3Df c;   c.X()=1; c.Y()=2; c.Z()=3;   bOk &= (c == Point3Df(1,2,3));
  Point3Df d;   d[0] =1; d[1] =2; d[2] =3;   bOk &= (d == Point3Df(1,2,3));
  return(bOk);
} // ValidateAssignment()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateIndexAccess(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a(1,2,3);     bOk &= (a[0] == 1    && a[1] == 2    && a[2] == 3); 

  return(bOk);
} // ValidateIndexAccess()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateComparators(std::ostream & stream)
{
  bool bOk(true);

  // default int
  Point3Di a(1,2,3);     bOk &= (a == a); 
  Point3Di b(1,2,3);     bOk &= (a == b); 
  Point3Di c(1,0,0);     bOk &= (a != c); 

  // default float
  Point3Df d(1,2,3);     bOk &= (d == d); 
  Point3Df e(1,2,3);     bOk &= (d == e); 
  Point3Df f(1,0,0);     bOk &= (d != f); 
  
  return(bOk);
} // ValidateComparators()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateGetMaxComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a( 1, 0, 0); bOk &= (a.GetMaxComponent() ==  1.0F);
  Point3Df b(-1, 0, 0); bOk &= (b.GetMaxComponent() ==  0.0F);
  Point3Df c( 0, 1, 0); bOk &= (c.GetMaxComponent() ==  1.0F);
  Point3Df d( 0,-1, 0); bOk &= (d.GetMaxComponent() ==  0.0F);
  Point3Df e( 0, 0, 1); bOk &= (e.GetMaxComponent() ==  1.0F);
  Point3Df f( 0, 0,-1); bOk &= (f.GetMaxComponent() ==  0.0F);

  return(bOk);
} // ValidateGetMaxComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponent() ==  1.0F);
  Point3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponent() == -1.0F);
  Point3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponent() ==  1.0F);
  Point3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponent() == -1.0F);
  Point3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponent() ==  1.0F);
  Point3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponent() == -1.0F);

  return(bOk);
} // ValidateGetMaxAbsComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateGetMaxComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a( 1, 0, 0); bOk &= (a.GetMaxComponentCoord() == 0);
  Point3Df b(-1, 0, 0); bOk &= (b.GetMaxComponentCoord() == 2);
  Point3Df c( 0, 1, 0); bOk &= (c.GetMaxComponentCoord() == 1);
  Point3Df d( 0,-1, 0); bOk &= (d.GetMaxComponentCoord() == 2);
  Point3Df e( 0, 0, 1); bOk &= (e.GetMaxComponentCoord() == 2);
  Point3Df f( 0, 0,-1); bOk &= (f.GetMaxComponentCoord() == 1);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponentCoord() == 0);
  Point3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponentCoord() == 0);
  Point3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponentCoord() == 1);
  Point3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponentCoord() == 1);
  Point3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponentCoord() == 2);
  Point3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponentCoord() == 2);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateGetScaledDividedUnIso(std::ostream & stream)
{
  bool bOk(true);

  Point3Df a(1,1,1);  Triplef c(2,3,4);  bOk &= (a.GetScaledAnIso(c)  == Point3Df(2,3,4)); 
  Point3Df b(2,3,4);  Triplef d(2,3,4);  bOk &= (b.GetDividedAnIso(d) == Point3Df(1,1,1)); 

  return(bOk);
} // ValidateGetScaledDividedUnIso()


/**
 * @param stream
 * @return success or failure of validation.
 */
bool vxTestPoint3D::ValidateEuclideanDistance(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a(0,0,0); 
  Point3Df b(1,0,0);      bOk &= (a.GetEuclideanDistance(b) == 1); 
  Point3Df c(0,2,0);      bOk &= (a.GetEuclideanDistance(c) == 2); 
  Point3Df d(0,0,3);      bOk &= (a.GetEuclideanDistance(d) == 3);

  return(bOk);
} // ValidateEuclideanDistance()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const operators  


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateScalarMultDiv(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df v; 
  Point3Df a(1,2,3);   

  v=2.0F*a;  bOk &= (v == Point3Df(2,4,6));   
  v=a*2;     bOk &= (v == Point3Df(2,4,6));   
  v=a/0.5F;  bOk &= (v == Point3Df(2,4,6));   

  return(bOk);
} // ValidateScalarMultDiv()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateVectorPlusMinus(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df  point3D (1,2,3); 
  Vector3Df vector3D(1,2,3); 

  bOk &= (point3D-point3D  == Vector3Df(0,0,0));      
  bOk &= (point3D-vector3D ==  Point3Df(0,0,0));     
  bOk &= (point3D+vector3D ==  Point3Df(2,4,6));     

  return(bOk);
} // ValidateScalarMultDiv()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateClamp(std::ostream & stream)
{
  bool bOk(true);

  Point3Df a(1,2,3);   a.Clamp(1.5F, 2.5F);  bOk &= (a == Point3Df(1.5F,2,2.5F));

  return(bOk);
} // ValidateClamp()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidatePermute(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Point3Df a(1,0,0);   
  Point3Df b(0,1,0);   
  Point3Df c(0,0,1);   
  Point3Df v(a);   

  v.Permute(1);    bOk &= (v == b);   
  v.Permute(1);    bOk &= (v == c);   
  v.Permute(1);    bOk &= (v == a);   

  v.Permute( 2);   bOk &= (v == c);   
  v.Permute( 1);   bOk &= (v == a);   
  v.Permute( 0);   bOk &= (v == a);   
  v.Permute(-1);   bOk &= (v == c);   
  v.Permute(-2);   bOk &= (v == a);   

  return(bOk);
} // ValidatePermute()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateScaleDivideAnIso(std::ostream & stream)
{
  bool bOk(true);

  Point3Df a(1,1,1);   a.ScaleAnIso (Triplef(2,3,4));  bOk &= (a == Point3Df(2,3,4));
  Point3Df b(2,3,4);   b.DivideAnIso(Triplef(2,3,4));  bOk &= (b == Point3Df(1,1,1));

  return(bOk);
} // ValidateScaleDivideAnIso()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateNonConstOperators(std::ostream & stream)
{
  bool bOk(true);

  Point3Df a(1,1,1);   a *= 2;                 bOk &= (a == Point3Df(2,2,2));
  Point3Df b(1,1,1);   b /= 0.5F;              bOk &= (b == Point3Df(2,2,2));

  Point3Df c(1,1,1);   c += Vector3Df(1,2,3);  bOk &= (c == Point3Df(2,3,4));
  Point3Df d(2,3,4);   d -= Vector3Df(1,2,3);  bOk &= (d == Point3Df(1,1,1));

  return(bOk);
} // ValidateNonConstOperators()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const operators


/**
  * @param stream
* @return success or falilure of validation.
 */
bool vxTestPoint3D::ValidateStreamIO(std::ostream & stream)
{
  bool bOk(true);

  Point3Df a(1,2,3);
  Point3Df b(0,0,0);
  std::strstream ioStream; 
  ioStream << a;         ioStream >> b;       bOk &= (b == Point3Df(1,2,3));
  ioStream << " 2 3 4 "; ioStream >> b;       bOk &= (b == Point3Df(2,3,4));
  ioStream << " " << a;  ioStream >> b;       bOk &= (b == Point3Df(1,2,3));
  ioStream <<  "2 3 4";  ioStream >> b;       bOk &= (b == Point3Df(2,3,4));

  return(bOk);
} // ValidateStreamIO()


////////////////////////////////////////////////////////////////////////
/// Performance test


vector<Point3Df> & GetRandomArrayFloat4()
{
  static vector<Point3Df> array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Point3Df(rand(), rand(), rand());
    array[k+1] = -1.0F * array[k];
  }
  return array;
} // GetRandomArrayFloat4()


vector<Point3D<float8> > & GetRandomArrayFloat8()
{
  static vector<Point3D<float8> > array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Point3D<float8>(rand(), rand(), rand());
    array[k+1] = -1.0 * array[k];
  }
  return array;
} // GetRandomArrayFloat8()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return true if success
 */
bool vxTestPoint3D::TestSpeedGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Point3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Point3Df iterations; sum="+ToAscii(sum), "vxTestPoint3D", "TestSpeedGetMaxAbsComponent");
  } // Point3Df
  {
    vector<Point3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Point3D<float8> iterations; sum="+ToAscii(sum), "vxTestPoint3D", "TestSpeedGetMaxAbsComponent");
  } // Point3D<float8>

  return(true);

} // TestSpeedGetMaxAbsComponent()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return true if success
 */
bool vxTestPoint3D::TestSpeedGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Point3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Point3Df iterations; sum="+ToAscii(sum), "vxTestPoint3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Point3Df
  {
    vector<Point3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Point3D<float8> iterations; sum="+ToAscii(sum), "vxTestPoint3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Point3D<float8>
  return(true);

} // TestSpeedGetMaxAbsComponentCoord()

#undef FILE_REVISION


// $Log: vxTestPoint3D.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/11/19 18:50:49  vxconfig
// code review
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.1  2003/05/20 15:53:04  dongqing
// initialization
//
// initial version
//
// $Id: vxTestPoint3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $


