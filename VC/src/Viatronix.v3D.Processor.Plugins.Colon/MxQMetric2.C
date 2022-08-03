/************************************************************************

  2D Quadric Error Metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

  $Id: MxQMetric2.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxQMetric2.h"


void MxQuadric2::init(float8 a, float8 b, float8 c, float8 len)
{
    a2 = a*a;  ab = a*b;  ac = a*c;
               b2 = b*b;  bc = b*c;
	                  c2 = c*c;

    r = len;
}

Mat2 MxQuadric2::tensor() const
{
    return Mat2(a2, ab, ab, b2);
}

Mat3 MxQuadric2::homogeneous() const
{
    return Mat3(Vec3(a2, ab, ac),
		Vec3(ab, b2, bc),
		Vec3(ac, bc, c2));
}

MxQuadric2& MxQuadric2::operator=(const MxQuadric2& Q)
{
    r = Q.r;

    a2 = Q.a2;  ab = Q.ab;  ac = Q.ac;
                b2 = Q.b2;  bc = Q.bc;
		            c2 = Q.c2;

    return *this;
}

MxQuadric2& MxQuadric2::operator+=(const MxQuadric2& Q)
{
    r += Q.r;

    a2 += Q.a2;  ab += Q.ab;  ac += Q.ac;
                 b2 += Q.b2;  bc += Q.bc;
		              c2 += Q.c2;

    return *this;
}

MxQuadric2& MxQuadric2::operator-=(const MxQuadric2& Q)
{
    r -= Q.r;

    a2 -= Q.a2;  ab -= Q.ab;  ac -= Q.ac;
                 b2 -= Q.b2;  bc -= Q.bc;
		              c2 -= Q.c2;

    return *this;
}

MxQuadric2& MxQuadric2::operator*=(float8 s)
{
    // Don't scale area

    a2 *= s;  ab *= s;  ac *= s;
              b2 *= s;  bc *= s;
	                c2 *= s;

    return *this;
}

float8 MxQuadric2::evaluate(float8 x, float8 y) const
{
    // Evaluate vAv + 2bv + c

    return x*x*a2 + 2*x*y*ab + 2*x*ac
	          + y*y*b2   + 2*y*bc
	                     + c2;
}

bool MxQuadric2::optimize(Vec2& v) const
{
    Mat2 Ainv;
    float8 det = tensor().invert(Ainv);
    if( FEQ(det, 0.0, 1e-12) )
	return false;

    v = -(Ainv*vector());

    return true;
}

bool MxQuadric2::optimize(float4 *x, float4 *y) const
{
    Vec2 v;

    bool success = optimize(v);
    if( success )
    {
	*x = (float4)v[X];
	*y = (float4)v[Y];
    }
    return success;
}
