#ifndef MXQMETRIC2_INCLUDED // -*- C++ -*-
#define MXQMETRIC2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2D Quadric Error Metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

  $Id: MxQMetric2.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxMat2.h"
#include "MxMat3.h"

class MxQuadric2 
{
private:
    float8 a2, ab, ac;
    float8     b2, bc;
    float8         c2;

    float8 r;

    void init(float8 a, float8 b, float8 c, float8 len);

public:
    MxQuadric2() { clear(); }
    MxQuadric2(float8 a, float8 b, float8 c, float8 len=1.0)
	{ init(a, b, c, len); }
    MxQuadric2(const float4 *n, float8 c, float8 len=1.0)
	{ init(n[X], n[Y], c, len); }
    MxQuadric2(const float8 *n, float8 c, float8 len=1.0)
	{ init(n[X], n[Y], c, len); }
    MxQuadric2(const MxQuadric2& Q) { *this = Q; }

    Mat2 tensor() const;
    Vec2 vector() const { return Vec2(ac, bc); }
    float8 offset() const { return c2; }
    float8 length() const { return r; }
    Mat3 homogeneous() const;

    void clear(float8 val=0.0) { a2=ab=ac=b2=bc=c2=r=val; }
    MxQuadric2& operator=(const MxQuadric2& Q);
    MxQuadric2& operator+=(const MxQuadric2& Q);
    MxQuadric2& operator-=(const MxQuadric2& Q);
    MxQuadric2& operator*=(float8 s);

    float8 evaluate(float8 x, float8 y) const;
    float8 evaluate(const float8 *v) const { return evaluate(v[X], v[Y]); }
    float8 evaluate(const float4 *v) const { return evaluate(v[X], v[Y]); }

    float8 operator()(float8 x, float8 y) const { return evaluate(x,y); }
    float8 operator()(const float8 *v) const  { return evaluate(v[X], v[Y]); }
    float8 operator()(const float4 *v) const  { return evaluate(v[X], v[Y]); }

    bool optimize(Vec2& v) const;
    bool optimize(float4 *x, float4 *y) const;

    //    bool optimize(Vec2& v, const Vec2& v1, const Vec2& v2) const;
};

// MXQMETRIC2_INCLUDED
#endif
