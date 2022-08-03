// $Id: vxTestTriple.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc, All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: dongqing@viatronix.com

#include "stdafx.h"
#include "vxTestTriple.h"
#include "Triple.h"
#include "Point3D.h"
#include "Vector3D.h"
#include "Normal3D.h"
#include "utilities.h"
#include "Timer.h"


#define FILE_REVISION "$Revision: 1.2 $"

using namespace std;

/**
 * Unit test of vxVolumeIterators stuff
 * @param out stream
 * @return success
 */
bool vxTestTriple::UnitTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateConstructors",  ValidateConstructors, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateAssignment",  ValidateAssignment, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateIndexAccess",  ValidateIndexAccess, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateComparators",  ValidateComparators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateGetMaxComponent",  ValidateGetMaxComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateGetMaxAbsComponent",  ValidateGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateGetMaxComponentCoord",  ValidateGetMaxComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateGetMaxAbsComponentCoord",  ValidateGetMaxAbsComponentCoord, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateGetScaledDividedUnIso",  ValidateGetScaledDividedUnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateUnaryPlusMinus",  ValidateUnaryPlusMinus, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateScalarMultDiv",  ValidateScalarMultDiv, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateClamp",  ValidateClamp, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidatePermute",  ValidatePermute, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateScaleDivideAnIso",  ValidateScaleDivideAnIso, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateNonConstOperators",  ValidateNonConstOperators, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "ValidateStreamIO",  ValidateStreamIO, bOk);

  stream << endl;
  stream << "Overall test took: " << timer.ElapsedTime() << endl;

  return bOk;
} // UnitTest()


/**
 * Performance test of vxVolume stuff
 * @param out stream
 * @return success
 */
bool vxTestTriple::PerformanceTest(std::ostream & stream)
//*******************************************************************
{
  bool bOk(true);
  Timer timer;

  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "TestSpeedGetMaxAbsComponent",  TestSpeedGetMaxAbsComponent, bOk);
  VXTEST_VALIDATE_FUNCTION(stream, "vxTestTriple", "TestSpeedGetMaxAbsComponentCoord",  TestSpeedGetMaxAbsComponentCoord, bOk);

  return bOk;
} // PerformanceTest()


////////////////////////////////////////////////////////////////////////
/// Demo

/**
 * Demonstrate basic class functionality
 * @param stream
 * @return true if success
 */
bool vxTestTriple::Demo(std::ostream & stream)
{
  Triplef triple(1.5F, 0, 0);

  LogFyi("triple=" + triple.ToString(), "vxTestTriple", "Demo");
  return(true);
} // Demo



