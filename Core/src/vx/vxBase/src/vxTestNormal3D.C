// $Id: vxTestNormal3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#include "stdafx.h"
#include "vxTestNormal3D.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"
#include "utilities.h"

#include "Timer.h"

#define FILE_REVISION "$Revision: 1.3 $"

using namespace std;

/**
 * Unit test of vxVolumeIterators stuff
 * @param out stream
 * @return success
 */
bool vxTestNormal3D::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateConstructors",  ValidateConstructors, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateAssignment",  ValidateAssignment, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateIndexAccess",  ValidateIndexAccess, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateComparators",  ValidateComparators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateGetMaxComponent",  ValidateGetMaxComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateGetMaxAbsComponent",  ValidateGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateGetMaxComponentCoord",  ValidateGetMaxComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateGetMaxAbsComponentCoord",  ValidateGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateGetScaledAnIso",  ValidateGetScaledAnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateDotAndCrossProducts",  ValidateDotAndCrossProducts, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateUnaryPlusMinus",  ValidateUnaryPlusMinus, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateScalarMultDiv",  ValidateScalarMultDiv, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidatePermute",  ValidatePermute, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateNormalize",  ValidateNormalize, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "ValidateStreamIO",  ValidateStreamIO, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestNormal3D::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "TestSpeedGetMaxAbsComponent",  TestSpeedGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "TestSpeedGetMaxAbsComponentCoord",  TestSpeedGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestNormal3D", "Demo",  Demo, bOk);

  return bOk;
} // PerformanceTest()


////////////////////////////////////////////////////////////////////////
/// Demo

/**
 * Demonstrate basic class functionality
 * @param stream
 * @return ture if success
 */
bool vxTestNormal3D::Demo(std::ostream & stream)
{
  Normal3Df fNormal3D(1.5F, 0, 0);

  LogFyi("fNormal3D=" + fNormal3D.ToString(), "vxTestNormal3D", "Demo");
  return(true);
} // Demo



