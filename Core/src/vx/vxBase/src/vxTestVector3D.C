// $Id: vxTestVector3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#include "stdafx.h"
#include "vxTestVector3D.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"
#include "utilities.h"
#include "Enums.h"

#include "Timer.h"

#define FILE_REVISION "$Revision: 1.3 $"

using namespace std;

/**
 * Unit test of vxVolumeIterators stuff
 * @param out stream
 * @return success
 */
bool vxTestVector3D::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateConstructors",  ValidateConstructors, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateAssignment",  ValidateAssignment, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateIndexAccess",  ValidateIndexAccess, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateComparators",  ValidateComparators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateGetMaxComponent",  ValidateGetMaxComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateGetMaxAbsComponent",  ValidateGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateGetMaxComponentCoord",  ValidateGetMaxComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateGetMaxAbsComponentCoord",  ValidateGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateGetScaledDividedUnIso",  ValidateGetScaledDividedUnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateDotAndCrossProducts",  ValidateDotAndCrossProducts, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateDistances",  ValidateDistances, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateUnaryPlusMinus",  ValidateUnaryPlusMinus, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateScalarMultDiv",  ValidateScalarMultDiv, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateVectorPlusMinus",  ValidateVectorPlusMinus, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateClamp",  ValidateClamp, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateNormalize",  ValidateNormalize, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidatePermute",  ValidatePermute, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateScaleDivideAnIso",  ValidateScaleDivideAnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateNonConstOperators",  ValidateNonConstOperators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "ValidateStreamIO",  ValidateStreamIO, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestVector3D::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "TestSpeedGetMaxAbsComponent",  TestSpeedGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "TestSpeedGetMaxAbsComponentCoord",  TestSpeedGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestVector3D", "Demo",  Demo, bOk);

  return bOk;
} // PerformanceTest()


////////////////////////////////////////////////////////////////////////
/// Demo

/**
 * Demonstrate basic class functionality
 * @param stream
 * @return true if success
 */
bool vxTestVector3D::Demo(std::ostream & stream)
{
  Vector3Df vector3D(1.5F, 0, 0);

  LogFyi("vector3D=" + vector3D.ToString(), "vxTestVector3D", "Demo");

  return(true);
} // Demo



////////////////////////////////////////////////////////////////////////
/// Validation

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor & Assignment

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateConstructors(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a;                                        bOk &= (a.X() == 0    && a.Y() == 0    && a.Z() == 0); 
  Vector3Df b(1,2,3);                                 bOk &= (b.X() == 1    && b.Y() == 2    && b.Z() == 3); 
  Vector3Df c(b);                                     bOk &= (c.X() == 1    && c.Y() == 2    && c.Z() == 3); 

  // copy constructors                               
  Triplef   triple  (1,2,3);  Vector3Df d(triple );   bOk &= (d == Vector3Df(1,2,3));
  Point3Df  point3D (1,2,3);  Vector3Df e(point3D);   bOk &= (e == Vector3Df(1,2,3));
  Vector3Df vector3D(1,2,3);  Vector3Df f(vector3D);  bOk &= (f == Vector3Df(1,2,3));
  Normal3Df normal3D(1,0,0);  Vector3Df g(normal3D);  bOk &= (g == Vector3Df(1,0,0));

  return(bOk);
} // ValidateConstructors


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateAssignment(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a;   a.SetXYZ(1,2,3);             bOk &= (a == Vector3Df(1,2,3));
  Vector3Df b;   b = a;                       bOk &= (b == Vector3Df(1,2,3));
  Vector3Df c;   c.X()=1; c.Y()=2; c.Z()=3;   bOk &= (c == Vector3Df(1,2,3));
  Vector3Df d;   d[0] =1; d[1] =2; d[2] =3;   bOk &= (d == Vector3Df(1,2,3));

  return(bOk);
} // ValidateAssignment()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateIndexAccess(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a(1,2,3);     bOk &= (a[0] == 1    && a[1] == 2    && a[2] == 3); 

  return(bOk);
} // ValidateIndexAccess()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Comparators

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateComparators(std::ostream & stream)
{
  bool bOk(true);

  // default int
  Vector3Di a(1,2,3);     bOk &= (a == a); 
  Vector3Di b(1,2,3);     bOk &= (a == b); 
  Vector3Di c(1,0,0);     bOk &= (a != c); 

  // default float
  Vector3Df d(1,2,3);     bOk &= (d == d); 
  Vector3Df e(1,2,3);     bOk &= (d == e); 
  Vector3Df f(1,0,0);     bOk &= (d != f); 

  return(bOk);
} // ValidateComparators()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const info

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateGetMaxComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a( 1, 0, 0); bOk &= (a.GetMaxComponent() ==  1.0F);
  Vector3Df b(-1, 0, 0); bOk &= (b.GetMaxComponent() ==  0.0F);
  Vector3Df c( 0, 1, 0); bOk &= (c.GetMaxComponent() ==  1.0F);
  Vector3Df d( 0,-1, 0); bOk &= (d.GetMaxComponent() ==  0.0F);
  Vector3Df e( 0, 0, 1); bOk &= (e.GetMaxComponent() ==  1.0F);
  Vector3Df f( 0, 0,-1); bOk &= (f.GetMaxComponent() ==  0.0F);

  return(bOk);
} // ValidateGetMaxComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponent() ==  1.0F);
  Vector3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponent() == -1.0F);
  Vector3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponent() ==  1.0F);
  Vector3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponent() == -1.0F);
  Vector3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponent() ==  1.0F);
  Vector3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponent() == -1.0F);

  return(bOk);
} // ValidateGetMaxAbsComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateGetMaxComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a( 1, 0, 0); bOk &= (a.GetMaxComponentCoord() == 0);
  Vector3Df b(-1, 0, 0); bOk &= (b.GetMaxComponentCoord() == 2);
  Vector3Df c( 0, 1, 0); bOk &= (c.GetMaxComponentCoord() == 1);
  Vector3Df d( 0,-1, 0); bOk &= (d.GetMaxComponentCoord() == 2);
  Vector3Df e( 0, 0, 1); bOk &= (e.GetMaxComponentCoord() == 2);
  Vector3Df f( 0, 0,-1); bOk &= (f.GetMaxComponentCoord() == 1);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a( 1, 0, 0); bOk &= (a.GetMaxAbsComponentCoord() == 0);
  Vector3Df b(-1, 0, 0); bOk &= (b.GetMaxAbsComponentCoord() == 0);
  Vector3Df c( 0, 1, 0); bOk &= (c.GetMaxAbsComponentCoord() == 1);
  Vector3Df d( 0,-1, 0); bOk &= (d.GetMaxAbsComponentCoord() == 1);
  Vector3Df e( 0, 0, 1); bOk &= (e.GetMaxAbsComponentCoord() == 2);
  Vector3Df f( 0, 0,-1); bOk &= (f.GetMaxAbsComponentCoord() == 2);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or failure of validation.
 */