////////////////////////////////////////////////////////////////////////
/// Validation

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateConstructors(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a;                                       bOk &= (a.X() == 0    && a.Y() == 0    && a.Z() == 0); 
  Triplef b(1,2,3);                                bOk &= (b.X() == 1    && b.Y() == 2    && b.Z() == 3);
  Triplef c(b);                                    bOk &= (c.X() == 1    && c.Y() == 2    && c.Z() == 3);

  // copy constructors
  Triplef   triple  (1,2,3);  Triplef d(triple );  bOk &= (d == Triplef(1,2,3));
  Point3Df  point3D (1,2,3);  Triplef e(point3D);  bOk &= (e == Triplef(1,2,3));
  Vector3Df vector3D(1,2,3);  Triplef f(vector3D); bOk &= (f == Triplef(1,2,3));

  return(bOk);
} // ValidateConstructors


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateAssignment(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a;   a.SetXYZ(1,2,3);             bOk &= (a == Triplef(1,2,3));
  Triplef b;   b = a;                       bOk &= (b == Triplef(1,2,3));
  Triplef c;   c.X()=1; c.Y()=2; c.Z()=3;   bOk &= (c == Triplef(1,2,3));
  Triplef d;   d[0] =1; d[1] =2; d[2] =3;   bOk &= (d == Triplef(1,2,3));

  return(bOk);
} // ValidateAssignment()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateIndexAccess(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a(1,2,3);     bOk &= (a[0] == 1    && a[1] == 2    && a[2] == 3); 

  return(bOk);
} // ValidateIndexAccess()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateComparators(std::ostream & stream)
{
  bool bOk(true);

  // default int
  Triplei a(1,2,3);     bOk &= (a == a); 
  Triplei b(1,2,3);     bOk &= (a == b); 
  Triplei c(1,0,0);     bOk &= (a != c); 

  // default float
  Triplef d(1,2,3);     bOk &= (d == d); 
  Triplef e(1,2,3);     bOk &= (d == e); 
  Triplef f(1,0,0);     bOk &= (d != f); 

  return(bOk);
} // ValidateComparators()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateGetMaxComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a( 1, 0, 0); bOk &= (a.GetMaxComponent() ==  1.0F);
  Triplef b(-1, 0, 0); bOk &= (b.GetMaxComponent() ==  0.0F);
  Triplef c( 0, 1, 0); bOk &= (c.GetMaxComponent() ==  1.0F);
  Triplef d( 0,-1, 0); bOk &= (d.GetMaxComponent() ==  0.0F);
  Triplef e( 0, 0, 1); bOk &= (e.GetMaxComponent() ==  1.0F);
  Triplef f( 0, 0,-1); bOk &= (f.GetMaxComponent() ==  0.0F);

  return(bOk);
} // ValidateGetMaxComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a( 1, 0, 0); bOk &= (a.GetMaxAbsComponent() ==  1.0F);
  Triplef b(-1, 0, 0); bOk &= (b.GetMaxAbsComponent() == -1.0F);
  Triplef c( 0, 1, 0); bOk &= (c.GetMaxAbsComponent() ==  1.0F);
  Triplef d( 0,-1, 0); bOk &= (d.GetMaxAbsComponent() == -1.0F);
  Triplef e( 0, 0, 1); bOk &= (e.GetMaxAbsComponent() ==  1.0F);
  Triplef f( 0, 0,-1); bOk &= (f.GetMaxAbsComponent() == -1.0F);

  return(bOk);
} // ValidateGetMaxAbsComponent()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateGetMaxComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a( 1, 0, 0); bOk &= (a.GetMaxComponentCoord() == 0);
  Triplef b(-1, 0, 0); bOk &= (b.GetMaxComponentCoord() == 2);
  Triplef c( 0, 1, 0); bOk &= (c.GetMaxComponentCoord() == 1);
  Triplef d( 0,-1, 0); bOk &= (d.GetMaxComponentCoord() == 2);
  Triplef e( 0, 0, 1); bOk &= (e.GetMaxComponentCoord() == 2);
  Triplef f( 0, 0,-1); bOk &= (f.GetMaxComponentCoord() == 1);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a( 1, 0, 0); bOk &= (a.GetMaxAbsComponentCoord() == 0);
  Triplef b(-1, 0, 0); bOk &= (b.GetMaxAbsComponentCoord() == 0);
  Triplef c( 0, 1, 0); bOk &= (c.GetMaxAbsComponentCoord() == 1);
  Triplef d( 0,-1, 0); bOk &= (d.GetMaxAbsComponentCoord() == 1);
  Triplef e( 0, 0, 1); bOk &= (e.GetMaxAbsComponentCoord() == 2);
  Triplef f( 0, 0,-1); bOk &= (f.GetMaxAbsComponentCoord() == 2);

  return(bOk);
} // ValidateGetMaxAbsComponentCoord()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateGetScaledDividedUnIso(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,1,1);  Triplef c(2,3,4);  bOk &= (a.GetScaledAnIso(c)  == Triplef(2,3,4)); 
  Triplef b(2,3,4);  Triplef d(2,3,4);  bOk &= (b.GetDividedAnIso(d) == Triplef(1,1,1)); 

  return(bOk);
} // ValidateGetScaledDividedUnIso()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math const operators  

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateUnaryPlusMinus(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,2,3);  

  a = +a;  bOk &= (a == Triplef( 1, 2, 3));  
  a = -a;  bOk &= (a == Triplef(-1,-2,-3));  

  return(bOk);
} // ValidateUnaryPlusMinus()



/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateScalarMultDiv(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef v; 
  Triplef a(1,2,3);   

  v=2.0F*a; bOk &= (v == Triplef(2,4,6));   
  v=a*2;    bOk &= (v == Triplef(2,4,6));   
  v=a/0.5F; bOk &= (v == Triplef(2,4,6));   

  return(bOk);
} // ValidateScalarMultDiv()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const