////////////////////////////////////////////////////////////////////////
/// Validation

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateConstructors(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a;                                       bOk &= (a.X() == 0    && a.Y() == 0    && a.Z() == 1); 
  Normal3Df b(2,0,0);                                bOk &= (b.X() == 1    && b.Y() == 0    && b.Z() == 0); 
  Normal3Df c(b);                                    bOk &= (c.X() == 1    && c.Y() == 0    && c.Z() == 0); 

  // copy constructors
  Triplef   triple  (2,0,0);  Normal3Df d(triple );  bOk &= (d.X() == 1    && d.Y() == 0    && d.Z() == 0); 
  Point3Df  point3D (0,2,0);  Normal3Df e(point3D);  bOk &= (e.X() == 0    && e.Y() == 1    && e.Z() == 0); 
  Vector3Df vector3D(0,0,2);  Normal3Df f(vector3D); bOk &= (f.X() == 0    && f.Y() == 0    && f.Z() == 1); 

  return(bOk);
} // ValidateConstructors


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateAssignment(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a;       a.SetXYZ(2, 0, 0);     bOk &= (a.X() == 1    && a.Y() == 0    && a.Z() == 0); 
  Normal3Df b;       b = a;                 bOk &= (b.X() == 1    && b.Y() == 0    && b.Z() == 0); 

  return(bOk);
} // ValidateAssignment()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateIndexAccess(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);     bOk &= (a[0] == 1    && a[1] == 0    && a[2] == 0); 
  Normal3Df b(0,1,0);     bOk &= (b[0] == 0    && b[1] == 1    && b[2] == 0); 
  Normal3Df c(0,0,1);     bOk &= (c[0] == 0    && c[1] == 0    && c[2] == 1); 

  return(bOk);
} // ValidateIndexAccess()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateComparators(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);     bOk &= (a == a); 
  Normal3Df b(1,0,0);     bOk &= (a == b); 
  Normal3Df c(0,0,1);     bOk &= (a != c); 

  return(bOk);
} // ValidateComparators()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateGetMaxComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a( 1, 0, 0); bOk &= (a.GetMaxComponent() ==  1.0F);
  Normal3Df b(-1, 0, 0); bOk &= (b.GetMaxComponent() ==  0.0F);
  Normal3Df c( 0, 1, 0); bOk &= (c.GetMaxComponent() ==  1.0F);
  Normal3Df d( 0,-1, 0); bOk &= (d.GetMaxComponent() ==  0.0F);
  Normal3Df e( 0, 0, 1); bOk &= (e.GetMaxComponent() ==  1.0F);
  Normal3Df f( 0, 0,-1); bOk &= (f.GetMaxComponent() ==  0.0F);

  return(bOk);
} // ValidateGetMaxComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponent() ==  1.0F);
  Normal3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponent() == -1.0F);
  Normal3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponent() ==  1.0F);
  Normal3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponent() == -1.0F);
  Normal3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponent() ==  1.0F);
  Normal3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponent() == -1.0F);

  return(bOk);
} // ValidateGetMaxAbsComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateGetMaxComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a( 1, 0, 0); bOk &= (a.GetMaxComponentCoord() == 0);
  Normal3Df b(-1, 0, 0); bOk &= (b.GetMaxComponentCoord() == 2);
  Normal3Df c( 0, 1, 0); bOk &= (c.GetMaxComponentCoord() == 1);
  Normal3Df d( 0,-1, 0); bOk &= (d.GetMaxComponentCoord() == 2);
  Normal3Df e( 0, 0, 1); bOk &= (e.GetMaxComponentCoord() == 2);
  Normal3Df f( 0, 0,-1); bOk &= (f.GetMaxComponentCoord() == 1);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponentCoord() == 0);
  Normal3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponentCoord() == 0);
  Normal3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponentCoord() == 1);
  Normal3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponentCoord() == 1);
  Normal3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponentCoord() == 2);
  Normal3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponentCoord() == 2);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateGetScaledAnIso(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);  Triplef d(2,3,4);  bOk &= (a.GetScaledAnIso(d) == Vector3Df(2,0,0)); 
  Normal3Df b(0,1,0);  Triplef e(2,3,4);  bOk &= (b.GetScaledAnIso(e) == Vector3Df(0,3,0)); 
  Normal3Df c(0,0,1);  Triplef f(2,3,4);  bOk &= (c.GetScaledAnIso(f) == Vector3Df(0,0,4)); 

  return(bOk);
} // ValidateGetScaledAnIso()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateDotAndCrossProducts(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);      bOk &= (a.Dot(a) == 1); 
  Normal3Df b(0,1,0);      bOk &= (b.Dot(b) == 1); 
  Normal3Df c(0,0,1);      bOk &= (c.Dot(c) == 1  &&  c.Dot(b) == 0  &&  c.Dot(a) == 0); 

  Vector3Df vector3D(2,2,2); bOk &= (a.Dot(vector3D) == 2); 
  Normal3Df normal3D(1,0,0); bOk &= (a.Dot(normal3D) == 1); 

  bOk &= (a.Cross(b) == Vector3Df(c));
  bOk &= (b.Cross(c) == Vector3Df(a));
  bOk &= (c.Cross(a) == Vector3Df(b));

  bOk &= (a.Cross(Vector3Df(b)) == Vector3Df(c));
  
  return(bOk);
} // ValidateDotAndCrossProducts()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateUnaryPlusMinus(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);  
  Normal3Df b(0,1,0);  
  Normal3Df c(0,0,1);  

  a = +a;  bOk &= (a.X() ==  1    && a.Y() ==  0    && a.Z() ==  0);  
  b = +b;  bOk &= (b.X() ==  0    && b.Y() ==  1    && b.Z() ==  0);  
  c = +c;  bOk &= (c.X() ==  0    && c.Y() ==  0    && c.Z() ==  1);  

  a = -a;  bOk &= (a.X() == -1    && a.Y() ==  0    && a.Z() ==  0);  
  b = -b;  bOk &= (b.X() ==  0    && b.Y() == -1    && b.Z() ==  0);  
  c = -c;  bOk &= (c.X() ==  0    && c.Y() ==  0    && c.Z() == -1);  

  return(bOk);
} // ValidateUnaryPlusMinus()



