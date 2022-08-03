#ifndef MXVEC4_INCLUDED // -*- C++ -*-
#define MXVEC4_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  4D Vector class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxVec4.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec3.h"

#ifdef MIX_USE_TVEC
#include "MxVector.h"

template<class T>
class MxTVec4 : public MxVBlock<T, 4>
{
public:
    MxTVec4() { }
    MxTVec4(T x, T y, T z, T w)
	{ (*this)[0]=x; (*this)[1]=y; (*this)[2]=z; (*this)[3]=w; }
    MxTVec4(const MxVBlock<T, 3>& v, T w)
	{(*this)[0]=v[0];(*this)[1]=v[1];(*this)[2]=v[2];(*this)[3]=w;}
    MxTVec4(const MxVBlock<T, 4>& v) : MxVBlock<T, 4>(v) { }
    MxTVec4(const float4 *v)
	{(*this)[0]=v[0];(*this)[1]=v[1];(*this)[2]=v[2];(*this)[3]=v[3];}
    MxTVec4(const float8 *v)
	{(*this)[0]=v[0];(*this)[1]=v[1];(*this)[2]=v[2];(*this)[3]=v[3];}
};

typedef MxTVec4<float8> Vec4;

#else

class Vec4  {
private:
    float8 elt[4];

protected:
    inline void copy(const Vec4& v);

public:
    //
    // Standard constructors
    //
    Vec4(float8 x=0, float8 y=0, float8 z=0, float8 w=0) {
	elt[0]=x; elt[1]=y; elt[2]=z; elt[3]=w;
    }
    Vec4(const Vec3& v,float8 w) {elt[0]=v[0];elt[1]=v[1];elt[2]=v[2];elt[3]=w;}
    Vec4(const Vec4& v) { copy(v); }
    Vec4(const float4 *v) {elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; elt[3]=v[3]; }
    Vec4(const float8 *v) {elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; elt[3]=v[3];}

    //
    // Access methods
    //
    float8& operator()(uint4 i)       { AssertBound(i<4); return elt[i]; }
    float8  operator()(uint4 i) const { AssertBound(i<4); return elt[i]; }
#ifdef __GNUC__
    float8& operator[](uint4 i)       { return elt[i]; }
    float8  operator[](uint4 i) const { return elt[i]; }
#endif

    operator float8*() { return elt; }
    operator const float8*() const { return elt; }
    // operator Vec3&() { return *((Vec3 *)this); }

    //
    // Comparison methods
    //
    inline bool operator==(const Vec4&) const;
    inline bool operator!=(const Vec4&) const;

    //
    // Assignment and in-place arithmetic methods
    //
    inline void set(float8 x, float8 y, float8 z, float8 w){
	elt[0]=x; elt[1]=y; elt[2]=z; elt[3]=w;
    }
    inline Vec4& operator=(const Vec4& v);
    inline Vec4& operator+=(const Vec4& v);
    inline Vec4& operator-=(const Vec4& v);
    inline Vec4& operator*=(float8 s);
    inline Vec4& operator/=(float8 s);

    //
    // Binary arithmetic methods
    //
    inline Vec4 operator+(const Vec4& v) const;
    inline Vec4 operator-(const Vec4& v) const;
    inline Vec4 operator-() const;

    inline Vec4 operator*(float8 s) const;
    inline Vec4 operator/(float8 s) const;
    inline float8 operator*(const Vec4& v) const;
};



////////////////////////////////////////////////////////////////////////
//
// Method definitions
//

inline void Vec4::copy(const Vec4& v)
{
    elt[0]=v.elt[0]; elt[1]=v.elt[1]; elt[2]=v.elt[2]; elt[3]=v.elt[3];
}

inline bool Vec4::operator==(const Vec4& v) const
{
    float8 dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z],  dw=elt[W]-v[W];
    return (dx*dx + dy*dy + dz*dz + dw*dw) < FEQ_EPS2;
}

inline bool Vec4::operator!=(const Vec4& v) const
{
    float8 dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z],  dw=elt[W]-v[W];
    return (dx*dx + dy*dy + dz*dz + dw*dw) > FEQ_EPS2;
}

inline Vec4& Vec4::operator=(const Vec4& v)
{
    copy(v);
    return *this;
}