bool vxTestVector3D::ValidateDistances(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,0,0);      
  Vector3Df b(0,2,0);      
  Vector3Df c(0,0,3);      

  bOk &= (a.GetLengthSquared() == 1); 
  bOk &= (b.GetLengthSquared() == 4); 
  bOk &= (c.GetLengthSquared() == 9);   

  bOk &= (a.GetLength() == 1); 
  bOk &= (b.GetLength() == 2); 
  bOk &= (c.GetLength() == 3); 

  bOk &= (a.GetLengthChessBoard() == 1); 
  bOk &= (b.GetLengthChessBoard() == 2); 
  bOk &= (c.GetLengthChessBoard() == 3); 

  bOk &= (a.GetLengthCityBlock() == 1); 
  bOk &= (b.GetLengthCityBlock() == 2); 
  bOk &= (c.GetLengthCityBlock() == 3); 

  return(bOk);
} // ValidateDistances()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateGetScaledDividedUnIso(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a(1,1,1);  Triplef c(2,3,4);  bOk &= (a.GetScaledAnIso(c)  == Vector3Df(2,3,4)); 
  Vector3Df b(2,3,4);  Triplef d(2,3,4);  bOk &= (b.GetDividedAnIso(d) == Vector3Df(1,1,1)); 

  return(bOk);
} // ValidateGetScaledDividedUnIso()


/**
 * @param stream
 * @return success or failure of validation.
 */
bool vxTestVector3D::ValidateDotAndCrossProducts(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a(2,0,0);      bOk &= (a.Dot(a) == 4); 
  Vector3Df b(0,2,0);      bOk &= (b.Dot(b) == 4); 
  Vector3Df c(0,0,2);      bOk &= (c.Dot(c) == 4  &&  c.Dot(b) == 0  &&  c.Dot(a) == 0); 

  Vector3Df vector3D(2,2,2); bOk &= (a.Dot(vector3D) == 4); 
  Normal3Df normal3D(1,0,0); bOk &= (a.Dot(normal3D) == 2); 

  bOk &= (a.Cross(b) == Vector3Df(c*2));
  bOk &= (b.Cross(c) == Vector3Df(a*2));
  bOk &= (c.Cross(a) == Vector3Df(b*2));

  bOk &= (a.Cross(Normal3Df(b)) == Vector3Df(c));

  return(bOk);
} // ValidateDotAndCrossProducts()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const operators  

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateUnaryPlusMinus(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,2,3);  

  a = +a;    bOk &= (a == Vector3Df( 1, 2, 3));  
  a = -a;    bOk &= (a == Vector3Df(-1,-2,-3));  

  return(bOk);
} // ValidateUnaryPlusMinus()



