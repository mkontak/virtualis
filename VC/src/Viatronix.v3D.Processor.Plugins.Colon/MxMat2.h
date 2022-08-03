#ifndef MXMAT2_INCLUDED // -*- C++ -*-
#define MXMAT2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2x2 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat2.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec2.h"

class Mat2 
{
private:
    Vec2 row[2];

protected:

    inline void copy(const Mat2& m);
    inline Vec2 col(int4 i) const {return Vec2(row[0][i],row[1][i]);}

public:
    // Standard matrices
    static Mat2 I;
    static Mat2 zero;
    static Mat2 unit;

    // Standard constructors
    Mat2() { copy(zero); }
    Mat2(float8 a, float8 b, float8 c, float8 d)
	{ row[0][0]=a; row[0][1]=b; row[1][0]=c; row[1][1]=d; }
    Mat2(const Vec2& r0,const Vec2& r1) { row[0]=r0; row[1]=r1; }
    Mat2(const Mat2& m) { copy(m); }

    // Access methods
    // M(i, j) == row i;col j
    float8& operator()(int4 i, int4 j)       { return row[i][j]; }
    float8  operator()(int4 i, int4 j) const { return row[i][j]; }
    Vec2&       operator[](int4 i)       { return row[i]; }
    const Vec2& operator[](int4 i) const { return row[i]; }

    operator       float8*()       { return row[0]; }
    operator const float8*() const { return row[0]; }

    // Assignment methods
    inline Mat2& operator=(const Mat2& m) { copy(m); return *this; }
    inline Mat2& operator+=(const Mat2& m);
    inline Mat2& operator-=(const Mat2& m);

    inline Mat2& operator*=(float8 s);
    inline Mat2& operator/=(float8 s);


    // Arithmetic methods
    inline Mat2 operator+(const Mat2& m) const;
    inline Mat2 operator-(const Mat2& m) const;
    inline Mat2 operator-() const;

    inline Mat2 operator*(float8 s) const;
    inline Mat2 operator/(float8 s) const;
    Mat2 operator*(const Mat2& m) const;

    inline Vec2 operator*(const Vec2& v) const; // [x y]

    // Matrix operations
    float8 det();
    Mat2 transpose();
    float8 invert(Mat2&);
};



inline void Mat2::copy(const Mat2& m)
{
    row[0] = m.row[0]; row[1] = m.row[1];
}

inline Mat2& Mat2::operator+=(const Mat2& m)
{
    row[0] += m.row[0]; row[1] += m.row[1];
    return *this;
}

inline Mat2& Mat2::operator-=(const Mat2& m)
{
    row[0] -= m.row[0]; row[1] -= m.row[1];
    return *this;
}

inline Mat2& Mat2::operator*=(float8 s)
{
    row[0] *= s; row[1] *= s;
    return *this;
}

inline Mat2& Mat2::operator/=(float8 s)
{
    row[0] /= s; row[1] /= s;
    return *this;
}

inline Mat2 Mat2::operator+(const Mat2& m) const
{
    return Mat2(row[0]+m.row[0],
		row[1]+m.row[1]);
}

inline Mat2 Mat2::operator-(const Mat2& m) const
{
    return Mat2(row[0]-m.row[0],
		row[1]-m.row[1]);
}

inline Mat2 Mat2::operator-() const
{
    return Mat2(-row[0], -row[1]);
}

inline Mat2 Mat2::operator*(float8 s) const
{
    return Mat2(row[0]*s, row[1]*s);
}

inline Mat2 Mat2::operator/(float8 s) const
{
    return Mat2(row[0]/s, row[1]/s);
}

inline Vec2 Mat2::operator*(const Vec2& v) const
{
    return Vec2(row[0]*v, row[1]*v);
}

inline ostream& operator<<(ostream& out, const Mat2& M)
{
    return out << M[0] << endl  << M[1];
}

inline istream& operator>>(istream& in, Mat2& M)
{
    return in >> M[0] >> M[1];
}

// MXMAT2_INCLUDED
#endif