inline Vec4& Vec4::operator+=(const Vec4& v)
{
    elt[0] += v[0];   elt[1] += v[1];   elt[2] += v[2];   elt[3] += v[3];
    return *this;
}

inline Vec4& Vec4::operator-=(const Vec4& v)
{
    elt[0] -= v[0];   elt[1] -= v[1];   elt[2] -= v[2];   elt[3] -= v[3];
    return *this;
}

inline Vec4& Vec4::operator*=(float8 s)
{
    elt[0] *= s;   elt[1] *= s;   elt[2] *= s;  elt[3] *= s;
    return *this;
}

inline Vec4& Vec4::operator/=(float8 s)
{
    elt[0] /= s;   elt[1] /= s;   elt[2] /= s;   elt[3] /= s;
    return *this;
}

inline Vec4 Vec4::operator+(const Vec4& v) const
{
    return Vec4(elt[0]+v[0], elt[1]+v[1], elt[2]+v[2], elt[3]+v[3]);
}

inline Vec4 Vec4::operator-(const Vec4& v) const
{
    return Vec4(elt[0]-v[0], elt[1]-v[1], elt[2]-v[2], elt[3]-v[3]);
}

inline Vec4 Vec4::operator-() const
{
    return Vec4(-elt[0], -elt[1], -elt[2], -elt[3]);
}

inline Vec4 Vec4::operator*(float8 s) const
{
    return Vec4(elt[0]*s, elt[1]*s, elt[2]*s, elt[3]*s);
}

inline Vec4 Vec4::operator/(float8 s) const
{
    return Vec4(elt[0]/s, elt[1]/s, elt[2]/s, elt[3]/s);
}

inline float8 Vec4::operator*(const Vec4& v) const
{
    return elt[0]*v[0] + elt[1]*v[1] + elt[2]*v[2] + elt[3]*v[3];
}

#endif

// Make scalar multiplication commutative
inline Vec4 operator*(float8 s, const Vec4& v) { return v*s; }



////////////////////////////////////////////////////////////////////////
//
// Primitive function definitions
//

//
// Code adapted from VecLib4d.c in Graphics Gems V
inline Vec4 cross(const Vec4& a, const Vec4& b, const Vec4& c)
{
    Vec4 result;

    float8 d1 = (b[Z] * c[W]) - (b[W] * c[Z]);
    float8 d2 = (b[Y] * c[W]) - (b[W] * c[Y]);
    float8 d3 = (b[Y] * c[Z]) - (b[Z] * c[Y]);
    float8 d4 = (b[X] * c[W]) - (b[W] * c[X]);
    float8 d5 = (b[X] * c[Z]) - (b[Z] * c[X]);
    float8 d6 = (b[X] * c[Y]) - (b[Y] * c[X]);

    result[X] = - a[Y] * d1 + a[Z] * d2 - a[W] * d3;
    result[Y] =   a[X] * d1 - a[Z] * d4 + a[W] * d5;
    result[Z] = - a[X] * d2 + a[Y] * d4 - a[W] * d6;
    result[W] =   a[X] * d3 - a[Y] * d5 + a[Z] * d6;

    return result;
}

inline float8 norm(const Vec4& v)
{
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

inline float8 norm2(const Vec4& v)
{
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
}

inline float8 unitize(Vec4& v)
{
    float8 l=norm2(v);
    if( l!=1.0 && l!=0.0 )
    {
	l = sqrt(l);
	v /= l;
    }
    return l;
}

inline Vec3 proj(const Vec4& v)
{
    Vec3 u(v[0], v[1], v[2]);
    if( v[3]!=1.0 && v[3]!=0.0 )
	u /= v[3];
    return u;
}



////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline ostream& operator<<(ostream& out, const Vec4& v)
{
    return
	out << v[0] << " " << v[1] << " " << v[2] << " " << v[3];
}

inline istream& operator>>(istream& in, Vec4& v)
{
    return in >> v[0] >> v[1] >> v[2] >> v[3];
}

#ifdef MXGL_INCLUDED
// inline void glV(const Vec4& v) { glVertex4d(v[X], v[Y], v[Z], v[W]); }
// inline void glC(const Vec4& v) { glColor4d(v[X], v[Y], v[Z], v[W]); }
#endif

// MXVEC4_INCLUDED
#endif