/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateScalarMultDiv(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df v; 
  Vector3Df a(1,2,3);   

  v=2.0F*a;  bOk &= (v == Vector3Df(2,4,6));   
  v=a*2;     bOk &= (v == Vector3Df(2,4,6));   
  v=a/0.5F;  bOk &= (v == Vector3Df(2,4,6));   

  return(bOk);
} // ValidateScalarMultDiv()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateVectorPlusMinus(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a(1,2,3); 

  bOk &= (a-a == Vector3Df(0,0,0));      
  bOk &= (a+a == Vector3Df(2,4,6));     

  return(bOk);
} // ValidateVectorPlusMinus()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateClamp(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,2,3);   a.Clamp(1.5F, 2.5F);  bOk &= (a == Vector3Df(1.5F,2,2.5F));

  return(bOk);
} // ValidateClamp()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidatePermute(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Vector3Df a(1,0,0);   
  Vector3Df b(0,1,0);   
  Vector3Df c(0,0,1);   
  Vector3Df v(a);   

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
bool vxTestVector3D::ValidateNormalize(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(2,0,0);   a.Normalize();  bOk &= (a == Vector3Df(1,0,0)); 
  Vector3Df b(0,2,0);   b.Normalize();  bOk &= (b == Vector3Df(0,1,0)); 
  Vector3Df c(0,0,2);   c.Normalize();  bOk &= (c == Vector3Df(0,0,1)); 

  // we no longer expect normalize to throw an exception for release code.
  // we may enable throwing an exception during development periods, but no
  // longer in a release.
  //try
  //{
  //  Vector3Df n(0,0,0);
  //  // should fail
  //  n.Normalize();
  //  // should not get here
  //  bOk = false;
  //}
  //catch (...)
  //{
  //  // should be caught here
  //  bOk &= true;
  //}

  return bOk;
} // ValidateNormalize()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateScaleDivideAnIso(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,1,1);   a.ScaleAnIso (Triplef(2,3,4));  bOk &= (a == Vector3Df(2,3,4));
  Vector3Df b(2,3,4);   b.DivideAnIso(Triplef(2,3,4));  bOk &= (b == Vector3Df(1,1,1));

  return(bOk);
} // ValidateScaleDivideAnIso()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const operators

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateNonConstOperators(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,1,1);   a *= 2;                 bOk &= (a == Vector3Df(2,2,2));
  Vector3Df b(1,1,1);   b /= 0.5F;              bOk &= (b == Vector3Df(2,2,2));

  Vector3Df c(1,1,1);   c += Vector3Df(1,2,3);  bOk &= (c == Vector3Df(2,3,4));
  Vector3Df d(2,3,4);   d -= Vector3Df(1,2,3);  bOk &= (d == Vector3Df(1,1,1));

  return(bOk);
} // ValidateNonConstOperators()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const operators

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestVector3D::ValidateStreamIO(std::ostream & stream)
{
  bool bOk(true);

  Vector3Df a(1,2,3);
  Vector3Df b(0,0,0);
  std::strstream ioStream; 
  ioStream << a;         ioStream >> b;       bOk &= (b == Vector3Df(1,2,3));
  ioStream << " 2 3 4 "; ioStream >> b;       bOk &= (b == Vector3Df(2,3,4));
  ioStream << " " << a;  ioStream >> b;       bOk &= (b == Vector3Df(1,2,3));
  ioStream <<  "2 3 4";  ioStream >> b;       bOk &= (b == Vector3Df(2,3,4));

  return(bOk);
} // ValidateStreamIO()


////////////////////////////////////////////////////////////////////////
/// Performance test


vector<Vector3Df> & GetRandomArrayFloat4()
{
  static vector<Vector3Df> array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Vector3Df(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat4()


vector<Vector3D<float8> > & GetRandomArrayFloat8()
{
  static vector<Vector3D<float8> > array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Vector3D<float8>(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat8()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return
 */
bool vxTestVector3D::TestSpeedGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Vector3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Vector3Df iterations; sum="+ToAscii(sum), "vxTestVector3D", "TestSpeedGetMaxAbsComponent");
  } // Vector3Df
  {
    vector<Vector3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Vector3D<float8> iterations; sum="+ToAscii(sum), "vxTestVector3D", "TestSpeedGetMaxAbsComponent");
  } // Vector3D<float8>
  return(bOk);
} // TestSpeedGetMaxAbsComponent()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return
 */
bool vxTestVector3D::TestSpeedGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Vector3Df> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Vector3Df iterations; sum="+ToAscii(sum), "vxTestVector3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Vector3Df
  {
    vector<Vector3D<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Vector3D<float8> iterations; sum="+ToAscii(sum), "vxTestVector3D", "TestSpeedGetMaxAbsComponentCoord");
  } // Vector3D<float8>

  return(bOk);

} // TestSpeedGetMaxAbsComponentCoord()


#undef FILE_REVISION


// $Log: vxTestVector3D.C,v $
// Revision 1.3  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.2  2005/09/26 12:43:48  vxconfig
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
// $Id: vxTestVector3D.C,v 1.3 2006/03/30 15:10:35 geconomos Exp $


