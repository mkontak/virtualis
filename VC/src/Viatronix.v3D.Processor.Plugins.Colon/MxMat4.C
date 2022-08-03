/************************************************************************

  4x4 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat4.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxMat4.h"

Mat4 Mat4::I(Vec4(1,0,0,0),Vec4(0,1,0,0),Vec4(0,0,1,0),Vec4(0,0,0,1));
Mat4 Mat4::zero(Vec4(0,0,0,0),Vec4(0,0,0,0),Vec4(0,0,0,0),Vec4(0,0,0,0));
Mat4 Mat4::unit(Vec4(1,1,1,1),Vec4(1,1,1,1),Vec4(1,1,1,1),Vec4(1,1,1,1));

Mat4 Mat4::trans(float8 x, float8 y, float8 z)
{
    return Mat4(Vec4(1,0,0,x),
		Vec4(0,1,0,y),
		Vec4(0,0,1,z),
		Vec4(0,0,0,1));
}

Mat4 Mat4::scale(float8 x, float8 y, float8 z)
{
    return Mat4(Vec4(x,0,0,0),
		Vec4(0,y,0,0),
		Vec4(0,0,z,0),
		Vec4(0,0,0,1));
}

Mat4 Mat4::xrot(float8 a)
{
    float8 c = cos(a);
    float8 s = sin(a);

    return Mat4(Vec4(1, 0, 0, 0),
		Vec4(0, c,-s, 0),
		Vec4(0, s, c, 0),
		Vec4(0, 0, 0, 1));
}

Mat4 Mat4::yrot(float8 a)
{
    float8 c = cos(a);
    float8 s = sin(a);

    return Mat4(Vec4(c, 0, s, 0),
		Vec4(0, 1, 0, 0),
		Vec4(-s,0, c, 0),
		Vec4(0, 0, 0, 1));
}

Mat4 Mat4::zrot(float8 a)
{
    float8 c = cos(a);
    float8 s = sin(a);

    return Mat4(Vec4(c,-s, 0, 0),
		Vec4(s, c, 0, 0),
		Vec4(0, 0, 1, 0),
		Vec4(0, 0, 0, 1));
}

Mat4 Mat4::operator*(const Mat4& m) const
{
    Mat4 A;
    int4 i,j;

    for(i=0;i<4;i++)
	for(j=0;j<4;j++)
	    A(i,j) = row[i]*m.col(j);

    return A;
}

float8 Mat4::det() const
{
    return row[0] * cross(row[1], row[2], row[3]);
}

Mat4 Mat4::transpose() const
{
    return Mat4(col(0), col(1), col(2), col(3));
}

Mat4 Mat4::adjoint() const
{
    Mat4 A;

    A.row[0] = cross( row[1], row[2], row[3]);
    A.row[1] = cross(-row[0], row[2], row[3]);
    A.row[2] = cross( row[0], row[1], row[3]);
    A.row[3] = cross(-row[0], row[1], row[2]);
        
    return A;
}

float8 Mat4::cramerInvert(Mat4& inv) const
{
    Mat4 A = adjoint();
    float8 d = A.row[0] * row[0];

    if( d==0.0 )
	return 0.0;

    inv = A.transpose() / d;
    return d;
}



// Matrix inversion code for 4x4 matrices.
// Originally ripped off and degeneralized from Paul's matrix library
// for the view synthesis (Chen) software.
//
// Returns determinant of a, and b=a inverse.
// If matrix is singular, returns 0 and leaves trash in b.
//
// Uses Gaussian elimination with partial pivoting.

#define SWAP(a, b, t)   {t = a; a = b; b = t;}
float8 Mat4::invert(Mat4& B) const
{
    Mat4 A(*this);

    int4 i, j, k;
    float8 max, t, det, pivot;

    /*---------- forward elimination ----------*/

    for (i=0; i<4; i++)                 /* put identity matrix in B */
        for (j=0; j<4; j++)
            B(i, j) = (float8)(i==j);

    det = 1.0;
    for (i=0; i<4; i++) {               /* eliminate in column i, below diag */
        max = -1.;
        for (k=i; k<4; k++)             /* find pivot for column i */
            if (fabs(A(k, i)) > max) {
                max = fabs(A(k, i));
                j = k;
            }
        if (max<=0.) return 0.;         /* if no nonzero pivot, PUNT */
        if (j!=i) {                     /* swap rows i and j */
            for (k=i; k<4; k++)
                SWAP(A(i, k), A(j, k), t);
            for (k=0; k<4; k++)
                SWAP(B(i, k), B(j, k), t);
            det = -det;
        }
        pivot = A(i, i);
        det *= pivot;
        for (k=i+1; k<4; k++)           /* only do elems to right of pivot */
            A(i, k) /= pivot;
        for (k=0; k<4; k++)
            B(i, k) /= pivot;
        /* we know that A(i, i) will be set to 1, so don't bother to do it */

        for (j=i+1; j<4; j++) {         /* eliminate in rows below i */
            t = A(j, i);                /* we're gonna zero this guy */
            for (k=i+1; k<4; k++)       /* subtract scaled row i from row j */
                A(j, k) -= A(i, k)*t;   /* (ignore k<=i, we know they're 0) */
            for (k=0; k<4; k++)
                B(j, k) -= B(i, k)*t;
        }
    }

    /*---------- backward elimination ----------*/

    for (i=4-1; i>0; i--) {             /* eliminate in column i, above diag */
        for (j=0; j<i; j++) {           /* eliminate in rows above i */
            t = A(j, i);                /* we're gonna zero this guy */
            for (k=0; k<4; k++)         /* subtract scaled row i from row j */
                B(j, k) -= B(i, k)*t;
        }
    }

    return det;
}