/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateClamp(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,2,3);   a.Clamp(1.5F, 2.5F);  bOk &= (a == Triplef(1.5F,2,2.5F));

  return(bOk);
} // ValidateClamp()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidatePermute(std::ostream & stream)
{
  bool bOk(true);

  // default float
  Triplef a(1,0,0);   
  Triplef b(0,1,0);   
  Triplef c(0,0,1);   
  Triplef v(a);   

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
bool vxTestTriple::ValidateScaleDivideAnIso(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,1,1);   a.ScaleAnIso (Triplef(2,3,4));  bOk &= (a == Triplef(2,3,4));
  Triplef b(2,3,4);   b.DivideAnIso(Triplef(2,3,4));  bOk &= (b == Triplef(1,1,1));

  return(bOk);
} // ValidateScaleDivideAnIso()


/**
 * @param stream
 * @return success or falilure of validation.
 */
bool vxTestTriple::ValidateNonConstOperators(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,1,1);   a *= 2;                 bOk &= (a == Triplef(2,2,2));
  Triplef b(1,1,1);   b /= 0.5F;              bOk &= (b == Triplef(2,2,2));

  return(bOk);
} // ValidateNonConstOperators()


/**
 * @return success or falilure of validation.
 * @param stream
 */
bool vxTestTriple::ValidateStreamIO(std::ostream & stream)
{
  bool bOk(true);

  Triplef a(1,2,3);
  Triplef b(0,0,0);
  std::strstream ioStream; 
  ioStream << a;         ioStream >> b;       bOk &= (b == Triplef(1,2,3));
  ioStream << " 2 3 4 "; ioStream >> b;       bOk &= (b == Triplef(2,3,4));
  ioStream << " " << a;  ioStream >> b;       bOk &= (b == Triplef(1,2,3));
  ioStream <<  "2 3 4";  ioStream >> b;       bOk &= (b == Triplef(2,3,4));

  return(bOk);
} // ValidateStreamIO()


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Math non-const operators

////////////////////////////////////////////////////////////////////////
/// Performance test


vector<Triplef> & GetRandomArrayFloat4()
{
  static vector<Triplef> array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Triplef(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat4()


vector<Triple<float8> > & GetRandomArrayFloat8()
{
  static vector<Triple<float8> > array(1000000);
  for (int4 k=0;  k<array.size();  k+=2)
  {
    array[k  ] = Triple<float8>(rand(), rand(), rand());
    array[k+1] = -array[k];
  }
  return array;
} // GetRandomArrayFloat8()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return true if success
 */
bool vxTestTriple::TestSpeedGetMaxAbsComponent(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Triplef> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Triplef iterations; sum="+ToAscii(sum), "vxTestTriple", "TestSpeedGetMaxAbsComponent");
  } // Triplef
  {
    vector<Triple<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k].GetMaxAbsComponent();

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Triple<float8> iterations; sum="+ToAscii(sum), "vxTestTriple", "TestSpeedGetMaxAbsComponent");
  } // Triple<float8>

  return(bOk);
} // TestSpeedGetMaxAbsComponent()


/**
 * Test performance of the operator ++ that does the actual region growing.
 * @param stream
 * @return true if success
 */
bool vxTestTriple::TestSpeedGetMaxAbsComponentCoord(std::ostream & stream)
{
  bool bOk(true);

  int k(0);
  {
    vector<Triplef> & nums = GetRandomArrayFloat4();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Triplef iterations; sum="+ToAscii(sum), "vxTestTriple", "TestSpeedGetMaxAbsComponentCoord");
  } // Triplef
  {
    vector<Triple<float8> > & nums = GetRandomArrayFloat8();

    Timer timer;
    float4 sum = 1;
    for (k=0;  k<nums.size();  ++k) sum += nums[k][nums[k].GetMaxAbsComponentCoord()];

    LogFyi(timer.AsString()+" for "+ToAscii(nums.size())+" Triple<float8> iterations; sum="+ToAscii(sum), "vxTestTriple", "TestSpeedGetMaxAbsComponentCoord");
  } // Triple<float8>
  return(bOk);
} // TestSpeedGetMaxAbsComponentCoord()

#undef FILE_REVISION


// $Log: vxTestTriple.C,v $
// Revision 1.2  2006/03/30 15:10:35  geconomos
// enabled precompiled headers
//
// Revision 1.1  2005/05/10 20:03:33  frank
// revived from vx
//
// Revision 1.1  2003/05/20 15:53:04  dongqing
// initialization
//
// initial version
//
// $Id: vxTestTriple.C,v 1.2 2006/03/30 15:10:35 geconomos Exp $


