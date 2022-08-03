/************************************************************************

  3x3 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat3.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxMat3.h"

Mat3 Mat3::I(Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1));
Mat3 Mat3::zero(Vec3(0,0,0), Vec3(0,0,0), Vec3(0,0,0));
Mat3 Mat3::unit(Vec3(1,1,1), Vec3(1,1,1), Vec3(1,1,1));

Mat3 Mat3::diag(const Vec3& v)
{
    Mat3 M(zero);

    M(0,0) = v[X];
    M(1,1) = v[Y];
    M(2,2) = v[Z];

    return M;
}

Mat3 Mat3::extend(const Vec3& v)
{
    return Mat3(v,v,v);
}

Mat3 Mat3::outer_product(const Vec3& v)
{
    Mat3 A;
    double x=v[X], y=v[Y], z=v[Z];

    A(0,0) = x*x;  A(0,1) = x*y;  A(0,2) = x*z;
    A(1,0)=A(0,1); A(1,1) = y*y;  A(1,2) = y*z;
    A(2,0)=A(0,2); A(2,1)=A(1,2); A(2,2) = z*z;

    return A;
}

Mat3 Mat3::operator*(const Mat3& m) const
{
    Mat3 A;
    int4 i,j;

    for(i=0;i<3;i++)
	for(j=0;j<3;j++)
	    A(i,j) = row[i]*m.col(j);

    return A;
}

double Mat3::det()
{
    return row[0] * (row[1] ^ row[2]);
}

double Mat3::trace()
{
    return row[0][0] + row[1][1] + row[2][2];
}

Mat3 Mat3::transpose()
{
    return Mat3(col(0), col(1), col(2));
}

Mat3 Mat3::adjoint()
{
    return Mat3(row[1]^row[2],
		row[2]^row[0],
		row[0]^row[1]);
}

double Mat3::invert(Mat3& inv)
{
    Mat3 A = adjoint();
    double d = A.row[0] * row[0];

    if( d==0.0 )
	return 0.0;

    inv = A.transpose() / d;
    return d;
}
