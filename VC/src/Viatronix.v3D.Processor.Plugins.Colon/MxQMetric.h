#ifndef MXQMETRIC_INCLUDED // -*- C++ -*-
#define MXQMETRIC_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  n-D Quadric Error Metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxQMetric.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxQMetric3.h"
#include "MxVector.h"
#include "MxMatrix.h"

class MxQuadric 
{
private:
    MxMatrix A;
    MxVector b;
    float8 c;

    float8 r;

public:
    MxQuadric(uint4 N) : A(N), b(N) { clear(); }
    MxQuadric(const MxVector& p1, const MxVector& p2, const MxVector& p3,
	      float8 area=1.0);
    MxQuadric(const MxQuadric3&, unsigned int N);
    MxQuadric(const MxQuadric& Q)
	: A(Q.A.dim()), b(Q.b.dim()) { *this = Q; }

    const MxMatrix& tensor() const { return A; }
    const MxVector& vector() const { return b; }
    float8 offset() const { return c; }
    float8 area() const { return r; }
    MxMatrix& homogeneous(MxMatrix& H) const;

    void clear(float8 val=0.0) { A=val; b=val; c=val; r=val; }
    MxQuadric& operator=(const MxQuadric& Q)
	{ A=Q.A; b=Q.b; c=Q.c; r=Q.r; return *this; }
    MxQuadric& operator+=(const MxQuadric& Q)
	{ A+=Q.A; b+=Q.b; c+=Q.c; r+=Q.r; return *this; }
    MxQuadric& operator-=(const MxQuadric& Q)
	{ A-=Q.A; b-=Q.b; c-=Q.c; r-=Q.r; return *this; }
    MxQuadric& operator*=(float8 s)
	{ A*=s; b*=s; c*=s; return *this; }

    float8 evaluate(const MxVector& v) const;
    float8 operator()(const MxVector& v) const { return evaluate(v); }

    bool optimize(MxVector& v) const;
};

// MXQMETRIC_INCLUDED
#endif
