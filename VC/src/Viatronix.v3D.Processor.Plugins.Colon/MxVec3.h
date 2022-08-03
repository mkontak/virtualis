#ifndef MXVEC3_INCLUDED // -*- C++ -*-
#define MXVEC3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  3D Vector class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxVec3.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec2.h"

#ifdef MIX_USE_TVEC
#include "MxVector.h"

template<class T>
class MxTVec3 : public MxVBlock<T, 3>
{
public:
    MxTVec3() { }
    MxTVec3(T x, T y, T z) { (*this)[0]=x; (*this)[1]=y; (*this)[2]=z; }
    MxTVec3(const MxVBlock<T, 3>& v) : MxVBlock<T, 3>(v) { }
    MxTVec3(const float4 *v) {(*this)[0]=v[0];(*this)[1]=v[1];(*this)[2]=v[2];}
    MxTVec3(const float8 *v) {(*this)[0]=v[0];(*this)[1]=v[1];(*this)[2]=v[2];}

    inline MxTVec3<T> operator^(const MxTVec3<T>& v) const
	{ MxTVec3<T> r; mxv_cross3(r, *this, v); return r; }
};

typedef MxTVec3<float8> Vec3;

#else

class Vec3  {
private:
    float8 elt[3];

protected:
    inline void copy(const Vec3& v);

public:
    //
    // Standard constructors
    //
    Vec3(float8 x=0, float8 y=0, float8 z=0) { elt[0]=x; elt[1]=y; elt[2]=z; }
    Vec3(const Vec2& v, float8 z) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=z; }
    Vec3(const Vec3& v) { copy(v); }
    Vec3(const float4 *v) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; }
    Vec3(const float8 *v) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; }

    //
    // Access methods
    //
    float8& operator()(uint4 i)       { AssertBound(i<3); return elt[i]; }
    float8  operator()(uint4 i) const { AssertBound(i<3); return elt[i]; }
#ifdef __GNUC__
    float8& operator[](uint4 i)       { return elt[i]; }
    float8  operator[](uint4 i) const { return elt[i]; }
#endif

    operator float8*() { return elt; }
    operator const float8*() const { return elt; }
    // operator Vec2&() { return *((Vec2 *)this); }

    //
    // Comparison operators
    //
    inline bool operator==(const Vec3& v) const;
    inline bool operator!=(const Vec3& v) const;

    //
    // Assignment and in-place arithmetic methods
    //
    inline void set(float8 x,float8 y,float8 z) {elt[0]=x; elt[1]=y; elt[2]=z;}
    inline Vec3& operator=(const Vec3& v);
    inline Vec3& operator+=(const Vec3& v);
    inline Vec3& operator-=(const Vec3& v);
    inline Vec3& operator*=(float8 s);
    inline Vec3& operator/=(float8 s);

    //
    // Binary arithmetic methods
    //
    inline Vec3 operator+(const Vec3& v) const;
    inline Vec3 operator-(const Vec3& v) const;
    inline Vec3 operator-() const;

    inline Vec3 operator*(float8 s) const;
    inline Vec3 operator/(float8 s) const;
    inline float8 operator*(const Vec3& v) const;
    inline Vec3 operator^(const Vec3& v) const;
};



////////////////////////////////////////////////////////////////////////
//
// Method definitions
//

inline void Vec3::copy(const Vec3& v)
{
    elt[0]=v.elt[0]; elt[1]=v.elt[1]; elt[2]=v.elt[2];
}

inline bool Vec3::operator==(const Vec3& v) const
{
    float8 dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z];
    return (dx*dx + dy*dy + dz*dz) < FEQ_EPS2;
}

inline bool Vec3::operator!=(const Vec3& v) const
{
    float8 dx=elt[X]-v[X],  dy=elt[Y]-v[Y],  dz=elt[Z]-v[Z];
    return (dx*dx + dy*dy + dz*dz) > FEQ_EPS2;
}

inline Vec3& Vec3::operator=(const Vec3& v)
{
    copy(v);
    return *this;
}

inline Vec3& Vec3::operator+=(const Vec3& v)
{
    elt[0] += v[0];   elt[1] += v[1];   elt[2] += v[2];
    return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v)
{
    elt[0] -= v[0];   elt[1] -= v[1];   elt[2] -= v[2];
    return *this;
}

inline Vec3& Vec3::operator*=(float8 s)
{
    elt[0] *= s;   elt[1] *= s;   elt[2] *= s;
    return *this;
}

inline Vec3& Vec3::operator/=(float8 s)
{
    elt[0] /= s;   elt[1] /= s;   elt[2] /= s;
    return *this;
}


inline Vec3 Vec3::operator+(const Vec3& v) const
{
    return Vec3(elt[0]+v[0], elt[1]+v[1], elt[2]+v[2]);
}

inline Vec3 Vec3::operator-(const Vec3& v) const
{
    return Vec3(elt[0]-v[0], elt[1]-v[1], elt[2]-v[2]);
}

inline Vec3 Vec3::operator-() const
{
    return Vec3(-elt[0], -elt[1], -elt[2]);
}

inline Vec3 Vec3::operator*(float8 s) const
{
    return Vec3(elt[0]*s, elt[1]*s, elt[2]*s);
}

inline Vec3 Vec3::operator/(float8 s) const
{
    return Vec3(elt[0]/s, elt[1]/s, elt[2]/s);
}

inline float8 Vec3::operator*(const Vec3& v) const
{
    return elt[0]*v[0] + elt[1]*v[1] + elt[2]*v[2];
}

inline Vec3 Vec3::operator^(const Vec3& v) const
{
    Vec3 w( elt[1]*v[2] - v[1]*elt[2],
	   -elt[0]*v[2] + v[0]*elt[2],
	    elt[0]*v[1] - v[0]*elt[1] );
    return w;
}
#endif

// Make scalar multiplication commutative
inline Vec3 operator*(float8 s, const Vec3& v) { return v*s; }



////////////////////////////////////////////////////////////////////////
//
// Primitive function definitions
//

inline float8 norm(const Vec3& v)
{
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

inline float8 norm2(const Vec3& v)
{
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

inline float8 unitize(Vec3& v)
{
    float8 l=norm2(v);
    if( l!=1.0 && l!=0.0 )
    {
	l = sqrt(l);
	v /= l;
    }
    return l;
}

inline Vec2 proj(const Vec3& v)
{
    Vec2 u(v[0], v[1]);
    if( v[2]!=1.0 && v[2]!=0.0 )
	u /= v[2];
    return u;
}



////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline ostream& operator<<(ostream& out, const Vec3& v)
{
    return out << v[0] << " " << v[1] << " " << v[2];
}

inline istream& operator>>(istream& in, Vec3& v)
{
    return in >> v[0] >> v[1] >> v[2];
}

#ifdef MXGL_INCLUDED
// inline void glV(const Vec3& v) { glVertex3d(v[X], v[Y], v[Z]); }
// inline void glN(const Vec3& v) { glNormal3d(v[X], v[Y], v[Z]); }
// inline void glC(const Vec3& v) { glColor3d(v[X], v[Y], v[Z]); }
#endif


// MXVEC3_INCLUDED
#endif
