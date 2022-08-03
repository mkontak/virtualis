#ifndef MXQMETRIC3_INCLUDED // -*- C++ -*-
#define MXQMETRIC3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  3D Quadric Error Metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxQMetric3.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxMat3.h"
#include "MxMat4.h"

class MxQuadric3 
{
private:
    float8 a2, ab, ac, ad;
    float8     b2, bc, bd;
    float8         c2, cd;
    float8             d2;

    float8 r;

    void init(float8 a, float8 b, float8 c, float8 d, float8 area);
    void init(const Mat4& Q, float8 area);

public:
    MxQuadric3() { clear(); }
    MxQuadric3(float8 a, float8 b, float8 c, float8 d, float8 area=1.0)
	{ init(a, b, c, d, area); }
    MxQuadric3(const float4 *n, float8 d, float8 area=1.0)
	{ init(n[X], n[Y], n[Z], d, area); }
    MxQuadric3(const float8 *n, float8 d, float8 area=1.0)
	{ init(n[X], n[Y], n[Z], d, area); }
    MxQuadric3(const MxQuadric3& Q) { *this = Q; }

    Mat3 tensor() const;
    Vec3 vector() const { return Vec3(ad, bd, cd); }
    float8 offset() const { return d2; }
    float8 area() const { return r; }
    Mat4 homogeneous() const;

    void set_coefficients(const float8 *);
    void set_area(float8 a) { r=a; }

    void clear(float8 val=0.0) { a2=ab=ac=ad=b2=bc=bd=c2=cd=d2=r=val; }
    MxQuadric3& operator=(const MxQuadric3& Q);
    MxQuadric3& operator+=(const MxQuadric3& Q);
    MxQuadric3& operator-=(const MxQuadric3& Q);
    MxQuadric3& operator*=(float8 s);
    MxQuadric3& transform(const Mat4& P);

    float8 evaluate(float8 x, float8 y, float8 z) const;
    float8 evaluate(const float8 *v) const {return evaluate(v[X], v[Y], v[Z]);}
    float8 evaluate(const float4 *v) const  {return evaluate(v[X], v[Y], v[Z]);}

    float8 operator()(float8 x, float8 y, float8 z) const
	{ return evaluate(x, y, z); }
    float8 operator()(const float8 *v) const {return evaluate(v[X],v[Y],v[Z]);}
    float8 operator()(const float4 *v) const  {return evaluate(v[X],v[Y],v[Z]);}

    bool optimize(Vec3& v) const;
    bool optimize(float4 *x, float4 *y, float4 *z) const;

    bool optimize(Vec3& v, const Vec3& v1, const Vec3& v2) const;
    bool optimize(Vec3& v,const Vec3& v1,const Vec3& v2,const Vec3& v3) const;


    ostream& write(ostream& out)
	{
	    return out << a2 << " " << ab << " " << ac << " " << ad << " "
		       << b2 << " " << bc << " " << bd << " " << c2 << " "
		       << cd << " " << d2 << " " << r;
	}

    ostream& write_full(ostream& out)
	{
	    return out << a2 << " " << ab << " " << ac << " " << ad << " "
		       << ab << " " << b2 << " " << bc << " " << bd << " "
		       << ac << " " << bc << " " << c2 << " " << cd << " "
		       << ad << " " << bd << " " << cd << " " << d2;
	}


    istream& read(istream& in)
	{
	    return in >> a2 >> ab >> ac >> ad >> b2
		      >> bc >> bd >> c2 >> cd >> d2 >> r;
	}
    

    istream& read_full(istream& in)
	{
	    return in >> a2 >> ab >> ac >> ad
		      >> ab >> b2 >> bc >> bd
		      >> ac >> bc >> c2 >> cd
		      >> ad >> bd >> cd >> d2;
	}

};


inline ostream& operator<<(ostream& out, MxQuadric3& Q) {return Q.write(out);}
inline istream& operator>>(istream& in, MxQuadric3& Q) { return Q.read(in); }


////////////////////////////////////////////////////////////////////////
//
// Quadric visualization routines
//

#define MX_RED_ELLIPSOIDS 0x1
#define MX_GREEN_ELLIPSOIDS 0x2
#define MX_CHARCOAL_ELLIPSOIDS 0x3

extern void mx_quadric_shading(int4 c=MX_GREEN_ELLIPSOIDS, bool twosided=true);
extern void mx_draw_quadric(const MxQuadric3& Q, float8 r, const float4*v=NULL);
extern void mx_draw_osculant(float4 k1, float4 k2, float4 extent=1.0);

// MXQMETRIC3_INCLUDED
#endif
