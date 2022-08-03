#ifndef MXMAT3_INCLUDED // -*- C++ -*-
#define MXMAT3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  3x3 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat3.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec3.h"

class Mat3 
{
private:
    Vec3 row[3];

protected:

    inline void copy(const Mat3& m);
    inline Vec3 col(int4 i) const {return Vec3(row[0][i],row[1][i],row[2][i]);}

public:
    // Standard matrices
    static Mat3 I;
    static Mat3 zero;
    static Mat3 unit;
    static Mat3 diag(const Vec3& v);    // diagonal elements from v
    static Mat3 extend(const Vec3& v);  // each row becomes v
    static Mat3 outer_product(const Vec3& v); // outer product of v by v

    // Standard constructors
    Mat3() { copy(zero); }
    Mat3(const Vec3& r0,const Vec3& r1,const Vec3& r2)
    { row[0]=r0; row[1]=r1; row[2]=r2; }
    Mat3(const Mat3& m) { copy(m); }

    // Access methods
    // M(i, j) == row i;col j
    float8& operator()(int4 i, int4 j)       { return row[i][j]; }
    float8  operator()(int4 i, int4 j) const { return row[i][j]; }
    Vec3&       operator[](int4 i)       { return row[i]; }
    const Vec3& operator[](int4 i) const { return row[i]; }

    operator       float8*()       { return row[0]; }
    operator const float8*() const { return row[0]; }

    // Assignment methods
    inline Mat3& operator=(const Mat3& m) { copy(m); return *this; }
    inline Mat3& operator+=(const Mat3& m);
    inline Mat3& operator-=(const Mat3& m);

    inline Mat3& operator*=(float8 s);
    inline Mat3& operator/=(float8 s);


    // Arithmetic methods
    inline Mat3 operator+(const Mat3& m) const;
    inline Mat3 operator-(const Mat3& m) const;
    inline Mat3 operator-() const;

    inline Mat3 operator*(float8 s) const;
    inline Mat3 operator/(float8 s) const;
    Mat3 operator*(const Mat3& m) const;

    inline Vec3 operator*(const Vec3& v) const; // [x y z]

    // Matrix operations
    float8 det();
    float8 trace();
    Mat3 transpose();
    Mat3 adjoint();
    float8 invert(Mat3&);
};



inline void Mat3::copy(const Mat3& m)
{
    row[0] = m.row[0]; row[1] = m.row[1]; row[2] = m.row[2];
}

inline Mat3& Mat3::operator+=(const Mat3& m)
{
    row[0] += m.row[0]; row[1] += m.row[1]; row[2] += m.row[2];
    return *this;
}

inline Mat3& Mat3::operator-=(const Mat3& m)
{
    row[0] -= m.row[0]; row[1] -= m.row[1]; row[2] -= m.row[2];
    return *this;
}

inline Mat3& Mat3::operator*=(float8 s)
{
    row[0] *= s; row[1] *= s; row[2] *= s;
    return *this;
}

inline Mat3& Mat3::operator/=(float8 s)
{
    row[0] /= s; row[1] /= s; row[2] /= s;
    return *this;
}

inline Mat3 Mat3::operator+(const Mat3& m) const
{
    return Mat3(row[0]+m.row[0],
		row[1]+m.row[1],
		row[2]+m.row[2]);
}

inline Mat3 Mat3::operator-(const Mat3& m) const
{
    return Mat3(row[0]-m.row[0],
		row[1]-m.row[1],
		row[2]-m.row[2]);
}

inline Mat3 Mat3::operator-() const
{
    return Mat3(-row[0], -row[1], -row[2]);
}

inline Mat3 Mat3::operator*(float8 s) const
{
    return Mat3(row[0]*s, row[1]*s, row[2]*s);
}

inline Mat3 Mat3::operator/(float8 s) const
{
    return Mat3(row[0]/s, row[1]/s, row[2]/s);
}

inline Vec3 Mat3::operator*(const Vec3& v) const
{
    return Vec3(row[0]*v, row[1]*v, row[2]*v);
}

inline ostream& operator<<(ostream& out, const Mat3& M)
{
    return out << M[0] << endl << M[1] << endl << M[2];
}

inline istream& operator>>(istream& in, Mat3& M)
{
    return in >> M[0] >> M[1] >> M[2];
}

// extern bool jacobi(const Mat3& m, Vec3& vals, Vec3 vecs[3]);
// extern bool jacobi(const Mat3& m, float8 *vals, float8 *vecs);
// extern bool fast_jacobi(const Mat3& m, Vec3& vals, Vec3 vecs[3]);

// MXMAT3_INCLUDED
#endif
