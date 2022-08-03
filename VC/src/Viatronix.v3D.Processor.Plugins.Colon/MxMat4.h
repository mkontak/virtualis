#ifndef MXMAT4_INCLUDED // -*- C++ -*-
#define MXMAT4_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  4x4 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat4.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec4.h"

class Mat4 
{
private:
    Vec4 row[4];

protected:

    inline void copy(const Mat4& m);
    inline Vec4 col(int4 i) const
        { return Vec4(row[0][i],row[1][i],row[2][i],row[3][i]); }

public:
    // Standard matrices
    static Mat4 I;
    static Mat4 zero;
    static Mat4 unit;

    static Mat4 trans(float8,float8,float8);
    static Mat4 scale(float8,float8,float8);
    static Mat4 xrot(float8); //
    static Mat4 yrot(float8); // Arguments are in radians
    static Mat4 zrot(float8); //


    // Standard constructors
    Mat4() { copy(zero); }
    Mat4(const Vec4& r0,const Vec4& r1,const Vec4& r2,const Vec4& r3)
    { row[0]=r0; row[1]=r1; row[2]=r2; row[3]=r3; }
    Mat4(const Mat4& m) { copy(m); }

    // Access methods
    // M(i, j) == row i;col j
    float8& operator()(int4 i, int4 j)       { return row[i][j]; }
    float8  operator()(int4 i, int4 j) const { return row[i][j]; }
    Vec4&       operator[](int4 i)       { return row[i]; }
    const Vec4& operator[](int4 i) const { return row[i]; }

    operator       float8*()       { return row[0]; }
    operator const float8*() const { return row[0]; }

    // Comparison methods
    inline int4 operator==(const Mat4&);

    // Assignment methods
    inline Mat4& operator=(const Mat4& m) { copy(m); return *this; }
    inline Mat4& operator+=(const Mat4& m);
    inline Mat4& operator-=(const Mat4& m);

    inline Mat4& operator*=(float8 s);
    inline Mat4& operator/=(float8 s);


    // Arithmetic methods
    inline Mat4 operator+(const Mat4& m) const;
    inline Mat4 operator-(const Mat4& m) const;
    inline Mat4 operator-() const;

    inline Mat4 operator*(float8 s) const;
    inline Mat4 operator/(float8 s) const;
    Mat4 operator*(const Mat4& m) const;

    inline Vec4 operator*(const Vec4& v) const; // [x y z w]
    inline Vec3 operator*(const Vec3& v) const; // [x y z w]

    // Matrix operations
    float8 det() const;
    Mat4 transpose() const;
    Mat4 adjoint() const;
    float8 invert(Mat4&) const;
    float8 cramerInvert(Mat4&) const;
};



inline void Mat4::copy(const Mat4& m)
{
    row[0] = m.row[0]; row[1] = m.row[1];
    row[2] = m.row[2]; row[3] = m.row[3];
}

inline int4 Mat4::operator==(const Mat4& m)
{
    return row[0]==m.row[0] &&
	   row[1]==m.row[1] &&
	   row[2]==m.row[2] &&
	   row[3]==m.row[3] ;
}

inline Mat4& Mat4::operator+=(const Mat4& m)
{
    row[0] += m.row[0]; row[1] += m.row[1];
    row[2] += m.row[2]; row[3] += m.row[3];
    return *this;
}

inline Mat4& Mat4::operator-=(const Mat4& m)
{
    row[0] -= m.row[0]; row[1] -= m.row[1];
    row[2] -= m.row[2]; row[3] -= m.row[3];
    return *this;
}

inline Mat4& Mat4::operator*=(float8 s)
{
    row[0] *= s; row[1] *= s; row[2] *= s; row[3] *= s;
    return *this;
}

inline Mat4& Mat4::operator/=(float8 s)
{
    row[0] /= s; row[1] /= s; row[2] /= s; row[3] /= s;
    return *this;
}

inline Mat4 Mat4::operator+(const Mat4& m) const
{
    return Mat4(row[0]+m.row[0],
		row[1]+m.row[1],
		row[2]+m.row[2],
		row[3]+m.row[3]);
}

inline Mat4 Mat4::operator-(const Mat4& m) const
{
    return Mat4(row[0]-m.row[0],
		row[1]-m.row[1],
		row[2]-m.row[2],
		row[3]-m.row[3]);
}

inline Mat4 Mat4::operator-() const
{
    return Mat4(-row[0], -row[1], -row[2], -row[3]);
}

inline Mat4 Mat4::operator*(float8 s) const
{
    return Mat4(row[0]*s, row[1]*s, row[2]*s, row[3]*s);
}

inline Mat4 Mat4::operator/(float8 s) const
{
    return Mat4(row[0]/s, row[1]/s, row[2]/s, row[3]/s);
}

inline Vec4 Mat4::operator*(const Vec4& v) const
{
    return Vec4(row[0]*v, row[1]*v, row[2]*v, row[3]*v);
}

//
// Transform a homogeneous 3-vector and reproject into normal 3-space
//
inline Vec3 Mat4::operator*(const Vec3& v) const
{
    Vec4 u=Vec4(v,1);
    float8 w=row[3]*u;

    if(w==0.0)
	return Vec3(row[0]*u, row[1]*u, row[2]*u);
    else
	return Vec3(row[0]*u/w, row[1]*u/w, row[2]*u/w);
}

inline ostream& operator<<(ostream& out, const Mat4& M)
{
    return out<<M[0]<<endl<<M[1]<<endl<<M[2]<<endl<<M[3];
}

inline istream& operator>>(istream& in, Mat4& M)
{
    return in >> M[0] >> M[1] >> M[2] >> M[3];
}

// extern bool jacobi(const Mat4& m, Vec4& vals, Vec4 vecs[4]);

#ifdef MXGL_INCLUDED
//inline void glGetMatrix(Mat4& m, GLenum which=GL_MODELVIEW_MATRIX)
//{ Mat4 tmp;  glGetDoublev(which, &tmp(0,0));  m=tmp.transpose(); }

//inline void glLoadMatrix(const Mat4& m)
//{ Mat4 tmp = m.transpose();  glLoadMatrixd(&tmp(0,0)); }

//inline void glMultMatrix(const Mat4& m)
//{ Mat4 tmp = m.transpose();  glMultMatrixd(&tmp(0,0)); }
#endif

// MXMAT4_INCLUDED
#endif