/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateScalarMultDiv(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df v; 
  Normal3Df a(1,0,0);   
  Normal3Df b(0,1,0);   
  Normal3Df c(0,0,1);   

  v=2*a;    bOk &= (v.X() == 2  &&  v.Y() == 0  &&  v.Z() == 0);   
  v=2*b;    bOk &= (v.X() == 0  &&  v.Y() == 2  &&  v.Z() == 0);   
  v=2*c;    bOk &= (v.X() == 0  &&  v.Y() == 0  &&  v.Z() == 2);   
                                                   
  v=a*2;    bOk &= (v.X() == 2  &&  v.Y() == 0  &&  v.Z() == 0);   
  v=b*2;    bOk &= (v.X() == 0  &&  v.Y() == 2  &&  v.Z() == 0);   
  v=c*2;    bOk &= (v.X() == 0  &&  v.Y() == 0  &&  v.Z() == 2);   
                                                   
  v=a/0.5F; bOk &= (v.X() == 2  &&  v.Y() == 0  &&  v.Z() == 0);   
  v=b/0.5F; bOk &= (v.X() == 0  &&  v.Y() == 2  &&  v.Z() == 0);   
  v=c/0.5F; bOk &= (v.X() == 0  &&  v.Y() == 0  &&  v.Z() == 2);   

  return(bOk);
} // ValidateScalarMultDiv()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidatePermute(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(1,0,0);   
  Normal3Df b(0,1,0);   
  Normal3Df c(0,0,1);   
  Normal3Df v(a);   

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
bool vxTestNormal3D::ValidateNormalize(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Normal3Df a(2,0,0);   bOk &= (a.X() == 1    && a.Y() == 0    && a.Z() == 0);
  Normal3Df b(0,2,0);   bOk &= (b.X() == 0    && b.Y() == 1    && b.Z() == 0);
  Normal3Df c(0,0,2);   bOk &= (c.X() == 0    && c.Y() == 0    && c.Z() == 1);
  
  // we no longer expect normalize to throw an exception for release code.
  // we may enable throwing an exception during development periods, but no
  // longer in a release.
  //try
  //{
  //  // should fail
  //  Normal3Df n(0,0,0);
  //  // should not get here
  //  bOk = false;
  //}
  //catch (...)
  //{
  //  // should be caught here
  //  bOk &= true;
  //}
  
  return(bOk);
} // ValidateNormalize()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestNormal3D::ValidateStreamIO(std::ostream & stream)
{
  bool bOk(true);

  Normal3Df a(1,0,0);
  Normal3Df b(0,1,0);
  std::stringstream ioStream; 
  ioStream << a;         ioStream >> b;       bOk &= (b == Normal3Df(1,0,0));
  ioStream << " 0 0 1 "; ioStream >> b;       bOk &= (b == Normal3Df(0,0,1));
  ioStream << " " << a;  ioStream >> b;       bOk &= (b == Normal3Df(1,0,0));
  ioStream <<  "0 0 1";  ioStream >> b;       bOk &= (b == Normal3Df(0,0,1));
  return(bOk);
} // ValidateStreamIO()


////////////////////////////////////////////////////////////////////////
/// Performance test


vector<Normal3Df> & GetRandomArrayFloat4()
{
  static vector<Normal3Df> array(1000000);
  for (int4 k=0;  k<array.size();  k+=2) 
  {
    array[k  ] = Normal3Df(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat4()


vector<Normal3D<float8> > & GetRandomArrayFloat8()
{
  static vector<Normal3D<float8> > array(1000000);
  for (int4 k=0;  k<array.size();  k+=2) 
  {
    array[k  ] = Normal3D<float8>(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat8()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return ture if success
 */
bool vxTestNormal3D::TestSpeedGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Normal3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Normal3Df iterations; sum="+ToAscii(sum), "vxTestNormal3D", "TestSpeedGetMaxAbsComponent");
  } // Normal3Df
  {
    vector<Normal3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Normal3D<float8> iterations; sum="+ToAscii(sum), "vxTestNormal3D", "TestSpeedGetMaxAbsComponent");
  } // Normal3D<float8>

  return(bOk);
} // TestSpeedGetMaxAbsComponent()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return ture if success
 */
bool vxTestNormal3D::TestSpeedGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Normal3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Normal3Df iterations; sum="+ToAscii(sum), "vxTestNormal3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Normal3Df
  {
    vector<Normal3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Normal3D<float8> iterations; sum="+ToAscii(sum), "vxTestNormal3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Normal3D<float8>

  return(bOk);
} // TestSpeedGetMaxAbsComponentCoord()


#undef FILE_REVISION


// $Log: vxTestNormal3D.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/09/26 12:41:44  vxconfig
// no more exception for normalizing zero vector
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.1  2003/05/20 14:53:51  dongqing
// initialization
//
// initial version
//
// $Id: vxTestNormal3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $


